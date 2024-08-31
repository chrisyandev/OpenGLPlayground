#include "Utils.h"
#include <SOIL2/SOIL2.h>
#include <filesystem>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

std::string Utils::getCurrentPath()
{
    return std::filesystem::current_path().string() + "\\";
}

std::string Utils::getResourcePath()
{
    return getCurrentPath() + "Resources\\";
}

std::string Utils::readShaderSource(const char* filePath)
{
    std::string content;
    std::ifstream fileStream(filePath, std::ios::in);
    std::string line = "";
    while (!fileStream.eof())
    {
        getline(fileStream, line);
        content.append(line + "\n");
    }
    fileStream.close();
    return content;
}

GLuint Utils::createShaderProgram(const char* vp, const char* fp)
{
    GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);

    std::string vertShaderStr = readShaderSource(vp);
    std::string fragShaderStr = readShaderSource(fp);
    const char* vertShaderSrc = vertShaderStr.c_str();
    const char* fragShaderSrc = fragShaderStr.c_str();

    glShaderSource(vShader, 1, &vertShaderSrc, NULL);
    glCompileShader(vShader);
    checkCompileErrors(vShader, "VERTEX");

    glShaderSource(fShader, 1, &fragShaderSrc, NULL);
    glCompileShader(fShader);
    checkCompileErrors(fShader, "FRAGMENT");

    GLuint vfProgram = glCreateProgram();
    glAttachShader(vfProgram, vShader);
    glAttachShader(vfProgram, fShader);
    glLinkProgram(vfProgram);
    checkCompileErrors(vfProgram, "PROGRAM");

    return vfProgram;
}

void Utils::checkCompileErrors(GLuint shader, const std::string& type)
{
    GLint compiled;
    GLchar infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (compiled != GL_TRUE)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &compiled);
        if (compiled != GL_TRUE)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}

GLuint Utils::loadTexture(const std::string& directoryPath, const std::string& texImageName)
{
    GLuint textureID;
    textureID = SOIL_load_OGL_texture((directoryPath + texImageName).c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
    if (textureID == 0)
    {
        std::cout << "could not find texture file " << texImageName << " in directory " << directoryPath << std::endl;
    }

    // if mipmapping
    glBindTexture(GL_TEXTURE_2D, textureID);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    // if also anisotropic filtering
    GLfloat anisoSetting = 0.0f;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &anisoSetting);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisoSetting);

    return textureID;
}

float Utils::toRadians(float degrees)
{
    return (degrees * 2.0f * 3.14159f) / 360.0f;
}

void Utils::calculateNormal(const float* verts, float* outNormal)
{
    glm::vec3 A(verts[0], verts[1], verts[2]);
    glm::vec3 B(verts[3], verts[4], verts[5]);
    glm::vec3 C(verts[6], verts[7], verts[8]);
    glm::vec3 AB = glm::normalize(B - A);
    glm::vec3 AC = glm::normalize(C - A);
    glm::vec3 normal = glm::cross(AB, AC);
    outNormal[0] = normal.x;
	outNormal[1] = normal.y;
	outNormal[2] = normal.z;
}
