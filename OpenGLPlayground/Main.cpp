#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include <vector>
#include <array>

#include "ShaderProgram.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"

int main()
{
    glfwInit(); // initialize GLFW

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // tell GLFW what version of OpenGL we're using
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // OpenGL 3.3
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // using the CORE profile so that we only have modern functions

    GLFWwindow* window = glfwCreateWindow(800, 800, "OpenGLPlayground", NULL, NULL); // create a window of 800 by 800 pixels with a title
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window); // introduce the window into the current context

    gladLoadGL(); // load GLAD so it configures OpenGL

    glViewport(0, 0, 800, 800); // specify the viewport with bottom left and top right coordinates

    // define triangle vertices for a triangle where the base sits on the x axis
    std::vector<GLfloat> vertices =
    {
        // coords ---------- colors
        0.f, 1.f, 0.f,      1.f, 0.f, 0.f,  // top vertex
        -0.5f, 0.f, 0.5f,   0.f, 1.f, 0.f,  // front left vertex
        0.5f, 0.f, 0.5,     0.f, 0.f, 1.f,  // front right vertex
        -0.5f, 0.f, -0.5f,  1.f, 1.f, 0.f,  // back left vertex
        0.5f, 0.f, -0.5,    0.f, 1.f, 1.f,  // back right vertex
    };

    // define order of vertices used to draw each triangle
    std::vector<GLuint> indices =
    {
        0, 1, 2, // front of pyramid
        0, 3, 1, // left of pyramid
        0, 4, 2, // right of pyramid
        0, 4, 3  // back of pyramid
    };

    ShaderProgram shaderProgram{ "default.vert", "default.frag" };

    VAO vertexArrayObject{};
    vertexArrayObject.Bind();
    VBO vertexBufferObject{ vertices };
    EBO elementBufferObject{ indices };
    vertexArrayObject.LinkVBO(vertexBufferObject, 0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    vertexArrayObject.LinkVBO(vertexBufferObject, 1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

    vertexBufferObject.Unbind();
    vertexArrayObject.Unbind();
    elementBufferObject.Unbind();

    float theta = 0.f;
    double prevTime = glfwGetTime();

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window)) // main loop
    {
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaderProgram.Activate();

        double currTime = glfwGetTime();
        double deltaTime = currTime - prevTime;
        if (deltaTime >= (1.f / 60.f))
        {
            theta += 0.5f * deltaTime;
            prevTime = currTime;

            GLint radiansLoc = glGetUniformLocation(shaderProgram.ID, "radians");
            glUniform1f(radiansLoc, theta);
        }
        
        vertexArrayObject.Bind();
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0); // draw the elements using GL_TRIANGLES primitive
        
        glfwSwapBuffers(window); // makes sure image is updated each frame
        glfwPollEvents(); // takes care of all GLFW events
    }

    vertexArrayObject.Delete();
    vertexBufferObject.Delete();
    elementBufferObject.Delete();
    shaderProgram.Delete();

    glfwDestroyWindow(window); // delete window
    glfwTerminate(); // terminate GLFW
    return 0; // program ends
}