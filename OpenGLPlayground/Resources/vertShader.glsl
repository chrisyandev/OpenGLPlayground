#version 430

layout (location=0) in vec3 position;

uniform mat4 mv_matrix;
uniform mat4 p_matrix;

out vec4 varyingColor;

void main(void)
{
    gl_Position = p_matrix * mv_matrix * vec4(position, 1.0);
    varyingColor = vec4(position, 1.0) * 0.5 + vec4(0.5, 0.5, 0.5, 0.5); // multiplies the location by 1/2 and then adds 1/2 to convert the range of values from [-1..+1] to [0..1]
}