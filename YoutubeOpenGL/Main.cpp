#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include <vector>
#include <array>

#include "ShaderProgram.h"
#include "VAO.h"
#include "VBO.h"

struct vec3
{
    float x;
    float y;
    float z;
};

struct vec4
{
    float w;
    float x;
    float y;
    float z;
};

vec3 Normalize(const vec3& v)
{
    float magnitude = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    if (magnitude <= 0.f)
    {
        return v;
    }
    return vec3{ v.x / magnitude, v.y / magnitude, v.z / magnitude };
}

vec4 CreateQuaternion(float theta, const vec3& axis)
{
    vec3 axisNorm = Normalize(axis);
    float w = std::cos(theta / 2);
    float x = std::sin(theta / 2) * axisNorm.x;
    float y = std::sin(theta / 2) * axisNorm.y;
    float z = std::sin(theta / 2) * axisNorm.z;
    return vec4{ w, x, y, z };
}

vec4 MultiplyQuaternions(const vec4& q1, const vec4& q2)
{
    float w = q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z;
    float x = q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y;
    float y = q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x;
    float z = q1.w * q2.z + q1.x * q2.y - q1.y * q2.y + q1.z * q2.w;
    return vec4{ w, x, y, z };
}

std::array<float, 16> QuaternionAsMatrix(const vec4& quat)
{
    /* Column-major order */
    return std::array<float, 16>
    {
         quat.w,  quat.x,  quat.y,  quat.z,
        -quat.x,  quat.w,  quat.z, -quat.y,
        -quat.y, -quat.z,  quat.w,  quat.x,
        -quat.z,  quat.y, -quat.x,  quat.w
    };

    /* Row-major order */
    //return std::array<float, 16>
    //{
    //    quat.w, -quat.x, -quat.y, -quat.z,
    //    quat.x,  quat.w, -quat.z,  quat.y,
    //    quat.y,  quat.z,  quat.w, -quat.x,
    //    quat.z, -quat.y,  quat.x,  quat.w
    //};
}

int main()
{
    glfwInit(); // initialize GLFW

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // tell GLFW what version of OpenGL we're using
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // OpenGL 3.3
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // using the CORE profile so that we only have modern functions

    GLFWwindow* window = glfwCreateWindow(800, 800, "YoutubeOpenGL", NULL, NULL); // create a window of 800 by 800 pixels with a title
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
        0.f, 1.f, 0.f, // top vertex
        -0.5f, 0.f, 0.5f, // front left vertex
        0.5f, 0.f, 0.5, // front right vertex
        -0.5f, 0.f, -0.5f, // back left vertex
        0.5f, 0.f, -0.5, // back right vertex
    };

    ShaderProgram shaderProgram{ "default.vert", "default.frag" };

    VAO vertexArrayObject{};
    vertexArrayObject.Bind();
    VBO vertexBufferObject{ vertices };
    vertexArrayObject.LinkVBO(vertexBufferObject, 0, 3, GL_FLOAT);

    vertexBufferObject.Unbind();
    vertexArrayObject.Unbind();

    float theta = 0.f;
    double prevTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) // main loop
    {
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shaderProgram.Activate();

        double currTime = glfwGetTime();
        if (currTime - prevTime >= (1.f / 60.f))
        {
            theta += 0.1f;
            prevTime = currTime;

            GLint radiansLoc = glGetUniformLocation(shaderProgram.ID, "radians");
            glUniform1f(radiansLoc, theta);

            /* Begin attempt quaternion rotation */
            //const vec4 q = CreateQuaternion(theta, vec3{ 0.f, 0.f, 0.f });
            //GLint rotationMatrixLoc = glGetUniformLocation(shaderProgram.ID, "rotationMatrix");
            //float* quatAsMatPtr = QuaternionAsMatrix(q).data();
            //glUniformMatrix4fv(rotationMatrixLoc, 1, GL_FALSE, quatAsMatPtr);
            /* End attempt quaternion rotation */
        }
        

        vertexArrayObject.Bind();
        glDrawArrays(GL_TRIANGLES, 0, 3); // draw the triangle using GL_TRIANGLES primitive
        
        glfwSwapBuffers(window); // makes sure image is updated each frame
        glfwPollEvents(); // takes care of all GLFW events
    }

    vertexArrayObject.Delete();
    vertexBufferObject.Delete();
    shaderProgram.Delete();

    glfwDestroyWindow(window); // delete window
    glfwTerminate(); // terminate GLFW
    return 0; // program ends
}