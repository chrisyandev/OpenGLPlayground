#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 rotationMatrix;

uniform float radians;

void main()
{
    /* Begin attempt quaternion rotation */

    mat4 rotationMatrixConjugate = mat4(rotationMatrix);
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            if (i == j) // don't modify the diagonal
            {
                continue;
            }
            rotationMatrixConjugate[i] *= -1.0;
        }
    }

    mat4 vectorAsMatrix = mat4(
         0.0,     aPos.x,  aPos.y,  aPos.z,
        -aPos.x,  0.0,     aPos.z, -aPos.y,
        -aPos.y, -aPos.z,  0.0,     aPos.x,
        -aPos.z,  aPos.y, -aPos.x,  0.0
    );

    mat4 qvq = rotationMatrix * vectorAsMatrix * rotationMatrixConjugate;

    vec4 rotatedPoint = vec4(qvq[0][0], qvq[0][1], qvq[0][2], 1.0);

    //gl_Position = rotatedPoint;

    /* End attempt quaternion rotation */




    mat4 yRotationMatrix = mat4(
    cos(radians),  0.0, -sin(radians),  0.0,
             0.0,  1.0,           0.0,  0.0,
    sin(radians),  0.0,  cos(radians),  0.0,
             0.0,  0.0,           0.0,  1.0
    );

    gl_Position = yRotationMatrix * vec4(aPos.x, aPos.y, aPos.z, 1.0);
}