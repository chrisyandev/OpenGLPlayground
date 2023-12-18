#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>

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

	glClearColor(0.07f, 0.13f, 0.17f, 1.0f); // background color
	glClear(GL_COLOR_BUFFER_BIT); // clean the back buffer and assign new color to it
	glfwSwapBuffers(window); // swap back buffer with front buffer

	while (!glfwWindowShouldClose(window)) // main loop
	{
		glfwPollEvents(); // takes care of all GLFW events
	}

	glfwDestroyWindow(window); // delete window
	glfwTerminate(); // terminate GLFW
	return 0; // program ends
}