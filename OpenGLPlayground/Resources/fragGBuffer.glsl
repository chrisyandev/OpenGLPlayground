#version 430 core

in vec3 WorldPos;
in vec2 TexCoords;
in vec3 Normal;

layout (binding=0) uniform sampler2D texture_diffuse;
layout (binding=1) uniform sampler2D texture_specular;

layout (location=0) out vec3 gPos; // writes to the first color attachment
layout (location=1) out vec3 gNormal; // writes to the second color attachment
layout (location=2) out vec4 gAlbedoSpec; // writes to the third color attachment

void main()
{
    // store the fragment position vector in the first gbuffer texture
    gPos = WorldPos;
    // also store the per-fragment normals into the gbuffer
    gNormal = normalize(Normal);
    // and the diffuse per-fragment color
    gAlbedoSpec.rgb = texture(texture_diffuse, TexCoords).rgb;
    // store specular intensity in gAlbedoSpec's alpha component
    gAlbedoSpec.a = texture(texture_specular, TexCoords).r;
}