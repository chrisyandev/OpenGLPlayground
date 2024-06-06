#pragma once
#include <glad/glad.h>
#include "VBO.h"

class VAO
{
public:
    VAO();
    void LinkVBO(VBO& VBO, GLuint attribIndex, GLuint attribNumComponents, GLenum attribType, GLboolean isNormalized, GLsizei stride, const GLvoid* startOffset);
    void Bind();
    void Unbind();
    void Delete();
    GLuint ID;
};