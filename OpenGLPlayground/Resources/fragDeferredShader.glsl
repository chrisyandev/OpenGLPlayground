#version 430 core

in vec2 TexCoords;

uniform sampler2D gPos;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

struct Light
{
    vec3 position;
    vec3 color;
    float linear;
    float quadratic;
};

const int NUM_LIGHTS = 4;
uniform Light lights[NUM_LIGHTS];
uniform vec3 camPos;

out vec4 FragColor;

void main()
{   
    // retrieve data from gbuffer
    vec3 FragPosSamp = texture(gPos, TexCoords).rgb;
    vec3 NormSamp = texture(gNormal, TexCoords).rgb;
    vec3 DiffSamp = texture(gAlbedoSpec, TexCoords).rgb;
    float SpecSamp = texture(gAlbedoSpec, TexCoords).a;

    // --- Debug ---
    if (TexCoords.x > 0.5)
    {
        FragColor = vec4(1.0, 1.0, 1.0, 1.0);
        return;
    }
    if (FragPosSamp == vec3(0.0, 0.0, 0.0))
    {
        FragColor = vec4(1.0, 0.0, 0.0, 1.0);
        return;
    }
    if (NormSamp == vec3(0.0, 0.0, 0.0))
    {
        FragColor = vec4(0.0, 1.0, 0.0, 1.0);
        return;
    }
    if (DiffSamp == vec3(0.0, 0.0, 0.0))
    {
        FragColor = vec4(0.0, 0.0, 1.0, 1.0);
        return;
    }
    if (SpecSamp == vec3(0.0, 0.0, 0.0))
    {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
        return;
    }
    // -------------
    
    // then calculate lighting as usual
    vec3 lighting  = DiffSamp * 0.1; // hard-coded ambient component
    vec3 viewDir  = normalize(camPos - FragPosSamp);
    for (int i = 0; i < NUM_LIGHTS; ++i)
    {
        // diffuse
        vec3 lightDir = normalize(lights[i].position - FragPosSamp);
        vec3 diffuse = max(dot(NormSamp, lightDir), 0.0) * DiffSamp * lights[i].color;
        // specular
        vec3 halfwayDir = normalize(lightDir + viewDir);  
        float spec = pow(max(dot(NormSamp, halfwayDir), 0.0), 16.0);
        vec3 specular = lights[i].color * spec * SpecSamp;
        // attenuation
        float distance = length(lights[i].position - FragPosSamp);
        float attenuation = 1.0 / (1.0 + lights[i].linear * distance + lights[i].quadratic * distance * distance);
        diffuse *= attenuation;
        specular *= attenuation;
        lighting += diffuse + specular;
    }

    FragColor = vec4(lighting, 1.0);
}