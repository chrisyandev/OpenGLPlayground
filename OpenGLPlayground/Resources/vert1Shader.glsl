#version 430

layout (location=0) in vec3 aPos;

uniform mat4 sh_mvp_matrix;

void main()
{
	gl_Position = sh_mvp_matrix * vec4(aPos, 1.0);
}