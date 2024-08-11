#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Utils.h"

constexpr GLuint SCR_WIDTH = 800;
constexpr GLuint SCR_HEIGHT = 600;
constexpr GLuint NUM_VAOS = 1;
constexpr GLuint NUM_VBOS = 2;

float cameraX, cameraY, cameraZ;
float cubePosX, cubePosY, cubePosZ;
GLuint renderingProgram;
GLuint vao[NUM_VAOS];
GLuint vbo[NUM_VBOS];

// allocate variables used in display() function, so that they won�t need to be allocated during rendering
GLuint mvLoc, pLoc;
int width, height, cubeCounter;
float aspect, timeFactor;
glm::mat4 pMat, vMat, mMat, mvMat, tMat, rMat;

void setupVertices() // 36 vertices, 12 triangles, makes 2x2x2 cube placed at origin
{
    float vertexPositions[108] =
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

    glGenVertexArrays(NUM_VAOS, vao);
    glBindVertexArray(*vao);

    glGenBuffers(NUM_VBOS, vbo);
    glBindBuffer(GL_ARRAY_BUFFER, *vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);
}

void init(GLFWwindow* window)
{
    std::filesystem::path currentPath = std::filesystem::current_path();
    std::string vertShaderPath = currentPath.string() + "\\Resources\\vertShader.glsl";
    std::string fragShaderPath = currentPath.string() + "\\Resources\\fragShader.glsl";
    renderingProgram = Utils::createShaderProgram(vertShaderPath.c_str(), fragShaderPath.c_str());

    cameraX = 0.0f; cameraY = 0.0f; cameraZ = 32.0f;
    cubePosX = 0.0f; cubePosY = -2.0f; cubePosZ = 0.0f; // shift down Y to reveal perspective

    setupVertices();
}

void display(GLFWwindow* window, double currentTime)
{
    glClear(GL_DEPTH_BUFFER_BIT);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(renderingProgram);

    // get the uniform variables for the MV and projection matrices
    mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
    pLoc = glGetUniformLocation(renderingProgram, "p_matrix");

    // build perspective matrix
    glfwGetFramebufferSize(window, &width, &height);
    aspect = (float)width / (float)height;
    pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f); // 1.0472 radians = 60 degrees

    // view matrix
    vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));

    for (cubeCounter = 0; cubeCounter < 24; ++cubeCounter)
    {
        timeFactor = currentTime + cubeCounter;

        // use current time to compute different translations in x, y, and z
        tMat = glm::translate(glm::mat4(1.0f), glm::vec3(sin(0.35f * timeFactor) * 8.0f, cos(0.52f * timeFactor) * 8.0f, sin(0.7f * timeFactor) * 8.0f)); // y(t)=A*sin(stretch*t)
        rMat = glm::rotate(glm::mat4(1.0f), 1.75f * timeFactor, glm::vec3(0.0f, 1.0f, 0.0f));
        rMat = glm::rotate(rMat, 1.75f * timeFactor, glm::vec3(1.0f, 0.0f, 0.0f)); // the 1.75 adjusts the rotation speed
        rMat = glm::rotate(rMat, 1.75f * timeFactor, glm::vec3(0.0f, 0.0f, 1.0f));

        // model and model-view matrix
        mMat = tMat * rMat;
        mvMat = vMat * mMat;

        // copy perspective and MV matrices to corresponding uniform variables
        glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
        glUniformMatrix4fv(pLoc, 1, GL_FALSE, glm::value_ptr(pMat));

        // associate VBO with the corresponding vertex attribute in the vertex shader
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);

        // adjust OpenGL settings and draw model
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
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