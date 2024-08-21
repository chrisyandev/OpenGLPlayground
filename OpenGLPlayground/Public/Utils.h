#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <fstream>

class Utils
{
public:
    static std::string getCurrentPath();
    static std::string getResourcePath();
    static std::string readShaderSource(const char* filePath);
    static GLuint createShaderProgram(const char* vp, const char* fp);
    static GLuint loadTexture(const std::string& directoryPath, const std::string& texImageName);
    static float toRadians(float degrees);

private:
    static void checkCompileErrors(GLuint shader, const std::string& type);
};
