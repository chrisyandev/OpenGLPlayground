#include "VAO.h"

VAO::VAO()
{
	glGenVertexArrays(1, &ID); // generate 1 Vertex Array Object name and store in ID
}

void VAO::LinkVBO(VBO VBO, GLuint layout)
{
	VBO.Bind();
	glVertexAttribPointer(layout, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); // define how OpenGL should read the VBO
	glEnableVertexAttribArray(layout);
	VBO.Unbind();
}

void VAO::Bind()
{
	glBindVertexArray(ID); // set as current Vertex Array Object
}

void VAO::Unbind()
{
	glBindVertexArray(0);
}

void VAO::Delete()
{
	glDeleteVertexArrays(1, &ID);
}