#include "VAO.h"

VAO::VAO()
    : ID{ 0 }
{
    glGenVertexArrays(1, &ID);
}

void VAO::LinkVBO(VBO& VBO, GLuint attribIndex, GLuint attribNumComponents, GLenum attribType, GLboolean isNormalized, GLsizei stride, const GLvoid* startOffset)
{
    VBO.Bind();
    glEnableVertexAttribArray(attribIndex);
    glVertexAttribPointer(attribIndex, attribNumComponents, attribType, isNormalized, stride, startOffset);
    VBO.Unbind();
}

void VAO::Bind()
{
    glBindVertexArray(ID);
}

void VAO::Unbind()
{
    glBindVertexArray(0);
}

void VAO::Delete()
{
    glDeleteVertexArrays(1, &ID);
}
