#version 430

in vec2 texCoord;
in vec3 varyingNormal;
in vec3 varyingLightDir;
in vec3 varyingVertPos;
in vec3 varyingHalfVector;

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
    // normalize the light, normal, and view vectors:
    vec3 L = normalize(varyingLightDir);
    vec3 N = normalize(varyingNormal);
    vec3 V = normalize(-v_matrix[3].xyz - varyingVertPos);
    vec3 H = normalize(varyingHalfVector);
    
    // get the angle between the light and surface normal:
    float cosTheta = dot(L, N);
    
    // angle between the normal and halfway vector
    float cosPhi = dot(H, N);
    
    // compute ADS contributions (per pixel), and combine to build output color:
    vec3 ambient = ((globalAmbient * material.ambient) + (light.ambient * material.ambient)).xyz;
    vec3 diffuse = light.diffuse.xyz * material.diffuse.xyz * max(cosTheta, 0.0);
    vec3 specular = light.specular.xyz * material.specular.xyz * pow(max(cosPhi, 0.0), material.shininess);

    vec4 texColor = texture(samp, texCoord);
    vec4 adsColor = vec4((ambient + diffuse + specular), 1.0);

    if (texColor == vec4(0.0, 0.0, 0.0, 1.0)) // if no texture
    {
        color = adsColor;
    }
    else
    {
        color = texColor * adsColor;
    }
}