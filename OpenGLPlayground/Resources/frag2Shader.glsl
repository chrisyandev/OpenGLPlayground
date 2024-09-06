#version 430

in vec2 texCoord;          // texture coordinate
in vec3 varyingNorm;       // world-space vertex normal
in vec3 varyingLightDir;   // vector pointing to the light
in vec3 varyingVertPos;    // vertex position in world space
in vec3 varyingHalfVec;    // vector between L and V
in vec4 shadow_coord;      // coordinates in the shadow map corresponding to the current pixel being rendered

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
uniform vec2 windowSize;

out vec4 color;

// Returns the shadow depth value for a texel at distance (x,y) from shadow_coord
float lookup(float offsetX, float offsetY)
{
    float t = textureProj(shadowSamp, shadow_coord +
        vec4(offsetX * (1 / windowSize.x) * shadow_coord.w,
             offsetY * (1 / windowSize.y) * shadow_coord.w,
             -0.01, 0.0));
             // the third parameter (-0.01) is an offset to counteract shadow acne
    return t; // 0.0 = entirely in shadow, 1.0 = entirely not in shadow
}

void main()
{
    float shadowFactor = 0.0;

    // normalize the light, normal, and view vectors:
    vec3 L = normalize(varyingLightDir);
    vec3 N = normalize(varyingNorm);
    vec3 V = normalize(-v_matrix[3].xyz - varyingVertPos);
    vec3 H = normalize(varyingHalfVec);

    // --- 4-sample dithered soft shadow ---
    // float softWidth = 2.5; // tunable amount of shadow spread
    // vec2 offset = mod(floor(gl_FragCoord.xy), 2.0) * softWidth; // produces one of 4 sample patterns depending on gl_FragCoord%2
    // shadowFactor += lookup(-1.5 * softWidth + offset.x, 1.5 * softWidth - offset.y);
    // shadowFactor += lookup(-1.5 * softWidth + offset.x, -0.5 * softWidth - offset.y);
    // shadowFactor += lookup(0.5 * softWidth + offset.x, 1.5 * softWidth - offset.y);
    // shadowFactor += lookup(0.5 * softWidth + offset.x, -0.5 * softWidth - offset.y);
    // shadowFactor = shadowFactor / 4.0; // shadowFactor is an average of the four sampled points
    // -------------------------------------

    // --- 64-sample high resolution soft shadow ---
    float stepSize = 2.5; // tunable amount of shadow spread
    float endDist = stepSize * 3.0 + stepSize / 2.0;
    for (float m = -endDist; m <= endDist; m = m + stepSize)
    {
       for (float n = -endDist; n <= endDist; n = n + stepSize)
       {
           shadowFactor += lookup(m,n);
       }
    }
    shadowFactor = shadowFactor / 64.0;
    // ---------------------------------------------
    
    float cosTheta = dot(L, N); // angle between the light and surface normal
    float cosPhi = dot(H, N); // angle between the surface normal and halfway vector

    vec3 ambient = (globalAmb * material.ambient + light.ambient * material.ambient).xyz;
    vec3 diffuse = light.diffuse.xyz * material.diffuse.xyz * max(cosTheta, 0.0);
    vec3 specular = light.specular.xyz * material.specular.xyz * pow(max(cosPhi, 0.0), material.shininess);

    color = vec4(ambient + (shadowFactor * (diffuse + specular)), 1.0);

    vec4 texColor = texture(texSamp, texCoord);
    if (texColor != vec4(0.0, 0.0, 0.0, 1.0)) // if there is a texture
    {
        color = texColor * color;
    }
}