#version 430

layout (location=0) in vec3 aPos;
layout (location=1) in vec2 aTexCoord;
layout (location=2) in vec3 aNorm;

layout (binding=0) uniform sampler2D texSamp;
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
uniform mat4 sh_mvp_matrix;
uniform vec2 windowSize;

out vec2 texCoord;          // texture coordinate
out vec3 varyingNorm;       // world-space vertex normal
out vec3 varyingLightDir;   // vector pointing to the light
out vec3 varyingVertPos;    // vertex position in world space
out vec3 varyingHalfVec;    // vector between L and V
out vec4 shadow_coord;      // shadow texture coordinates

void main()
{
    // output to the rasterizer for interpolation
    texCoord = aTexCoord;
    varyingVertPos = (m_matrix * vec4(aPos, 1.0)).xyz;
    varyingLightDir = light.position - varyingVertPos;
    varyingNorm = (n_matrix * vec4(aNorm, 1.0)).xyz;
    varyingHalfVec = (varyingLightDir - varyingVertPos).xyz;

    shadow_coord = sh_mvp_matrix * vec4(aPos, 1.0);
    gl_Position = p_matrix * v_matrix * m_matrix * vec4(aPos, 1.0);
}