#pragma once
#include <glad/glad.h>
#include <vector>

class VBO
{
public:
    VBO(std::vector<GLfloat>& vertices);
    void Bind();
    void Unbind();
    void Delete();
    GLuint ID;
};
