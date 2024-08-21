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
constexpr GLuint NUM_VBOS = 13;

std::string resourcePath;
float cameraX, cameraY, cameraZ;
float cubePosX, cubePosY, cubePosZ, pyrPosX, pyrPosY, pyrPosZ;
GLuint renderingProgram;
GLuint vao[NUM_VAOS];
GLuint vbo[NUM_VBOS];
Sphere mySphere(48);
Torus myTorus(0.5f, 0.2f, 48);
ImportedModel myModel("shuttle.obj");

// allocate variables used in display() function, so that they won’t need to be allocated during rendering
GLuint mvLoc, pLoc;
int width, height;
float aspect;
glm::mat4 pMat, vMat, mMat;
std::stack<glm::mat4> mvStack;
GLuint brickTexture;
GLuint earthTexture;
GLuint shuttleTexture;

void setupVertices()
{
    // 36 vertices, 12 triangles, makes 2x2x2 cube placed at origin
    float cubePositions[108] =
    {
        -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f, -1.0f,
         1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f,
    };

    // pyramid with 18 vertices, comprising 6 triangles (four sides, and two on the bottom)
    float pyramidPositions[54] =
    {
        -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f,    // front face
         1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.0f,   // right face
         1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, // back face
        -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f,  // left face
        -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, // base left front
         1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f // base right back
    };

    float pyrTexCoords[36] =
    {
         0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 1.0f,   0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 1.0f, // top and right faces
         0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 1.0f,   0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 1.0f, // back and left faces
         0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,   1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f  // base triangles
    };

    glGenVertexArrays(NUM_VAOS, vao);
    glBindVertexArray(*vao);

    glGenBuffers(NUM_VBOS, vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubePositions), cubePositions, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidPositions), pyramidPositions, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pyrTexCoords), pyrTexCoords, GL_STATIC_DRAW);

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

    // ------------------------------- imported model -----------------------------------
    std::vector<glm::vec3> modVerts = myModel.getVertices();
    std::vector<glm::vec2> modTexs = myModel.getTextureCoords();
    std::vector<glm::vec3> modNorms = myModel.getNormals();
    std::vector<float> modPosVals; // vertex positions
    std::vector<float> modTexVals; // texture coordinates
    std::vector<float> modNormVals; // normal vectors

    int modNumVertices = myModel.getNumVertices();
    for (int i = 0; i < modNumVertices; i++)
    {
        modPosVals.push_back(modVerts[i].x);
        modPosVals.push_back(modVerts[i].y);
        modPosVals.push_back(modVerts[i].z);
        modTexVals.push_back(modTexs[i].s);
        modTexVals.push_back(modTexs[i].t);
        modNormVals.push_back(modNorms[i].x);
        modNormVals.push_back(modNorms[i].y);
        modNormVals.push_back(modNorms[i].z);
    }

    // put the vertices into buffer #11
    glBindBuffer(GL_ARRAY_BUFFER, vbo[10]);
    glBufferData(GL_ARRAY_BUFFER, modPosVals.size() * 4, &modPosVals[0], GL_STATIC_DRAW);
    // put the texture coordinates into buffer #12
    glBindBuffer(GL_ARRAY_BUFFER, vbo[11]);
    glBufferData(GL_ARRAY_BUFFER, modTexVals.size() * 4, &modTexVals[0], GL_STATIC_DRAW);
    // put the normals into buffer #13
    glBindBuffer(GL_ARRAY_BUFFER, vbo[12]);
    glBufferData(GL_ARRAY_BUFFER, modNormVals.size() * 4, &modNormVals[0], GL_STATIC_DRAW);
    // ----------------------------------------------------------------------------------
}

void init(GLFWwindow* window)
{
    resourcePath = Utils::getResourcePath();
    std::string vertShaderPath = resourcePath + "vertShader.glsl";
    std::string fragShaderPath = resourcePath + "fragShader.glsl";
    renderingProgram = Utils::createShaderProgram(vertShaderPath.c_str(), fragShaderPath.c_str());

    cameraX = 0.0f; cameraY = 0.0f; cameraZ = 8.0f;
    cubePosX = 0.0f; cubePosY = 0.0f; cubePosZ = 0.0f;
    pyrPosX = 0.0f; pyrPosY = 0.0f; pyrPosZ = 0.0f;

    setupVertices();

    // build perspective matrix
    glfwGetFramebufferSize(window, &width, &height);
    aspect = (float)width / (float)height;
    pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f); // 1.0472 radians = 60 degrees

    brickTexture = Utils::loadTexture(resourcePath, "brick1.jpg");
    earthTexture = Utils::loadTexture(resourcePath, "earthmap1k.jpg");
    shuttleTexture = Utils::loadTexture(resourcePath, "spstob_1.jpg");
}

void display(GLFWwindow* window, double currentTime)
{
    glClear(GL_DEPTH_BUFFER_BIT);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(renderingProgram);

    // settings
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LEQUAL); // passes if the incoming depth value is less than or equal to the stored depth value

    // reference uniform variables
    mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
    pLoc = glGetUniformLocation(renderingProgram, "p_matrix");
    
    // copy perspective matrix
    glUniformMatrix4fv(pLoc, 1, GL_FALSE, glm::value_ptr(pMat));
    
    // push view matrix onto the stack
    vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));
    mvStack.push(vMat);

    // ---------------------- pyramid == sun --------------------------------------------
    mvStack.push(mvStack.top());
    mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)); // sun position
    mvStack.push(mvStack.top());
    mvStack.top() *= glm::rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(1.0f, 0.0f, 0.0f)); // sun rotation
    glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
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
    glDrawArrays(GL_TRIANGLES, 0, 18); // draw the sun
    mvStack.pop(); // remove the sun’s axial rotation from the stack
    // ----------------------------------------------------------------------------------

    // ----------------------- cube == planet -------------------------------------------
    mvStack.push(mvStack.top());
    mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(sin((float)currentTime) * 4.0, 0.0f, cos((float)currentTime) * 4.0));
    mvStack.push(mvStack.top());
    mvStack.top() *= glm::rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0, 1.0, 0.0)); // planet rotation
    glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glFrontFace(GL_CW); // the cube vertices have clockwise winding order
    glDrawArrays(GL_TRIANGLES, 0, 36); // draw the planet
    mvStack.pop(); // remove the planet’s axial rotation from the stack

    // ----------------------- smaller cube == moon -------------------------------------
    mvStack.push(mvStack.top());
    mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, sin((float)currentTime) * 2.0, cos((float)currentTime) * 2.0));
    mvStack.top() *= glm::rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0, 0.0, 1.0)); // moon rotation
    mvStack.top() *= glm::scale(glm::mat4(1.0f), glm::vec3(0.25f, 0.25f, 0.25f)); // make the moon smaller
    glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glDrawArrays(GL_TRIANGLES, 0, 36); // draw the moon
    
    // remove moon scale/rotation/position, planet position, sun position, and view matrices from stack
    mvStack.pop();
    mvStack.pop();
    mvStack.pop();
    mvStack.pop();
    // ----------------------------------------------------------------------------------

    // ------------------------------ procedural sphere ---------------------------------
    mvStack.push(vMat);
    mvStack.push(mvStack.top());
    mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 0.0f, 0.0f));
    mvStack.top() *= glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
    glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));

    glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glFrontFace(GL_CCW); // the sphere vertices have clockwise winding order

    glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0); // specify layout of tex coords
    glEnableVertexAttribArray(1); // enable vert shader to access tex coords stored in VBO
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, earthTexture);

    glDrawArrays(GL_TRIANGLES, 0, mySphere.getNumIndices());

    mvStack.pop();
    mvStack.pop();
    // ----------------------------------------------------------------------------------

    // ------------------------------ procedural torus ----------------------------------
    mvStack.push(vMat);
    mvStack.push(mvStack.top());
    mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, 0.0f));
    mvStack.top() *= glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f));
    mvStack.top() *= glm::rotate(glm::mat4(1.0f), Utils::toRadians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));

    glBindBuffer(GL_ARRAY_BUFFER, vbo[6]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glFrontFace(GL_CCW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[7]);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, brickTexture);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[9]);
    glDrawElements(GL_TRIANGLES, myTorus.getNumIndices(), GL_UNSIGNED_INT, 0);

    mvStack.pop();
    mvStack.pop();
    // ----------------------------------------------------------------------------------

    // ------------------------------- imported model -----------------------------------
    mvStack.push(vMat);
    mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(cos((float)currentTime) * 4.0f, sin((float)currentTime) * 4.0f, cos((float)currentTime) * 4.0f));
    mvStack.top() *= glm::rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(1.0, 1.0, 0.0));
    mvStack.top() *= glm::scale(glm::mat4(1.0f), glm::vec3(4.0f, 4.0f, 4.0f));
    glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));

    glBindBuffer(GL_ARRAY_BUFFER, vbo[10]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glFrontFace(GL_CCW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[11]);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, shuttleTexture);
    
    glDrawArrays(GL_TRIANGLES, 0, myModel.getNumVertices());

    mvStack.pop();
    // ----------------------------------------------------------------------------------
}

void window_reshape_callback(GLFWwindow* window, int newWidth, int newHeight)
{
    // update perspective matrix
    aspect = (float)width / (float)height; // new width & height provided by the callback
    glViewport(0, 0, newWidth, newHeight); // set screen region associated with framebuffer
    pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f); // 1.0472 radians = 60 degrees
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
        display(window, glfwGetTime());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}