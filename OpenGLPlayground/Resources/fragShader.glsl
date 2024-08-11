#version 430

out vec4 color;

uniform mat4 mv_matrix;
uniform mat4 p_matrix;

in vec4 varyingColor;

void main(void)
{
    color = varyingColor;
}