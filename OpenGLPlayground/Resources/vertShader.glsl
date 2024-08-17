#version 430

layout (location=0) in vec3 aPos;
layout (location=1) in vec2 aTexCoord;

uniform mat4 mv_matrix;
uniform mat4 p_matrix;

out vec2 texCoord; // texture coordinate output to rasterizer for interpolation

layout (binding=0) uniform sampler2D samp;

void main(void)
{
    gl_Position = p_matrix * mv_matrix * vec4(aPos, 1.0);
    texCoord = aTexCoord;
}