#version 430

in vec2 texCoord;
in vec4 varyingColor;

layout (binding=0) uniform sampler2D samp; // (binding=0) means texture unit 0

struct PositionalLight
{
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec3 position;
};

struct Material
{ 
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
};

uniform mat4 m_matrix;
uniform mat4 v_matrix;
uniform mat4 p_matrix;
uniform mat4 n_matrix;
uniform vec4 globalAmbient;
uniform PositionalLight light;
uniform Material material;

out vec4 color;

void main(void)
{
    vec4 texColor = texture(samp, texCoord);
    color = texColor * varyingColor;

    if (color == vec4(0.0, 0.0, 0.0, 1.0)) // if no texture
    {
        color = varyingColor;
    }
}