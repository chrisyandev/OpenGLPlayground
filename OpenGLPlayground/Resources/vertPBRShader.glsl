#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;

uniform mat4 m_matrix;
uniform mat4 v_matrix;
uniform mat4 p_matrix;
uniform mat3 n_matrix;

out vec3 WorldPos;
out vec2 TexCoords;
out vec3 Normal;

void main()
{
    WorldPos = vec3(m_matrix * vec4(aPos, 1.0));
    TexCoords = aTexCoords;
    Normal = n_matrix * aNormal;   

    gl_Position =  p_matrix * v_matrix * vec4(WorldPos, 1.0);
}