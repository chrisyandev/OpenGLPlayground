#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Texture.h"
#include "shaderClass.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"

const unsigned int width = 800;
const unsigned int height = 800;

GLfloat vertices[] =
{	// COORDINATES			// COLORS			// TEXCOORDS
	-0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	0.0f, 0.0f, // side of pyramid
	-0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	5.0f, 0.0f, // side of pyramid
	 0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	0.0f, 0.0f, // side of pyramid
	 0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	5.0f, 0.0f, // side of pyramid
	 0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,	2.5f, 5.0f  // tip of pyramid
};

GLuint indices[] =	// describes vertices order
{
	0, 1, 2,
	0, 2, 3,
	0, 1, 4,
	1, 2, 4,
	2, 3, 4,
	3, 0, 4
};

int main()
{
	glfwInit(); // initialize GLFW

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // tell GLFW what version of OpenGL we're using
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // OpenGL 3.3
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // using the CORE profile so that we only have modern functions

	GLFWwindow* window = glfwCreateWindow(width, height, "YoutubeOpenGL", NULL, NULL); // create a window of 800 by 800 pixels with a title
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window); // introduce the window into the current context

	gladLoadGL(); // load GLAD so it configures OpenGL

	glViewport(0, 0, width, height); // specify the viewport with bottom left and top right coordinates

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

	Texture popCat("brick.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE); // create texture
	popCat.texUnit(shaderProgram, "tex0", 0);

	float rotation = 0.0f; // for rotating the pyramid
	double prevTime = glfwGetTime();

	glEnable(GL_DEPTH_TEST); // enables the depth buffer

	while (!glfwWindowShouldClose(window)) // main loop
	{
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f); // background color
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clean the back buffer and depth buffer
		shaderProgram.Activate(); // set the Shader Program we want to use

		double currTime = glfwGetTime();
		if (currTime - prevTime >= 1 / 60) // for rotating the pyramid
		{
			rotation += 0.5f;
			prevTime = currTime;
		}

		glm::mat4 model = glm::mat4(1.0f); // initializes matrices
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 proj = glm::mat4(1.0f);

		model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f)); // performs different transformations to each matrix
		view = glm::translate(view, glm::vec3(0.0f, -0.5f, -2.0f));
		proj = glm::perspective(glm::radians(45.0f), (float)(width / height), 0.1f, 100.0f);

		int modelLoc = glGetUniformLocation(shaderProgram.ID, "model"); // outputs the matrices into the vertex shader
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		int viewLoc = glGetUniformLocation(shaderProgram.ID, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		int projLoc = glGetUniformLocation(shaderProgram.ID, "proj");
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));

		glUniform1f(uniID, 0.5f); // set the value for "scale"
		popCat.Bind(); // so it gets rendered
		VAO1.Bind(); // set as current VAO
		glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(int), GL_UNSIGNED_INT, 0); // draw triangles, passing in: number of indices, datatype of indices, index of indices
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