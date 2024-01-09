#version 330 core

layout (location = 0) in vec3 aPos; // vertex positions
layout (location = 1) in vec3 aColor; // colors
layout (location = 2) in vec2 aTex; // texture coordinates

out vec3 color; // outputs the color
out vec2 texCoord; // outputs the texture coordinates

uniform float scale; // controls the scale of the vertices

uniform mat4 model; // inputs the matrices needed for 3D viewing with perspective
uniform mat4 view;
uniform mat4 proj;

void main()
{
	gl_Position = proj * view * model * vec4(aPos, 1.0); // outputs the coordinates of all vertices
	color = aColor; // assigns the colors from the vertex data
	texCoord = aTex; // assigns the texture coordinates from the vertex data
}