#ifndef VAO_CLASS_H
#define VAO_CLASS_H

#include <glad/glad.h>
#include "VBO.h"

class VAO
{
public:
	/* Reference ID of the Vertex Array Object. */
	GLuint ID;
	/* Generates a VAO. */
	VAO();

	/* Links a VBO Attribute such as a position or color to the VAO. */
	void LinkVBO(VBO VBO, GLuint layout);
	/* Binds the VAO. */
	void Bind();
	/* Unbinds the VAO. */
	void Unbind();
	/* Deletes the VAO. */
	void Delete();
};

#endif
