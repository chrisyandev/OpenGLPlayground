#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <fstream>

class Utils
{
public:
    static std::string readShaderSource(const char* filePath);
    static GLuint createShaderProgram(const char* vp, const char* fp);
    static GLuint loadTexture(std::string directoryPath, std::string texImageName);

private:
    static void checkCompileErrors(GLuint shader, std::string type);
};
