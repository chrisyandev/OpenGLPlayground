#version 330 core

layout (location = 0) in vec3 aPos;

uniform float radians;

void main()
{
    mat4 yRotationMatrix = mat4(
    cos(radians),  0.0, -sin(radians),  0.0,
             0.0,  1.0,           0.0,  0.0,
    sin(radians),  0.0,  cos(radians),  0.0,
             0.0,  0.0,           0.0,  1.0
    );

    gl_Position = yRotationMatrix * vec4(aPos.x, aPos.y, aPos.z, 1.0);
}