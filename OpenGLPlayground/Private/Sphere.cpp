#include <cmath>
#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include "Utils.h"
#include "Sphere.h"

Sphere::Sphere()
{
	init(48);
}

Sphere::Sphere(int numSlices) // number of slices, precision of sphere
{
	init(numSlices);
}

void Sphere::init(int numSlices)
{
	numVertices = (numSlices + 1) * (numSlices + 1);
	numIndices = numSlices * numSlices * 6;

	// populate vectors
	for (int i = 0; i < numVertices; i++)
	{
		vertices.push_back(glm::vec3());
	}
	for (int i = 0; i < numVertices; i++)
	{
		texCoords.push_back(glm::vec2());
	}
	for (int i = 0; i < numVertices; i++)
	{
		normals.push_back(glm::vec3());
	}
	for (int i = 0; i < numIndices; i++)
	{
		indices.push_back(0);
	}

	// calculate vertex positions that form the sphere
	for (int i = 0; i <= numSlices; i++) // for each horizontal slice
	{
		for (int j = 0; j <= numSlices; j++) // for each vertical slice
		{
			float y = (float)cos(Utils::toRadians(180.0f - i * 180.0f / numSlices));
			float x = -(float)cos(Utils::toRadians(j * 360.0f / numSlices)) * (float)abs(cos(asin(y)));
			float z = (float)sin(Utils::toRadians(j * 360.0f / numSlices)) * (float)abs(cos(asin(y)));
			vertices[i * (numSlices + 1) + j] = glm::vec3(x, y, z);
			texCoords[i * (numSlices + 1) + j] = glm::vec2((float)j / numSlices, (float)i / numSlices);
			normals[i * (numSlices + 1) + j] = glm::vec3(x, y, z);
		}
	}

	// calculate indices for two triangles which point to neighboring vertices to the right, top, and to the top-right of vertex j
	for (int i = 0; i < numSlices; i++)
	{
		for (int j = 0; j < numSlices; j++)
		{
			indices[6 * (i * numSlices + j) + 0] = i * (numSlices + 1) + j;
			indices[6 * (i * numSlices + j) + 1] = i * (numSlices + 1) + j + 1;
			indices[6 * (i * numSlices + j) + 2] = (i + 1) * (numSlices + 1) + j;
			indices[6 * (i * numSlices + j) + 3] = i * (numSlices + 1) + j + 1;
			indices[6 * (i * numSlices + j) + 4] = (i + 1) * (numSlices + 1) + j + 1;
			indices[6 * (i * numSlices + j) + 5] = (i + 1) * (numSlices + 1) + j;
		}
	}
}