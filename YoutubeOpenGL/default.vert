#version 330 core

layout (location = 0) in vec3 aPos; // vertex positions
layout (location = 1) in vec3 aColor; // colors
layout (location = 2) in vec2 aTex; // texture coordinates

out vec3 color; // outputs the color
out vec2 texCoord; // outputs the texture coordinates

uniform float scale; // controls the scale of the vertices

void main()
{
	gl_Position = vec4(aPos.x + aPos.x * scale, aPos.y + aPos.y * scale, aPos.z + aPos.z * scale, 1.0);
	color = aColor; // assigns the colors from the vertex data
	texCoord = aTex; // assigns the texture coordinates from the vertex data
}