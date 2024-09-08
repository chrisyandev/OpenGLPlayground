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
    static GLuint createShaderProgram(const char* cp);
    static GLuint createShaderProgram(const char* vp, const char* fp);
    static GLuint loadTexture(const std::string& directoryPath, const std::string& texImageName);
    static float toRadians(float degrees);
    static void calculateNormal(const float* verts, float* outNormal);

    // GOLD material - ambient, diffuse, specular, and shininess
    static float* goldAmbient() { static float a[4] = { 0.2473f, 0.1995f, 0.0745f, 1 }; return (float*)a; }
    static float* goldDiffuse() { static float a[4] = { 0.7516f, 0.6065f, 0.2265f, 1 }; return (float*)a; }
    static float* goldSpecular() { static float a[4] = { 0.6283f, 0.5558f, 0.3661f, 1 }; return (float*)a; }
    static float goldShininess() { return 51.2f; }
    // SILVER material - ambient, diffuse, specular, and shininess
    static float* silverAmbient() { static float a[4] = { 0.1923f, 0.1923f, 0.1923f, 1 }; return (float*)a; }
    static float* silverDiffuse() { static float a[4] = { 0.5075f, 0.5075f, 0.5075f, 1 }; return (float*)a; }
    static float* silverSpecular() { static float a[4] = { 0.5083f, 0.5083f, 0.5083f, 1 }; return (float*)a; }
    static float silverShininess() { return 51.2f; }
    // BRONZE material - ambient, diffuse, specular, and shininess
    static float* bronzeAmbient() { static float a[4] = { 0.2125f, 0.1275f, 0.0540f, 1 }; return (float*)a; }
    static float* bronzeDiffuse() { static float a[4] = { 0.7140f, 0.4284f, 0.1814f, 1 }; return (float*)a; }
    static float* bronzeSpecular() { static float a[4] = { 0.3935f, 0.2719f, 0.1667f, 1 }; return (float*)a; }
    static float bronzeShininess() { return 25.6f; }

private:
    static void checkCompileErrors(GLuint shader, const std::string& type);
};
