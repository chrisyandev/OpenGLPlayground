#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <cmath>
#include <stack>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Utils.h"
#include "Sphere.h"

constexpr GLuint SCR_WIDTH = 800;
constexpr GLuint SCR_HEIGHT = 600;
constexpr GLuint NUM_VAOS = 1;
constexpr GLuint NUM_VBOS = 6;

std::string resourcePath;
float cameraX, cameraY, cameraZ;
float cubePosX, cubePosY, cubePosZ, pyrPosX, pyrPosY, pyrPosZ;
GLuint renderingProgram;
GLuint vao[NUM_VAOS];
GLuint vbo[NUM_VBOS];
Sphere mySphere(48);

// allocate variables used in display() function, so that they won’t need to be allocated during rendering
GLuint mvLoc, pLoc;
int width, height;
float aspect;
glm::mat4 pMat, vMat, mMat;
std::stack<glm::mat4> mvStack;
GLuint brickTexture;

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

    // ------------------------------ procedural sphere ------------------------------
    std::vector<int> ind = mySphere.getIndices();
    std::vector<glm::vec3> vert = mySphere.getVertices();
    std::vector<glm::vec2> tex = mySphere.getTexCoords();
    std::vector<glm::vec3> norm = mySphere.getNormals();
    std::vector<float> pvalues; // vertex positions
    std::vector<float> tvalues; // texture coordinates
    std::vector<float> nvalues; // normal vectors

    int numIndices = mySphere.getNumIndices();
    for (int i = 0; i < numIndices; i++) {
        pvalues.push_back((vert[ind[i]]).x);
        pvalues.push_back((vert[ind[i]]).y);
        pvalues.push_back((vert[ind[i]]).z);
        tvalues.push_back((tex[ind[i]]).s);
        tvalues.push_back((tex[ind[i]]).t);
        nvalues.push_back((norm[ind[i]]).x);
        nvalues.push_back((norm[ind[i]]).y);
        nvalues.push_back((norm[ind[i]]).z);
    }

    // put the vertices into buffer #4
    glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
    glBufferData(GL_ARRAY_BUFFER, pvalues.size() * 4, &pvalues[0], GL_STATIC_DRAW);
    // put the texture coordinates into buffer #5
    glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
    glBufferData(GL_ARRAY_BUFFER, tvalues.size() * 4, &tvalues[0], GL_STATIC_DRAW);
    // put the normals into buffer #6
    glBindBuffer(GL_ARRAY_BUFFER, vbo[5]);
    glBufferData(GL_ARRAY_BUFFER, nvalues.size() * 4, &nvalues[0], GL_STATIC_DRAW);
}

void init(GLFWwindow* window)
{
    resourcePath = std::filesystem::current_path().string() + "\\Resources\\";
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
    mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
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
    glBindTexture(GL_TEXTURE0, brickTexture);

    glDrawArrays(GL_TRIANGLES, 0, mySphere.getNumIndices());

    mvStack.pop();
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