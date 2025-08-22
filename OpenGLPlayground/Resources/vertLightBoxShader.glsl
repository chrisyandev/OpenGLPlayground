#version 430 core

layout (location=0) in vec3 aPos;
layout (location=1) in vec2 aTexCoords;
layout (location=2) in vec3 aNormal;

uniform mat4 p_matrix;
uniform mat4 v_matrix;
uniform mat4 m_matrix;

void main()
{
    gl_Position = p_matrix * v_matrix * m_matrix * vec4(aPos, 1.0);
}