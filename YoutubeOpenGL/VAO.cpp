#include "VAO.h"

VAO::VAO()
    : ID{ 0 }
{
    glGenVertexArrays(1, &ID);
}

void VAO::LinkVBO(VBO& VBO, GLuint attribIndex, GLuint attribNumComponents, GLenum attribType)
{
    VBO.Bind();
    glEnableVertexAttribArray(attribIndex);
    glVertexAttribPointer(attribIndex, attribNumComponents, attribType, GL_FALSE, 0, (void*)0);
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
