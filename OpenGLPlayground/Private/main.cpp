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

constexpr GLuint SCR_WIDTH = 1800;
constexpr GLuint SCR_HEIGHT = 1600;
constexpr GLuint NUM_VAOS = 1;
constexpr GLuint NUM_VBOS = 7;
constexpr GLsizei cubeStride = 8 * sizeof(float);

std::string resourcePath;
float cameraX, cameraY, cameraZ;
GLuint vao[NUM_VAOS];
GLuint vbo[NUM_VBOS];
Sphere mySphere(48);

// allocate variables used in display() function, so that they won�t need to be allocated during rendering
int width = SCR_WIDTH, height = SCR_HEIGHT;
float aspect;
glm::mat4 mMat, vMat, pMat, invTrMat;
std::stack<glm::mat4> trfmStack;
GLuint brickTexture, earthTexture;

// shader uniform locations
GLuint mLoc, vLoc, pLoc, nLoc, shLoc;
GLuint globalAmbLoc, winSizeLoc;

// PBR-related
GLuint pbrProgram;
glm::mat3 nMat;
glm::vec3 lightPositions[] =
{
    glm::vec3(-10.0f,  20.0f, 10.0f),
    glm::vec3(10.0f,  20.0f, 10.0f),
    glm::vec3(-10.0f, 20.0f, 10.0f),
    glm::vec3(10.0f, 20.0f, 10.0f),
};
glm::vec3 lightColors[] =
{
    glm::vec3(300.0f, 300.0f, 300.0f),
    glm::vec3(300.0f, 300.0f, 300.0f),
    glm::vec3(300.0f, 300.0f, 300.0f),
    glm::vec3(300.0f, 300.0f, 300.0f)
};

// Deferred-related
GLuint gBufferProgram;
GLuint gBuffer, gPosition, gNormal, gAlbedoSpec;
GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
GLuint rboDepth;

void calcPyramidNormals(const float* verts, float* outNormals)
{
    for (int i = 0; i < 54; i += 9)
    {
        float faceVerts[9];
        std::copy(verts + i, verts + i + 9, faceVerts);
        float norm[3];
        Utils::calculateNormal(faceVerts, norm);
        std::copy(norm, norm + 3, outNormals + i);
        std::copy(norm, norm + 3, outNormals + i + 3);
        std::copy(norm, norm + 3, outNormals + i + 6);
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
    glBindVertexArray(vao[0]);

    glGenBuffers(NUM_VBOS, vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeData), cubeData, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pyrVerts), pyrVerts, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pyrTexCoords), pyrTexCoords, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
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
    glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
    glBufferData(GL_ARRAY_BUFFER, sphPosVals.size() * 4, &sphPosVals[0], GL_STATIC_DRAW);
    // put the texture coordinates into buffer #5
    glBindBuffer(GL_ARRAY_BUFFER, vbo[5]);
    glBufferData(GL_ARRAY_BUFFER, sphTexVals.size() * 4, &sphTexVals[0], GL_STATIC_DRAW);
    // put the normals into buffer #6
    glBindBuffer(GL_ARRAY_BUFFER, vbo[6]);
    glBufferData(GL_ARRAY_BUFFER, sphNormVals.size() * 4, &sphNormVals[0], GL_STATIC_DRAW);
    // ----------------------------------------------------------------------------------
}

void setupGBuffer()
{
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

    glGenTextures(1, &gAlbedoSpec);
    glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);

    glDrawBuffers(3, attachments);

    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    // finally check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "Framebuffer not complete!" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void init(GLFWwindow* window)
{
    resourcePath = Utils::getResourcePath();
    std::string vertPBRShaderPath = resourcePath + "vertPBRShader.glsl";
    std::string fragPBRShaderPath = resourcePath + "fragPBRShader.glsl";
    std::string vertGBufferShaderPath = resourcePath + "vertGBufferShader.glsl";
    std::string fragGBufferShaderPath = resourcePath + "fragGBufferShader.glsl";
    pbrProgram = Utils::createShaderProgram(vertPBRShaderPath.c_str(), fragPBRShaderPath.c_str());
    gBufferProgram = Utils::createShaderProgram(vertGBufferShaderPath.c_str(), fragGBufferShaderPath.c_str());

    cameraX = 0.0f; cameraY = 0.0f; cameraZ = 8.0f;

    setupVertices();
    setupGBuffer();

    // build perspective matrix
    glfwGetFramebufferSize(window, &width, &height);
    aspect = (float)width / (float)height;
    pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f); // 1.0472 radians = 60 degrees

    brickTexture = Utils::loadTexture(resourcePath, "brick1.jpg");
    earthTexture = Utils::loadTexture(resourcePath, "earthmap1k.jpg");
}

// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    // todo
    glDisable(GL_CULL_FACE); // needed
    glDisable(GL_DEPTH_TEST);
    // ----
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

// todo
    glBindVertexArray(vao[0]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
// ----
}

void geometryPass(GLFWwindow* window, double currentTime)
{
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

// todo
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    //glBindFramebuffer(GL_DRAW_FRAMEBUFFER, gBuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//

    glUseProgram(gBufferProgram);

    // reference uniform variables
    mLoc = glGetUniformLocation(gBufferProgram, "m_matrix");
    vLoc = glGetUniformLocation(gBufferProgram, "v_matrix");
    pLoc = glGetUniformLocation(gBufferProgram, "p_matrix");
    nLoc = glGetUniformLocation(gBufferProgram, "n_matrix");

    // copy perspective matrix
    glUniformMatrix4fv(pLoc, 1, GL_FALSE, glm::value_ptr(pMat));

    // build and copy view matrix
    vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));
    glUniformMatrix4fv(vLoc, 1, GL_FALSE, glm::value_ptr(vMat));

    trfmStack.push(glm::mat4(1.0f)); // + initial matrix
    trfmStack.push(trfmStack.top()); // ++ copy default matrix
    trfmStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)); // root position

    // ---------------------- pyramid == sun --------------------------------------------
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
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, brickTexture);
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, brickTexture);
    // -------------------------
    // --- pyramid normals ---
    glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);
    invTrMat = glm::transpose(glm::inverse(mMat));
    nMat = glm::mat3(invTrMat[0], invTrMat[1], invTrMat[2]);
    glUniformMatrix3fv(nLoc, 1, GL_FALSE, glm::value_ptr(nMat));
    // ------------------------
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
    // --- cube texture coordinates ---
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, cubeStride, (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // --------------------------------
    // --- cube texture samplers ---
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, brickTexture);
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, brickTexture);
    // -----------------------------
    // --- cube normals ---
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, cubeStride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    // --------------------
    // --- cube model inverse transpose ---
    invTrMat = glm::transpose(glm::inverse(mMat));
    nMat = glm::mat3(invTrMat[0], invTrMat[1], invTrMat[2]);
    glUniformMatrix3fv(nLoc, 1, GL_FALSE, glm::value_ptr(nMat));
    // ------------------------------------
    glDrawArrays(GL_TRIANGLES, 0, 36); // draw the planet
    trfmStack.pop(); // +++ remove planet's rotation axis and scaling
    trfmStack.pop(); // ++ remove planet's translation
    // ----------------------------------------------------------------------------------

    // ------------------------------ procedural sphere ---------------------------------
    trfmStack.push(trfmStack.top()); // +++ inherit sun's translation
    trfmStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 0.0f, 0.0f));
    trfmStack.top() *= glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
    trfmStack.top() *= glm::rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0f, 1.0f, 0.0f));
    mMat = trfmStack.top();
    glUniformMatrix4fv(mLoc, 1, GL_FALSE, glm::value_ptr(mMat));

    glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glFrontFace(GL_CCW); // the sphere vertices have clockwise winding order
    // --- sphere texturing ---
    glBindBuffer(GL_ARRAY_BUFFER, vbo[5]);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0); // specify layout of tex coords
    glEnableVertexAttribArray(1); // enable vert shader to access tex coords stored in VBO
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, earthTexture);
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, earthTexture);
    // ------------------------
    // --- sphere normals ---
    glBindBuffer(GL_ARRAY_BUFFER, vbo[6]);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);
    invTrMat = glm::transpose(glm::inverse(mMat));
    nMat = glm::mat3(invTrMat[0], invTrMat[1], invTrMat[2]);
    glUniformMatrix3fv(nLoc, 1, GL_FALSE, glm::value_ptr(nMat));
    // -----------------------
    glDrawArrays(GL_TRIANGLES, 0, mySphere.getNumIndices());

    trfmStack.pop(); // ++ remove sphere's transformations
    // ----------------------------------------------------------------------------------

    trfmStack.pop(); // + remove root position
    trfmStack.pop(); // remove initial matrix

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void lightingPass(GLFWwindow* window, double currentTime)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(pbrProgram);

// todo
    //GLuint gPositionLoc = glGetUniformLocation(pbrProgram, "gPosition");
    //glUniform1i(gPositionLoc, 0);
    //GLuint gNormalLoc = glGetUniformLocation(pbrProgram, "gNormal");
    //glUniform1i(gNormalLoc, 1);
    //GLuint gAlbedoSpecLoc = glGetUniformLocation(pbrProgram, "gAlbedoSpec");
    //glUniform1i(gAlbedoSpecLoc, 2);
//

    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glActiveTexture(GL_TEXTURE0 + 2);
    glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);

    // temporarily setting each frame
    glUniform3f(glGetUniformLocation(pbrProgram, "albedoScale"), 1.0f, 1.0f, 1.0f);
    glUniform1f(glGetUniformLocation(pbrProgram, "aoScale"), 1.0f);
    glUniform1f(glGetUniformLocation(pbrProgram, "metallicScale"), 1.0f);
    glUniform1f(glGetUniformLocation(pbrProgram, "roughnessScale"), 1.0f);

    // provide camera position
    glUniform3fv(
        glGetUniformLocation(pbrProgram, "camPos"),
        1, &glm::vec3(-cameraX, -cameraY, -cameraZ)[0]);

    // render light source (simply re-render sphere at light positions)
    // this looks a bit off as we use the same shader, but it'll make their positions obvious and 
    // keeps the codeprint small.
    for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
    {
        glm::vec3 newPos = lightPositions[i];
        glUniform3fv(
            glGetUniformLocation(pbrProgram, ("lightPositions[" + std::to_string(i) + "]").c_str()),
            1, &newPos[0]);
        glUniform3fv(
            glGetUniformLocation(pbrProgram, ("lightColors[" + std::to_string(i) + "]").c_str()),
            1, &lightColors[i][0]);
    }

    renderQuad();

    // 2.5. copy content of geometry's depth buffer to default framebuffer's depth buffer
// ----------------------------------------------------------------------------------
    glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void window_reshape_callback(GLFWwindow* window, int newWidth, int newHeight)
{
    width = newWidth;
    height = newHeight;

    aspect = (float)width / (float)height;
    glViewport(0, 0, width, height); // set screen region associated with framebuffer
    pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f); // update perspective matrix, 1.0472 radians = 60 degrees

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

    while (!glfwWindowShouldClose(window))
    {
        geometryPass(window, glfwGetTime());
        lightingPass(window, glfwGetTime());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}