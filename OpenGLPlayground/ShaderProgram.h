#pragma once
#include<glad/glad.h>
#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include<cerrno>

class ShaderProgram
{
public:
	ShaderProgram(const char* vertFileName, const char* fragFileName);
	void Activate();
	void Delete();
	GLuint ID;
private:
	void CheckErrors(unsigned int shader, const char* type);
};

std::string GetFileContents(const char* filename);