#include "ShaderProgram.h"

ShaderProgram::ShaderProgram(const char* vertFileName, const char* fragFileName)
{
    std::string vertStr = GetFileContents(vertFileName);
    std::string fragStr = GetFileContents(fragFileName);
    const char* vertCode = vertStr.c_str();
    const char* fragCode = fragStr.c_str();

    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertShader, 1, &vertCode, NULL);
    glCompileShader(vertShader);
    CheckErrors(vertShader, "VERTEX");

    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &fragCode, NULL);
    glCompileShader(fragShader);
    CheckErrors(fragShader, "FRAGMENT");

    ID = glCreateProgram();
    glAttachShader(ID, vertShader);
    glAttachShader(ID, fragShader);
    glLinkProgram(ID);
    CheckErrors(ID, "PROGRAM");

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);
}

void ShaderProgram::Activate()
{
    glUseProgram(ID);
}

void ShaderProgram::Delete()
{
    glDeleteProgram(ID);
}

void ShaderProgram::CheckErrors(unsigned int shader, const char* type)
{
    GLint hasCompiled;
    char infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
        if (hasCompiled == GL_FALSE)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "SHADER_COMPILATION_ERROR for: " << type << "\n" << std::endl;
        }
    }
    else
    {
        glGetProgramiv(shader, GL_COMPILE_STATUS, &hasCompiled);
        if (hasCompiled == GL_FALSE)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "SHADER_LINKING_ERROR for: " << type << "\n" << std::endl;
        }
    }
}

std::string GetFileContents(const char* filename)
{
    std::ifstream in(filename, std::ios::binary);
    if (in)
    {
        std::string contents;
        in.seekg(0, std::ios::end);
        contents.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&contents[0], contents.size());
        in.close();
        return(contents);
    }
    throw(errno);
}
