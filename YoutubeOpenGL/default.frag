#version 330 core

out vec4 FragColor; // outputs colors in RGBA

in vec3 color; // inputs the color from the vertex shader
in vec2 texCoord; // inputs the texture coordinates from the vertex shader

uniform sampler2D tex0; // gets the texture unit from main()

void main()
{
	FragColor = texture(tex0, texCoord);
}