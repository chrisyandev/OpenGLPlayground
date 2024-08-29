#version 430

layout (location=0) in vec3 aPos;
layout (location=1) in vec2 aTexCoord;
layout (location=2) in vec3 aNorm;

layout (binding=0) uniform sampler2D samp;

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

out vec2 texCoord; // texture coordinate output to rasterizer for interpolation
out vec4 varyingColor;

void main(void)
{
    vec4 color;

    vec4 P = m_matrix * vec4(aPos, 1.0);                        // convert vertex position to world space
    vec3 N = normalize((n_matrix * vec4(aNorm,1.0)).xyz);       // convert normal to world space
    vec3 L = normalize(light.position - P.xyz);                 // calculate world space light vector (from vertex to light)
    vec3 V = normalize(-v_matrix[3].xyz - P.xyz);               // view vector is from vertex to cam, cam position is extracted from view matrix
    vec3 R = reflect(-L, N);                                    // R is reflection of -L with respect to surface normal N

    // ambient, diffuse, and specular contributions
    vec3 ambient = ((globalAmbient * material.ambient) + (light.ambient * material.ambient)).xyz;
    vec3 diffuse = light.diffuse.xyz * material.diffuse.xyz * max(dot(N,L), 0.0);
    vec3 specular = material.specular.xyz * light.specular.xyz * pow(max(dot(R,V), 0.0f), material.shininess);

    gl_Position = p_matrix * v_matrix * m_matrix * vec4(aPos, 1.0);
    texCoord = aTexCoord;
    varyingColor = vec4((ambient + diffuse + specular), 1.0);
}