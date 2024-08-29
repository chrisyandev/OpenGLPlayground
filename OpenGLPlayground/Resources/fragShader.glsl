#version 430

in vec2 texCoord;
in vec4 varyingColor;

uniform mat4 m_matrix;
uniform mat4 v_matrix;
uniform mat4 p_matrix;

out vec4 color;

layout (binding=0) uniform sampler2D samp; // (binding=0) means texture unit 0

void main(void)
{
    color = texture(samp, texCoord);

    if (color == vec4(0.0, 0.0, 0.0, 1.0)) // if no texture
    {
        color = varyingColor;
    }
}