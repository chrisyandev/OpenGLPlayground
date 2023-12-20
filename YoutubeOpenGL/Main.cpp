#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include"shaderClass.h"
#include"VAO.h"
#include"VBO.h"
#include"EBO.h"

GLfloat vertices[] = // coordinates of vertices
{
	-0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f, // outer bottom left
	0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f, // outer bottom right
	0.0f, 0.5f * float(sqrt(3)) * 2 / 3, 0.0f, // outer top middle
	-0.5f / 2, 0.5f * float(sqrt(3)) / 6, 0.0f, // inner top left
	0.5f / 2, 0.5f * float(sqrt(3)) / 6, 0.0f, // inner top right
	0.0f, -0.5f * float(sqrt(3)) / 3, 0.0f // inner bottom middle
};

GLuint indices[] =
{
	0, 3, 5, // lower left triangle
	3, 2, 4, // lower right triangle
	5, 4, 1 // upper middle triangle
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

	VAO1.LinkVBO(VBO1, 0); // link VBO to VAO
	VAO1.Unbind(); // unbind all to prevent accidental modification // make sure unbinding VAO comes first
	VBO1.Unbind();
	EBO1.Unbind();

	while (!glfwWindowShouldClose(window)) // main loop
	{
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f); // background color
		glClear(GL_COLOR_BUFFER_BIT); // clean the back buffer and assign new color to it
		shaderProgram.Activate(); // set the Shader Program we want to use
		VAO1.Bind(); // set as current VAO
		glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0); // draw triangles, passing in: number of indices, datatype of indices, index of indices
		glfwSwapBuffers(window); // makes sure image is updated each frame
		glfwPollEvents(); // takes care of all GLFW events
	}

	VAO1.Delete(); // delete all objects we created
	VBO1.Delete();
	EBO1.Delete();
	shaderProgram.Delete();

	glfwDestroyWindow(window); // delete window
	glfwTerminate(); // terminate GLFW
	return 0; // program ends
}