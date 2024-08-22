#version 430

layout (location=0) in vec3 aPos;
layout (location=1) in vec2 aTexCoord;

uniform mat4 mv_matrix;
uniform mat4 p_matrix;

out vec2 texCoord; // texture coordinate output to rasterizer for interpolation
out vec4 varyingColor;

layout (binding=0) uniform sampler2D samp;

void main(void)
{
    gl_Position = p_matrix * mv_matrix * vec4(aPos, 1.0);
    texCoord = aTexCoord;

    // multiplies the location by 1/2 and then adds 1/2 to convert the range of values from [-1..+1] to [0..1]
    varyingColor = vec4(aPos, 1.0) * 0.5 + vec4(0.5, 0.5, 0.5, 0.5);
}