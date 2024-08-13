#version 430

in vec4 varyingColor;

uniform mat4 mv_matrix;
uniform mat4 p_matrix;

out vec4 color;

void main(void)
{
    color = varyingColor;
}