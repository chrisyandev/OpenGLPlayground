#include"Texture.h"

Texture::Texture(const char* image, GLenum texType, GLenum slot, GLenum format, GLenum pixelType)
{
	type = texType;

	int widthImg, heightImg, numColCh; // width, height, num of color channels of the image
	stbi_set_flip_vertically_on_load(true); // flip the image so it appears right side up
	unsigned char* bytes = stbi_load(image, &widthImg, &heightImg, &numColCh, 0); // read the image from a file and store it in bytes

	glGenTextures(1, &ID); 	// generate 1 texture object name
	glActiveTexture(slot); // assigns the texture to a texture unit
	glBindTexture(texType, ID);

	glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR); 	// configure the type of algorithm that is used to make the image smaller or bigger
	glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(texType, GL_TEXTURE_WRAP_S, GL_REPEAT); // configure the way the texture repeats
	glTexParameteri(texType, GL_TEXTURE_WRAP_T, GL_REPEAT);

	/*
	* Extra lines in case you choose to use GL_CLAMP_TO_BORDER
	* float flatColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
	* glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, flatColor);
	*/

	glTexImage2D(texType, 0, GL_RGBA, widthImg, heightImg, 0, format, pixelType, bytes); // assign the image to the texture object
	glGenerateMipmap(texType); // generate mipmaps

	stbi_image_free(bytes); // delete the image data as it is already in the texture object

	glBindTexture(texType, 0); // unbind the texture object so that it can't accidentally be modified
}

void Texture::texUnit(Shader& shader, const char* uniform, GLuint unit)
{
	GLuint texUni = glGetUniformLocation(shader.ID, uniform); // get the location of the uniform
	shader.Activate(); // shader needs to be activated before changing the value of a uniform
	glUniform1i(texUni, unit); // set the value of the uniform
}

void Texture::Bind()
{
	glBindTexture(type, ID);
}

void Texture::Unbind()
{
	glBindTexture(type, 0);
}

void Texture::Delete()
{
	glDeleteTextures(1, &ID);
}