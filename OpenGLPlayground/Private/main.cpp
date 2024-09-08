#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <stack>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Utils.h"
#include "Sphere.h"
#include "Torus.h"
#include "ImportedModel.h"

constexpr GLuint SCR_WIDTH = 800;
constexpr GLuint SCR_HEIGHT = 600;
constexpr GLuint NUM_VAOS = 1;
constexpr GLuint NUM_VBOS = 17;
constexpr GLsizei cubeStride = 8 * sizeof(float);

std::string resourcePath;
float cameraX, cameraY, cameraZ;
GLuint renderingProgram1, renderingProgram2;
GLuint vao[NUM_VAOS];
GLuint vbo[NUM_VBOS];
Sphere mySphere(48);
Torus myTorus(0.5f, 0.2f, 48);
ImportedModel myShuttle("shuttle.obj");
ImportedModel myDolphin("dolphinHighPoly.obj");

// allocate variables used in display() function, so that they won’t need to be allocated during rendering
int width, height;
float aspect;
glm::mat4 mMat, vMat, pMat, invTrMat;
glm::vec3 currLightPos, lightPosV;
float lightPos[3];
std::stack<glm::mat4> trfmStack;
GLuint brickTexture;
GLuint earthTexture;
GLuint shuttleTexture;

// shader uniform locations
GLuint mLoc, vLoc, pLoc, nLoc, shLoc;
GLuint lightAmbLoc, lightDifLoc, lightSpeLoc, lightPosLoc;
GLuint matAmbLoc, matDifLoc, matSpeLoc, matShiLoc;
GLuint globalAmbLoc, winSizeLoc;

// light properties
glm::vec3 initLightPos(-5.0f, 3.0f, 4.0f);
float globalAmb[4] = { 0.7f, 0.7f, 0.7f, 1.0f };
float lightAmb[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
float lightDif[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
float lightSpe[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

// material properties
float* matAmb = Utils::silverAmbient();
float* matDif = Utils::silverDiffuse();
float* matSpe = Utils::silverSpecular();
float matShi = Utils::silverShininess();

// shadow-related variables
int screenSizeX, screenSizeY;
GLuint shadowTex, shadowBuffer;
glm::mat4 lightVmatrix;
glm::mat4 lightPmatrix;
glm::mat4 shadowMVP;
glm::mat4 b;

// compute shader variables
GLuint ssbo[3];
GLuint computeShader;
int csInput1[] = { 10, 12, 16, 18, 50, 17 };
int csInput2[] = { 30, 14, 80, 20, 51, 12 };
int csOutput[6];

void calcPyramidNormals(const float* verts, float* outNormals)
{
    for (int i = 0; i < 54; i+=9)
    {
        float faceVerts[9];
        std::copy(verts+i, verts+i+9, faceVerts);
        float norm[3];
		Utils::calculateNormal(faceVerts, norm);
        std::copy(norm, norm+3, outNormals+i);
        std::copy(norm, norm+3, outNormals+i+3);
        std::copy(norm, norm+3, outNormals+i+6);
    }
}

void setupVertices()
{
    // 36 vertices, 12 triangles, makes 2x2x2 cube placed at origin
    float cubeData[] =
    {
        // Position            // Normals           // Texture Coords
        -1.0f,  1.0f, -1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 1.0f, // Front face
        -1.0f, -1.0f, -1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f,
         1.0f, -1.0f, -1.0f,    0.0f, 0.0f, 1.0f,    1.0f, 0.0f,
         1.0f, -1.0f, -1.0f,    0.0f, 0.0f, 1.0f,    1.0f, 0.0f,
         1.0f,  1.0f, -1.0f,    0.0f, 0.0f, 1.0f,    1.0f, 1.0f,
        -1.0f,  1.0f, -1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 1.0f,
                                                    
         1.0f, -1.0f, -1.0f,    1.0f, 0.0f, 0.0f,    0.0f, 0.0f, // Right face
         1.0f, -1.0f,  1.0f,    1.0f, 0.0f, 0.0f,    1.0f, 0.0f,
         1.0f,  1.0f,  1.0f,    1.0f, 0.0f, 0.0f,    1.0f, 1.0f,
         1.0f,  1.0f,  1.0f,    1.0f, 0.0f, 0.0f,    1.0f, 1.0f,
         1.0f,  1.0f, -1.0f,    1.0f, 0.0f, 0.0f,    0.0f, 1.0f,
         1.0f, -1.0f, -1.0f,    1.0f, 0.0f, 0.0f,    0.0f, 0.0f,
                               
         1.0f, -1.0f,  1.0f,    0.0f, 0.0f, -1.0f,   1.0f, 0.0f, // Back face
        -1.0f, -1.0f,  1.0f,    0.0f, 0.0f, -1.0f,   0.0f, 0.0f,
        -1.0f,  1.0f,  1.0f,    0.0f, 0.0f, -1.0f,   0.0f, 1.0f,
        -1.0f,  1.0f,  1.0f,    0.0f, 0.0f, -1.0f,   0.0f, 1.0f,
         1.0f,  1.0f,  1.0f,    0.0f, 0.0f, -1.0f,   1.0f, 1.0f,
         1.0f, -1.0f,  1.0f,    0.0f, 0.0f, -1.0f,   1.0f, 0.0f,
                               
        -1.0f, -1.0f,  1.0f,   -1.0f, 0.0f, 0.0f,    1.0f, 0.0f, // Left face
        -1.0f, -1.0f, -1.0f,   -1.0f, 0.0f, 0.0f,    0.0f, 0.0f,
        -1.0f,  1.0f, -1.0f,   -1.0f, 0.0f, 0.0f,    0.0f, 1.0f,
        -1.0f,  1.0f, -1.0f,   -1.0f, 0.0f, 0.0f,    0.0f, 1.0f,
        -1.0f,  1.0f,  1.0f,   -1.0f, 0.0f, 0.0f,    1.0f, 1.0f,
        -1.0f, -1.0f,  1.0f,   -1.0f, 0.0f, 0.0f,    1.0f, 0.0f,

        -1.0f,  1.0f, -1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 1.0f, // Top face
         1.0f,  1.0f, -1.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f,
         1.0f,  1.0f,  1.0f,    0.0f, 1.0f, 0.0f,    1.0f, 0.0f,
         1.0f,  1.0f,  1.0f,    0.0f, 1.0f, 0.0f,    1.0f, 0.0f,
        -1.0f,  1.0f,  1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f,
        -1.0f,  1.0f, -1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 1.0f,

        -1.0f, -1.0f, -1.0f,    0.0f, -1.0f, 0.0f,   1.0f, 0.0f, // Bottom face
         1.0f, -1.0f, -1.0f,    0.0f, -1.0f, 0.0f,   0.0f, 0.0f,
         1.0f, -1.0f,  1.0f,    0.0f, -1.0f, 0.0f,   0.0f, 1.0f,
         1.0f, -1.0f,  1.0f,    0.0f, -1.0f, 0.0f,   0.0f, 1.0f,
        -1.0f, -1.0f,  1.0f,    0.0f, -1.0f, 0.0f,   1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,    0.0f, -1.0f, 0.0f,   1.0f, 0.0f
    };

    // pyramid with 18 vertices, comprising 6 triangles (four sides, and two on the bottom)
    float pyrVerts[54] =
    {
        -1.0f, -1.0f, 1.0f,  // front face
         1.0f, -1.0f, 1.0f,
         0.0f, 1.0f, 0.0f,
         1.0f, -1.0f, 1.0f,  // right face
         1.0f, -1.0f, -1.0f,
         0.0f, 1.0f, 0.0f,
         1.0f, -1.0f, -1.0f, // back face
        -1.0f, -1.0f, -1.0f,
         0.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, -1.0f, // left face
        -1.0f, -1.0f, 1.0f,
         0.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, -1.0f, // base left front
         1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,
         1.0f, -1.0f, 1.0f,  // base right back
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f
    };

    float pyrTexCoords[36] =
    {
         0.0f, 0.0f, // front face
         1.0f, 0.0f, 
         0.5f, 1.0f,    
         0.0f, 0.0f, // right face
         1.0f, 0.0f,  
         0.5f, 1.0f,
         0.0f, 0.0f, // back face
         1.0f, 0.0f,  
         0.5f, 1.0f,
         0.0f, 0.0f, // left face
         1.0f, 0.0f,
         0.5f, 1.0f,
         0.0f, 0.0f, // base triangle 1
         1.0f, 1.0f,
         0.0f, 1.0f,
         1.0f, 1.0f, // base triangle 2
         0.0f, 0.0f,
         1.0f, 0.0f
    };

    float pyrNorms[54];
    calcPyramidNormals(pyrVerts, pyrNorms);

    glGenVertexArrays(NUM_VAOS, vao);
    glBindVertexArray(*vao);

    glGenBuffers(NUM_VBOS, vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeData), cubeData, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pyrVerts), pyrVerts, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pyrTexCoords), pyrTexCoords, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[13]);
    glBufferData(GL_ARRAY_BUFFER, 54 * sizeof(float), pyrNorms, GL_STATIC_DRAW);

    // ------------------------------ procedural sphere -------------------------------
    std::vector<int> sphIdxs = mySphere.getIndices();
    std::vector<glm::vec3> sphVerts = mySphere.getVertices();
    std::vector<glm::vec2> sphTexs = mySphere.getTexCoords();
    std::vector<glm::vec3> sphNorms = mySphere.getNormals();
    std::vector<float> sphPosVals; // vertex positions
    std::vector<float> sphTexVals; // texture coordinates
    std::vector<float> sphNormVals; // normal vectors

    // flatten every vector into array of floats
    int sphNumIdxs = mySphere.getNumIndices();
    for (int i = 0; i < sphNumIdxs; i++)
    {
        sphPosVals.push_back(sphVerts[sphIdxs[i]].x);
        sphPosVals.push_back(sphVerts[sphIdxs[i]].y);
        sphPosVals.push_back(sphVerts[sphIdxs[i]].z);

        sphTexVals.push_back(sphTexs[sphIdxs[i]].s);
        sphTexVals.push_back(sphTexs[sphIdxs[i]].t);

        sphNormVals.push_back(sphNorms[sphIdxs[i]].x);
        sphNormVals.push_back(sphNorms[sphIdxs[i]].y);
        sphNormVals.push_back(sphNorms[sphIdxs[i]].z);
    }

    // put the vertices into buffer #4
    glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
    glBufferData(GL_ARRAY_BUFFER, sphPosVals.size() * 4, &sphPosVals[0], GL_STATIC_DRAW);
    // put the texture coordinates into buffer #5
    glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
    glBufferData(GL_ARRAY_BUFFER, sphTexVals.size() * 4, &sphTexVals[0], GL_STATIC_DRAW);
    // put the normals into buffer #6
    glBindBuffer(GL_ARRAY_BUFFER, vbo[5]);
    glBufferData(GL_ARRAY_BUFFER, sphNormVals.size() * 4, &sphNormVals[0], GL_STATIC_DRAW);
    // ----------------------------------------------------------------------------------

    // ------------------------------ procedural torus ----------------------------------
    std::vector<int> torIdxs = myTorus.getIndices();
    std::vector<glm::vec3> torVerts = myTorus.getVertices();
    std::vector<glm::vec2> torTexs = myTorus.getTexCoords();
    std::vector<glm::vec3> torNorms = myTorus.getNormals();
    std::vector<float> torPosVals; // vertex positions
    std::vector<float> torTexVals; // texture coordinates
    std::vector<float> torNormVals; // normal vectors

    // flatten every vector into array of floats
    int torNumVerts = myTorus.getNumVertices();
    for (int i = 0; i < torNumVerts; i++)
    {
        torPosVals.push_back(torVerts[i].x);
        torPosVals.push_back(torVerts[i].y);
        torPosVals.push_back(torVerts[i].z);

        torTexVals.push_back(torTexs[i].s);
        torTexVals.push_back(torTexs[i].t);

        torNormVals.push_back(torNorms[i].x);
        torNormVals.push_back(torNorms[i].y);
        torNormVals.push_back(torNorms[i].z);
    }

    // put the vertices into buffer #7
    glBindBuffer(GL_ARRAY_BUFFER, vbo[6]);
    glBufferData(GL_ARRAY_BUFFER, torPosVals.size() * 4, &torPosVals[0], GL_STATIC_DRAW);
    // put the texture coordinates into buffer #8
    glBindBuffer(GL_ARRAY_BUFFER, vbo[7]);
    glBufferData(GL_ARRAY_BUFFER, torTexVals.size() * 4, &torTexVals[0], GL_STATIC_DRAW);
    // put the normals into buffer #9
    glBindBuffer(GL_ARRAY_BUFFER, vbo[8]);
    glBufferData(GL_ARRAY_BUFFER, torNormVals.size() * 4, &torNormVals[0], GL_STATIC_DRAW);
    // put the indices into buffer #10
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[9]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, torIdxs.size() * 4, &torIdxs[0], GL_STATIC_DRAW);
    // ----------------------------------------------------------------------------------

    // ------------------------------- imported shuttle -----------------------------------
    std::vector<glm::vec3> shuVerts = myShuttle.getVertices();
    std::vector<glm::vec2> shuTexs = myShuttle.getTextureCoords();
    std::vector<glm::vec3> shuNorms = myShuttle.getNormals();
    std::vector<float> shuPosVals; // vertex positions
    std::vector<float> shuTexVals; // texture coordinates
    std::vector<float> shuNormVals; // normal vectors

    int shuNumVertices = myShuttle.getNumVertices();
    for (int i = 0; i < shuNumVertices; i++)
    {
        shuPosVals.push_back(shuVerts[i].x);
        shuPosVals.push_back(shuVerts[i].y);
        shuPosVals.push_back(shuVerts[i].z);
        shuTexVals.push_back(shuTexs[i].s);
        shuTexVals.push_back(shuTexs[i].t);
        shuNormVals.push_back(shuNorms[i].x);
        shuNormVals.push_back(shuNorms[i].y);
        shuNormVals.push_back(shuNorms[i].z);
    }

    // put the vertices into buffer #11
    glBindBuffer(GL_ARRAY_BUFFER, vbo[10]);
    glBufferData(GL_ARRAY_BUFFER, shuPosVals.size() * 4, &shuPosVals[0], GL_STATIC_DRAW);
    // put the texture coordinates into buffer #12
    glBindBuffer(GL_ARRAY_BUFFER, vbo[11]);
    glBufferData(GL_ARRAY_BUFFER, shuTexVals.size() * 4, &shuTexVals[0], GL_STATIC_DRAW);
    // put the normals into buffer #13
    glBindBuffer(GL_ARRAY_BUFFER, vbo[12]);
    glBufferData(GL_ARRAY_BUFFER, shuNormVals.size() * 4, &shuNormVals[0], GL_STATIC_DRAW);
    // ------------------------------------------------------------------------------------

        // ------------------------------- imported dolphin -----------------------------------
    std::vector<glm::vec3> dolVerts = myDolphin.getVertices();
    std::vector<glm::vec2> dolTexs = myDolphin.getTextureCoords();
    std::vector<glm::vec3> dolNorms = myDolphin.getNormals();
    std::vector<float> dolPosVals; // vertex positions
    std::vector<float> dolTexVals; // texture coordinates
    std::vector<float> dolNormVals; // normal vectors

    int dolNumVertices = myDolphin.getNumVertices();
    for (int i = 0; i < dolNumVertices; i++)
    {
        dolPosVals.push_back(dolVerts[i].x);
        dolPosVals.push_back(dolVerts[i].y);
        dolPosVals.push_back(dolVerts[i].z);
        dolTexVals.push_back(dolTexs[i].s);
        dolTexVals.push_back(dolTexs[i].t);
        dolNormVals.push_back(dolNorms[i].x);
        dolNormVals.push_back(dolNorms[i].y);
        dolNormVals.push_back(dolNorms[i].z);
    }

    // put the vertices into buffer #11
    glBindBuffer(GL_ARRAY_BUFFER, vbo[14]);
    glBufferData(GL_ARRAY_BUFFER, dolPosVals.size() * 4, &dolPosVals[0], GL_STATIC_DRAW);
    // put the texture coordinates into buffer #12
    glBindBuffer(GL_ARRAY_BUFFER, vbo[15]);
    glBufferData(GL_ARRAY_BUFFER, dolTexVals.size() * 4, &dolTexVals[0], GL_STATIC_DRAW);
    // put the normals into buffer #13
    glBindBuffer(GL_ARRAY_BUFFER, vbo[16]);
    glBufferData(GL_ARRAY_BUFFER, dolNormVals.size() * 4, &dolNormVals[0], GL_STATIC_DRAW);
    // ----------------------------------------------------------------------------------------
}

void setupShadowBuffers(GLFWwindow* window)
{
    glfwGetFramebufferSize(window, &width, &height);
    
    glGenFramebuffers(1, &shadowBuffer); // create the custom frame buffer

    // create the shadow texture and configure it to hold depth information
    glGenTextures(1, &shadowTex);
    glBindTexture(GL_TEXTURE_2D, shadowTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
}

void init(GLFWwindow* window)
{
    resourcePath = Utils::getResourcePath();
    std::string vert1ShaderPath = resourcePath + "vert1Shader.glsl";
    std::string frag1ShaderPath = resourcePath + "frag1Shader.glsl";
    std::string vert2ShaderPath = resourcePath + "vert2Shader.glsl";
    std::string frag2ShaderPath = resourcePath + "frag2Shader.glsl";
    renderingProgram1 = Utils::createShaderProgram(vert1ShaderPath.c_str(), frag1ShaderPath.c_str());
    renderingProgram2 = Utils::createShaderProgram(vert2ShaderPath.c_str(), frag2ShaderPath.c_str());

    cameraX = 0.0f; cameraY = 0.0f; cameraZ = 8.0f;
    currLightPos = glm::vec3(initLightPos);

    setupVertices();
    setupShadowBuffers(window);

    b = glm::mat4(
        0.5f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.5f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.5f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f);

    // build perspective matrix
    glfwGetFramebufferSize(window, &width, &height);
    aspect = (float)width / (float)height;
    pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f); // 1.0472 radians = 60 degrees

    brickTexture = Utils::loadTexture(resourcePath, "brick1.jpg");
    earthTexture = Utils::loadTexture(resourcePath, "earthmap1k.jpg");
    shuttleTexture = Utils::loadTexture(resourcePath, "spstob_1.jpg");

    // --- compute shader setup ---
    std::string compShaderPath = resourcePath + "compShader.glsl";
    computeShader = Utils::createShaderProgram(compShaderPath.c_str());

    glGenBuffers(3, ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo[0]);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(csInput1), csInput1, GL_STATIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo[1]);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(csInput2), csInput2, GL_STATIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo[2]);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(csOutput), csOutput, GL_STATIC_READ);
    // ----------------------------
}

void computeSum()
{
    glUseProgram(computeShader);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo[0]);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo[1]);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ssbo[2]);

    glDispatchCompute(6, 1, 1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo[2]);
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(csOutput), csOutput);
}

void installLights(GLuint renderingProgram)
{
    // save the light position in a float array
    lightPos[0] = currLightPos.x;
    lightPos[1] = currLightPos.y;
    lightPos[2] = currLightPos.z;

    // get the locations of the light and material fields in the shader
    globalAmbLoc = glGetUniformLocation(renderingProgram, "globalAmb");
    lightAmbLoc = glGetUniformLocation(renderingProgram, "light.ambient");
    lightDifLoc = glGetUniformLocation(renderingProgram, "light.diffuse");
    lightSpeLoc = glGetUniformLocation(renderingProgram, "light.specular");
    lightPosLoc = glGetUniformLocation(renderingProgram, "light.position");
    matAmbLoc = glGetUniformLocation(renderingProgram, "material.ambient");
    matDifLoc = glGetUniformLocation(renderingProgram, "material.diffuse");
    matSpeLoc = glGetUniformLocation(renderingProgram, "material.specular");
    matShiLoc = glGetUniformLocation(renderingProgram, "material.shininess");

    // set the uniform light and material values in the shader
    glProgramUniform4fv(renderingProgram, globalAmbLoc, 1, globalAmb);
    glProgramUniform4fv(renderingProgram, lightAmbLoc, 1, lightAmb);
    glProgramUniform4fv(renderingProgram, lightDifLoc, 1, lightDif);
    glProgramUniform4fv(renderingProgram, lightSpeLoc, 1, lightSpe);
    glProgramUniform3fv(renderingProgram, lightPosLoc, 1, lightPos);
    glProgramUniform4fv(renderingProgram, matAmbLoc, 1, matAmb);
    glProgramUniform4fv(renderingProgram, matDifLoc, 1, matDif);
    glProgramUniform4fv(renderingProgram, matSpeLoc, 1, matSpe);
    glProgramUniform1f(renderingProgram, matShiLoc, matShi);
}

void passOne(GLFWwindow* window, double currentTime)
{
    glUseProgram(renderingProgram1);

    glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL); // passes if the incoming depth value is less than or equal to the stored depth value

    // reference uniform variables
    shLoc = glGetUniformLocation(renderingProgram1, "sh_mvp_matrix");

    trfmStack.push(glm::mat4(1.0f)); // + initial matrix

    // ---------------------- pyramid == sun --------------------------------------------
    trfmStack.push(trfmStack.top()); // ++ copy default matrix
    trfmStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)); // sun position
    trfmStack.push(trfmStack.top()); // +++ push another transform because we want child objects to be relative to the translation above
    trfmStack.top() *= glm::rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(1.0f, 0.0f, 0.0f)); // sun rotation
    mMat = trfmStack.top();

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glFrontFace(GL_CCW); // the pyramid vertices have counter-clockwise winding order
        // --- pyramid shadowing ---
    shadowMVP = lightPmatrix * lightVmatrix * mMat;
    glUniformMatrix4fv(shLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP));
        // ------------------------------
    glDrawArrays(GL_TRIANGLES, 0, 18); // draw the sun
    trfmStack.pop(); // ++ sun's axial rotation removed
    // ----------------------------------------------------------------------------------

    // ----------------------- cube == planet -------------------------------------------
    trfmStack.push(trfmStack.top()); // +++ inherit sun's translation
    trfmStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(sin((float)currentTime) * 4.0, 0.0f, cos((float)currentTime) * 4.0));
    trfmStack.push(trfmStack.top()); // ++++ push another transform because we want child objects to be relative to the translation above
    trfmStack.top() *= glm::rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0, 1.0, 0.0)); // planet rotation
    trfmStack.top() *= glm::scale(glm::mat4(1.0f), glm::vec3(0.75f, 0.75f, 0.75f));
    mMat = trfmStack.top();
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, cubeStride, 0);
    glEnableVertexAttribArray(0);
    glFrontFace(GL_CW); // the cube vertices have clockwise winding order
        // --- cube shadowing ---
    shadowMVP = lightPmatrix * lightVmatrix * mMat;
    glUniformMatrix4fv(shLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP));
        // ----------------------
    glDrawArrays(GL_TRIANGLES, 0, 36); // draw the planet
    trfmStack.pop(); // +++ planet's rotation axis and scaling removed

    // ----------------------- smaller cube == moon -------------------------------------
    trfmStack.push(trfmStack.top()); // ++++ inherit planet's translation
    trfmStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, sin((float)currentTime) * 2.0, cos((float)currentTime) * 2.0));
    trfmStack.top() *= glm::rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0, 0.0, 1.0)); // moon rotation
    trfmStack.top() *= glm::scale(glm::mat4(1.0f), glm::vec3(0.25f, 0.25f, 0.25f)); // make the moon smaller
    mMat = trfmStack.top();

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, cubeStride, 0);
    glEnableVertexAttribArray(0);
        // --- smaller cube shadowing ---
    shadowMVP = lightPmatrix * lightVmatrix * mMat;
    glUniformMatrix4fv(shLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP));
        // ------------------------------
    glDrawArrays(GL_TRIANGLES, 0, 36); // draw the moon

    trfmStack.pop(); // +++ remove moon's transformations
    trfmStack.pop(); // ++ remove planet's translation
    // ----------------------------------------------------------------------------------

    // ------------------------------ procedural sphere ---------------------------------
    trfmStack.push(trfmStack.top()); // +++ inherit sun's translation
    trfmStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 0.0f, 0.0f));
    trfmStack.top() *= glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
    trfmStack.top() *= glm::rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0f, 1.0f, 0.0f));
    mMat = trfmStack.top();

    glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glFrontFace(GL_CCW); // the sphere vertices have clockwise winding order
        // --- sphere shadowing ---
    shadowMVP = lightPmatrix * lightVmatrix * mMat;
    glUniformMatrix4fv(shLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP));
        // ------------------------
    glDrawArrays(GL_TRIANGLES, 0, mySphere.getNumIndices());

    trfmStack.pop(); // ++ remove procedural sphere's transformations
    // ----------------------------------------------------------------------------------

    // ------------------------------ procedural torus ----------------------------------
    trfmStack.push(trfmStack.top()); // +++ inherit sun's translation
    trfmStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, 0.0f));
    trfmStack.top() *= glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f));
    trfmStack.top() *= glm::rotate(glm::mat4(1.0f), Utils::toRadians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    trfmStack.top() *= glm::rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0f, -1.0, 0.0f));
    mMat = trfmStack.top();

    glBindBuffer(GL_ARRAY_BUFFER, vbo[6]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glFrontFace(GL_CCW);
        // --- torus shadowing ----
    shadowMVP = lightPmatrix * lightVmatrix * mMat;
    glUniformMatrix4fv(shLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP));
        // ------------------------
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[9]);
    glDrawElements(GL_TRIANGLES, myTorus.getNumIndices(), GL_UNSIGNED_INT, 0);

    trfmStack.pop(); // ++ remove procedural torus's transformations
    // ----------------------------------------------------------------------------------

    // ------------------------------- imported shuttle -----------------------------------
    trfmStack.push(trfmStack.top()); // +++ inherit sun's translation
    trfmStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(cos((float)currentTime) * 4.0f, sin((float)currentTime) * 4.0f, cos((float)currentTime) * 4.0f));
    trfmStack.top() *= glm::rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(1.0, 1.0, 0.0));
    trfmStack.top() *= glm::scale(glm::mat4(1.0f), glm::vec3(4.0f, 4.0f, 4.0f));
    mMat = trfmStack.top();

    glBindBuffer(GL_ARRAY_BUFFER, vbo[10]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glFrontFace(GL_CCW);
        // --- shuttle shadowing ----
    shadowMVP = lightPmatrix * lightVmatrix * mMat;
    glUniformMatrix4fv(shLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP));
        // --------------------------
    glDrawArrays(GL_TRIANGLES, 0, myShuttle.getNumVertices());

    trfmStack.pop(); // ++ remove shuttle's transformations
    // ------------------------------------------------------------------------------------

    // ------------------------------- imported dolphin -----------------------------------
    trfmStack.push(trfmStack.top()); // +++ inherit sun's translation
    trfmStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(cos((float)currentTime) * 4.0f, -sin((float)currentTime) * 4.0f, -cos((float)currentTime) * 4.0f));
    trfmStack.top() *= glm::rotate(glm::mat4(1.0f), -(float)currentTime, glm::vec3(1.0, 1.0, 0.0));
    trfmStack.top() *= glm::scale(glm::mat4(1.0f), glm::vec3(4.0f, 4.0f, 4.0f));
    mMat = trfmStack.top();

    glBindBuffer(GL_ARRAY_BUFFER, vbo[14]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glFrontFace(GL_CCW);
        // --- dolphin shadowing ----
    shadowMVP = lightPmatrix * lightVmatrix * mMat;
    glUniformMatrix4fv(shLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP));
        // --------------------------
    glDrawArrays(GL_TRIANGLES, 0, myDolphin.getNumVertices());

    trfmStack.pop(); // ++ remove dolphin's transformations
    // ------------------------------------------------------------------------------------

    trfmStack.pop(); // + remove sun's translation
    trfmStack.pop(); // remove initial matrix

}

void passTwo(GLFWwindow* window, double currentTime)
{
    glUseProgram(renderingProgram2);

    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // reference uniform variables
    mLoc = glGetUniformLocation(renderingProgram2, "m_matrix");
    vLoc = glGetUniformLocation(renderingProgram2, "v_matrix");
    pLoc = glGetUniformLocation(renderingProgram2, "p_matrix");
    nLoc = glGetUniformLocation(renderingProgram2, "n_matrix");
    shLoc = glGetUniformLocation(renderingProgram2, "sh_mvp_matrix");
    winSizeLoc = glGetUniformLocation(renderingProgram2, "windowSize");

    // copy perspective matrix
    glUniformMatrix4fv(pLoc, 1, GL_FALSE, glm::value_ptr(pMat));

    // build and copy view matrix
    vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));
    glUniformMatrix4fv(vLoc, 1, GL_FALSE, glm::value_ptr(vMat));

    // copy window size
    glUniform2f(winSizeLoc, (float)width, (float)height);

    // set up lights based on the current light's position
    currLightPos = glm::vec3(initLightPos);
    installLights(renderingProgram2);

    trfmStack.push(glm::mat4(1.0f)); // + initial matrix

    // ---------------------- pyramid == sun --------------------------------------------
    trfmStack.push(trfmStack.top()); // ++ copy default matrix
    trfmStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)); // sun position
    trfmStack.push(trfmStack.top()); // +++ push another transform because we want child objects to be relative to the translation above
    trfmStack.top() *= glm::rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(1.0f, 0.0f, 0.0f)); // sun rotation
    mMat = trfmStack.top();
    glUniformMatrix4fv(mLoc, 1, GL_FALSE, glm::value_ptr(mMat));

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glFrontFace(GL_CCW); // the pyramid vertices have counter-clockwise winding order
        // --- pyramid texturing ---
    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, brickTexture);
        // -------------------------
        // --- pyramid lighting ---
    glBindBuffer(GL_ARRAY_BUFFER, vbo[13]);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);
    invTrMat = glm::transpose(glm::inverse(mMat));
    glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
        // ------------------------
	    // --- pyramid shadowing ---
    shadowMVP = b * lightPmatrix * lightVmatrix * mMat;
	glUniformMatrix4fv(shLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP));
        // -------------------------
    glDrawArrays(GL_TRIANGLES, 0, 18); // draw the sun
    trfmStack.pop(); // ++ sun's axial rotation removed
    // ----------------------------------------------------------------------------------

    // ----------------------- cube == planet -------------------------------------------
    trfmStack.push(trfmStack.top()); // +++ inherit sun's translation
    trfmStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(sin((float)currentTime) * 4.0, 0.0f, cos((float)currentTime) * 4.0));
    trfmStack.push(trfmStack.top()); // ++++ push another transform because we want child objects to be relative to the translation above
    trfmStack.top() *= glm::rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0, 1.0, 0.0)); // planet rotation
    trfmStack.top() *= glm::scale(glm::mat4(1.0f), glm::vec3(0.75f, 0.75f, 0.75f));
    mMat = trfmStack.top();
    glUniformMatrix4fv(mLoc, 1, GL_FALSE, glm::value_ptr(mMat));

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, cubeStride, 0);
    glEnableVertexAttribArray(0);
    glFrontFace(GL_CW); // the cube vertices have clockwise winding order
        // --- cube texturing ---
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, cubeStride, (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, brickTexture);
        // ----------------------
        // --- cube lighting ---
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, cubeStride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    invTrMat = glm::transpose(glm::inverse(mMat));
    glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
        // ---------------------
        // --- cube shadowing ---
    shadowMVP = b * lightPmatrix * lightVmatrix * mMat;
    glUniformMatrix4fv(shLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP));
        // -------------------------
    glDrawArrays(GL_TRIANGLES, 0, 36); // draw the planet
    trfmStack.pop(); // +++ planet's rotation axis and scaling removed

    // ----------------------- smaller cube == moon -------------------------------------
    trfmStack.push(trfmStack.top()); // ++++ inherit planet's translation
    trfmStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, sin((float)currentTime) * 2.0, cos((float)currentTime) * 2.0));
    trfmStack.top() *= glm::rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0, 0.0, 1.0)); // moon rotation
    trfmStack.top() *= glm::scale(glm::mat4(1.0f), glm::vec3(0.25f, 0.25f, 0.25f)); // make the moon smaller
    mMat = trfmStack.top();
    glUniformMatrix4fv(mLoc, 1, GL_FALSE, glm::value_ptr(mMat));

    // recalculate inverse-transpose of M matrix
    invTrMat = glm::transpose(glm::inverse(mMat));
    glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, cubeStride, 0);
    glEnableVertexAttribArray(0);
    glBindTexture(GL_TEXTURE_2D, 0); // unbind texture
    glDrawArrays(GL_TRIANGLES, 0, 36); // draw the moon

    trfmStack.pop(); // +++ remove moon's transformations
    trfmStack.pop(); // ++ remove planet's translation
    // ----------------------------------------------------------------------------------

    // ------------------------------ procedural sphere ---------------------------------
    trfmStack.push(trfmStack.top()); // +++ inherit sun's translation
    trfmStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 0.0f, 0.0f));
    trfmStack.top() *= glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
    trfmStack.top() *= glm::rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0f, 1.0f, 0.0f));
    mMat = trfmStack.top();
    glUniformMatrix4fv(mLoc, 1, GL_FALSE, glm::value_ptr(mMat));

    glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glFrontFace(GL_CCW); // the sphere vertices have clockwise winding order
	    // --- sphere texturing ---
    glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0); // specify layout of tex coords
    glEnableVertexAttribArray(1); // enable vert shader to access tex coords stored in VBO
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, earthTexture);
        // ------------------------
        // --- sphere lighting ---
    glBindBuffer(GL_ARRAY_BUFFER, vbo[5]);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);
    invTrMat = glm::transpose(glm::inverse(mMat));
    glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
        // -----------------------
        // --- sphere shadowing ---
    shadowMVP = b * lightPmatrix * lightVmatrix * mMat;
    glUniformMatrix4fv(shLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP));
        // ------------------------
    glDrawArrays(GL_TRIANGLES, 0, mySphere.getNumIndices());

    trfmStack.pop(); // ++ remove sphere's transformations
    // ----------------------------------------------------------------------------------

    // ------------------------------ procedural torus ----------------------------------
    trfmStack.push(trfmStack.top()); // +++ inherit sun's translation
    trfmStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, 0.0f));
    trfmStack.top() *= glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f));
    trfmStack.top() *= glm::rotate(glm::mat4(1.0f), Utils::toRadians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    trfmStack.top() *= glm::rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0f, -1.0, 0.0f));
    mMat = trfmStack.top();
    glUniformMatrix4fv(mLoc, 1, GL_FALSE, glm::value_ptr(mMat));

    glBindBuffer(GL_ARRAY_BUFFER, vbo[6]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glFrontFace(GL_CCW);
        // --- torus texturing ---
    glBindBuffer(GL_ARRAY_BUFFER, vbo[7]);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, brickTexture);
        // -----------------------
        // --- torus lighting ---
    glBindBuffer(GL_ARRAY_BUFFER, vbo[8]);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);
    invTrMat = glm::transpose(glm::inverse(mMat));
    glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
        // ----------------------
        // --- torus shadowing ---
    shadowMVP = b * lightPmatrix * lightVmatrix * mMat;
    glUniformMatrix4fv(shLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP));
        // -------------------------
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[9]);
    glDrawElements(GL_TRIANGLES, myTorus.getNumIndices(), GL_UNSIGNED_INT, 0);

    trfmStack.pop(); // ++ remove torus's transformations
    // ----------------------------------------------------------------------------------

    // ------------------------------- imported shuttle -----------------------------------
    trfmStack.push(trfmStack.top()); // +++ inherit sun's translation
    trfmStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(cos((float)currentTime) * 4.0f, sin((float)currentTime) * 4.0f, cos((float)currentTime) * 4.0f));
    trfmStack.top() *= glm::rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(1.0, 1.0, 0.0));
    trfmStack.top() *= glm::scale(glm::mat4(1.0f), glm::vec3(4.0f, 4.0f, 4.0f));
    mMat = trfmStack.top();
    glUniformMatrix4fv(mLoc, 1, GL_FALSE, glm::value_ptr(mMat));

    glBindBuffer(GL_ARRAY_BUFFER, vbo[10]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glFrontFace(GL_CCW);
        // --- shuttle texturing ---
    glBindBuffer(GL_ARRAY_BUFFER, vbo[11]);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, shuttleTexture);
        // ------------------------
        // --- shuttle lighting ---
    glBindBuffer(GL_ARRAY_BUFFER, vbo[12]);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);
    invTrMat = glm::transpose(glm::inverse(mMat));
    glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
        // ------------------------
        // --- shuttle shadowing ---
    shadowMVP = b * lightPmatrix * lightVmatrix * mMat;
    glUniformMatrix4fv(shLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP));
        // -------------------------
    glDrawArrays(GL_TRIANGLES, 0, myShuttle.getNumVertices());

    trfmStack.pop(); // ++ remove shuttle's transformations
    // ------------------------------------------------------------------------------------

    // ------------------------------- imported dolphin -----------------------------------
    trfmStack.push(trfmStack.top()); // +++ inherit sun's translation
    trfmStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(cos((float)currentTime) * 4.0f, -sin((float)currentTime) * 4.0f, -cos((float)currentTime) * 4.0f));
    trfmStack.top() *= glm::rotate(glm::mat4(1.0f), -(float)currentTime, glm::vec3(1.0, 1.0, 0.0));
    trfmStack.top() *= glm::scale(glm::mat4(1.0f), glm::vec3(4.0f, 4.0f, 4.0f));
    mMat = trfmStack.top();
    glUniformMatrix4fv(mLoc, 1, GL_FALSE, glm::value_ptr(mMat));

    glBindBuffer(GL_ARRAY_BUFFER, vbo[14]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glFrontFace(GL_CCW);
        // --- dolphin texturing ---
    glBindTexture(GL_TEXTURE_2D, 0);
        // ------------------------
        // --- dolphin lighting ---
    glBindBuffer(GL_ARRAY_BUFFER, vbo[16]);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);
    invTrMat = glm::transpose(glm::inverse(mMat));
    glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
        // ------------------------
        // --- dolphin shadowing ---
    shadowMVP = b * lightPmatrix * lightVmatrix * mMat;
    glUniformMatrix4fv(shLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP));
        // -------------------------
    glDrawArrays(GL_TRIANGLES, 0, myDolphin.getNumVertices());

    trfmStack.pop(); // ++ remove dolphin's transformations
    // ------------------------------------------------------------------------------------

    trfmStack.pop(); // + remove sun's translation
    trfmStack.pop(); // remove initial matrix
}

void display(GLFWwindow* window, double currentTime)
{
    glClear(GL_DEPTH_BUFFER_BIT);
    glClear(GL_COLOR_BUFFER_BIT);

    // set up view and perspective matrix from the light point of view, for pass 1
    lightVmatrix = glm::lookAt(currLightPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // vector from light to origin
    lightPmatrix = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);

    // make the custom frame buffer current, and associate it with the shadow texture
    glBindFramebuffer(GL_FRAMEBUFFER, shadowBuffer);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowTex, 0);

    // disable drawing colors
    glDrawBuffer(GL_NONE);

    passOne(window, currentTime);

    // restore the default display buffer, and re-enable drawing
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, shadowTex);
    glDrawBuffer(GL_FRONT); // re-enables drawing colors

    passTwo(window, currentTime);
}

void window_reshape_callback(GLFWwindow* window, int newWidth, int newHeight)
{
    width = newWidth;
    height = newHeight;

    aspect = (float)width / (float)height;
    glViewport(0, 0, width, height); // set screen region associated with framebuffer
    pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f); // update perspective matrix, 1.0472 radians = 60 degrees

    glBindTexture(GL_TEXTURE_2D, shadowTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0); // update shadow size
}

int main(void)
{
    if (!glfwInit())
    {
        exit(EXIT_FAILURE);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGLPlayground", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(EXIT_FAILURE);
    }

    glfwSetWindowSizeCallback(window, window_reshape_callback);

    init(window);

    // --- compute shader output ---
    computeSum();
    std::cout << csInput1[0] << " " << csInput1[1] << " " << csInput1[2] << " " << csInput1[3] << " " << csInput1[4] << " " << csInput1[5] << std::endl;
    std::cout << csInput2[0] << " " << csInput2[1] << " " << csInput2[2] << " " << csInput2[3] << " " << csInput2[4] << " " << csInput2[5] << std::endl;
    std::cout << csOutput[0] << " " << csOutput[1] << " " << csOutput[2] << " " << csOutput[3] << " " << csOutput[4] << " " << csOutput[5] << std::endl;
    // -----------------------------

    while (!glfwWindowShouldClose(window))
    {
        display(window, glfwGetTime());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}