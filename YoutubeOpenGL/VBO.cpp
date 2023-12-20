#include "VBO.h"

VBO::VBO(GLfloat* vertices, GLsizeiptr size)
{
	glGenBuffers(1, &ID); // generate 1 Vertex Buffer Object name and store in ID
	glBindBuffer(GL_ARRAY_BUFFER, ID); // set as current Vertex Buffer Object
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW); // store vertices inside buffer
}

void VBO::Bind()
{
	glBindBuffer(GL_ARRAY_BUFFER, ID);
}

void VBO::Unbind()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VBO::Delete()
{
	glDeleteBuffers(1, &ID);
}