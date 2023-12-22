#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>

#include "Texture.h"
#include "shaderClass.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"

GLfloat vertices[] =
{	// COORDINATES			// COLORS			// TEXCOORDS
	-0.5f, -0.5f, 0.0f,     1.0f, 0.0f, 0.0f,	0.0f, 0.0f, // bottom left
	-0.5f,  0.5f, 0.0f,     0.0f, 1.0f, 0.0f,	0.0f, 1.0f, // top left
	 0.5f,  0.5f, 0.0f,     0.0f, 0.0f, 1.0f,	1.0f, 1.0f, // top right
	 0.5f, -0.5f, 0.0f,     1.0f, 1.0f, 1.0f,	1.0f, 0.0f, // bottom right
};

GLuint indices[] =	// describes vertices order
{
	0, 2, 1,
	0, 3, 2,
};

int main()
{
	glfwInit(); // initialize GLFW

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // tell GLFW what version of OpenGL we're using
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // OpenGL 3.3
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // using the CORE profile so that we only have modern functions

	GLFWwindow* window = glfwCreateWindow(800, 800, "YoutubeOpenGL", NULL, NULL); // create a window of 800 by 800 pixels with a title
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window); // introduce the window into the current context

	gladLoadGL(); // load GLAD so it configures OpenGL

	glViewport(0, 0, 800, 800); // specify the viewport with bottom left and top right coordinates

	Shader shaderProgram("default.vert", "default.frag"); // generate shader object

	VAO VAO1; // generate VAO
	VAO1.Bind(); // set as current VAO

	VBO VBO1(vertices, sizeof(vertices)); // generate VBO and link it to vertices
	EBO EBO1(indices, sizeof(indices)); // generate EBO and link it to indices

	VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0); // link VBO position attribute to VAO
	VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float))); // link VBO color attribute to VAO
	VAO1.LinkAttrib(VBO1, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float))); // link VBO texture coordinates attribute to VAO
	VAO1.Unbind(); // unbind all to prevent accidental modification // make sure unbinding VAO comes first
	VBO1.Unbind();
	EBO1.Unbind();

	GLuint uniID = glGetUniformLocation(shaderProgram.ID, "scale"); // gets ID of uniform called "scale"

	Texture popCat("pop_cat.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE); // create texture
	popCat.texUnit(shaderProgram, "tex0", 0);

	while (!glfwWindowShouldClose(window)) // main loop
	{
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f); // background color
		glClear(GL_COLOR_BUFFER_BIT); // clean the back buffer and assign new color to it
		shaderProgram.Activate(); // set the Shader Program we want to use
		glUniform1f(uniID, 0.5f); // set the value for "scale"
		popCat.Bind(); // so it gets rendered
		VAO1.Bind(); // set as current VAO
		glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0); // draw triangles, passing in: number of indices, datatype of indices, index of indices
		glfwSwapBuffers(window); // makes sure image is updated each frame
		glfwPollEvents(); // takes care of all GLFW events
	}

	VAO1.Delete(); // delete all objects we created
	VBO1.Delete();
	EBO1.Delete();
	popCat.Delete();
	shaderProgram.Delete();

	glfwDestroyWindow(window); // delete window
	glfwTerminate(); // terminate GLFW
	return 0; // program ends
}