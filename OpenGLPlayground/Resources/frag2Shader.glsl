#version 430

in vec2 texCoord;          // texture coordinate
in vec3 varyingNorm;       // world-space vertex normal
in vec3 varyingLightDir;   // vector pointing to the light
in vec3 varyingVertPos;    // vertex position in world space
in vec3 varyingHalfVec;    // vector between L and V
in vec4 shadow_coord;      // shadow texture coordinates

layout (binding=0) uniform sampler2D texSamp; // (binding=0) means texture unit 0
layout (binding=1) uniform sampler2DShadow shadowSamp;

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
uniform vec4 globalAmb;
uniform PositionalLight light;
uniform Material material;
uniform mat4 shadowMVP;

out vec4 color;

void main()
{
    // normalize the light, normal, and view vectors:
    vec3 L = normalize(varyingLightDir);
    vec3 N = normalize(varyingNorm);
    vec3 V = normalize(-v_matrix[3].xyz - varyingVertPos);
    vec3 H = normalize(varyingHalfVec);
    
    float cosTheta = dot(L, N); // angle between the light and surface normal
    float cosPhi = dot(H, N); // angle between the surface normal and halfway vector

    vec3 ambient = ((globalAmb * material.ambient) + (light.ambient * material.ambient)).xyz;
    
    float shadowLookup = textureProj(shadowSamp, shadow_coord); // 0 = in shadow, 1 = not in shadow
    if (shadowLookup == 0.0)
    {
        color = vec4(ambient, 1.0);
    }
    else
    {
        vec3 diffuse = light.diffuse.xyz * material.diffuse.xyz * max(cosTheta, 0.0);
        vec3 specular = light.specular.xyz * material.specular.xyz * pow(max(cosPhi, 0.0), material.shininess);
        color = vec4((ambient + diffuse + specular), 1.0);
    }

    vec4 texColor = texture(texSamp, texCoord);
    if (texColor != vec4(0.0, 0.0, 0.0, 1.0)) // if there is a texture
    {
        color = texColor * color;
    }
}