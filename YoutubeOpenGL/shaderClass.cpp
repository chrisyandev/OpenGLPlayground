#include "shaderClass.h"

/* Reads a text file and outputs its contents. */
std::string get_file_contents(const char* filename)
{
	std::ifstream in(filename, std::ios::binary);
	if (in)
	{
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);
	}
	throw(errno);
}

Shader::Shader(const char* vertexFile, const char* fragmentFile)
{
	std::string vertexCode = get_file_contents(vertexFile);
	std::string fragmentCode = get_file_contents(fragmentFile);

	const char* vertexShaderSource = vertexCode.c_str(); // convert source strings into character arrays
	const char* fragmentShaderSource = fragmentCode.c_str();

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER); // create vertex shader object
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL); // reference the source
	glCompileShader(vertexShader);

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER); // create fragment shader object
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL); // reference the source
	glCompileShader(fragmentShader);

	ID = glCreateProgram(); // create shader program object
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	glLinkProgram(ID); // wrap up after attaching shaders

	glDeleteShader(vertexShader); // no longer needed so delete these
	glDeleteShader(fragmentShader);
}

void Shader::Activate()
{
	glUseProgram(ID);
}

void Shader::Delete()
{
	glDeleteProgram(ID);
}