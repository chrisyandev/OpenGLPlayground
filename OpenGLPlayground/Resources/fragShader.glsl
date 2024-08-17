#version 430

in vec2 texCoord;

uniform mat4 mv_matrix;
uniform mat4 p_matrix;

out vec4 color;

layout (binding=0) uniform sampler2D samp; // (binding=0) means texture unit 0

void main(void)
{
    color = texture(samp, texCoord);
}