#include <cmath>
#include <vector>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include "Utils.h"
#include "Torus.h"

Torus::Torus()
	: innerRadius{ 0.5f }
	, outerRadius{ 0.2f }
	, numRings{ 48 }
{
	init();
}

Torus::Torus(float innerRadius, float outerRadius, int numRings)
	: innerRadius{ innerRadius }
	, outerRadius{ outerRadius }
	, numRings{ numRings }
{
	init();
}

void Torus::init()
{
	numVertices = (numRings + 1) * (numRings + 1);
	numIndices = numRings * numRings * 6;

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
	for (int i = 0; i < numVertices; i++)
	{
		sTangents.push_back(glm::vec3());
	}
	for (int i = 0; i < numVertices; i++)
	{
		tTangents.push_back(glm::vec3());
	}
	for (int i = 0; i < numIndices; i++)
	{
		indices.push_back(0);
	}

	// calculate first ring
	for (int i = 0; i < numRings + 1; i++)
	{
		float angle = Utils::toRadians(i * 360.0f / numRings);

		// build the ring by rotating points around the origin, then moving them outward
		glm::mat4 rMat = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 0.0f, 1.0f));
		glm::vec3 initPos(rMat * glm::vec4(0.0f, outerRadius, 0.0f, 1.0f));
		vertices[i] = glm::vec3(initPos + glm::vec3(innerRadius, 0.0f, 0.0f));

		// compute texture coordinates for each vertex on the ring
		texCoords[i] = glm::vec2(0.0f, ((float)i / (float)numRings));

		// compute tangents and normals -- first tangent is Y-axis rotated around Z
		rMat = glm::rotate(glm::mat4(1.0f), angle + (3.14159f / 2.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		tTangents[i] = glm::vec3(rMat * glm::vec4(0.0f, -1.0f, 0.0f, 1.0f));
		sTangents[i] = glm::vec3(glm::vec3(0.0f, 0.0f, -1.0f)); // second tangent is -Z
		normals[i] = glm::cross(tTangents[i], sTangents[i]); // their cross product is the normal
	}

	// rotate the first ring about Y to get the other rings
	for (int ring = 1; ring < numRings + 1; ring++)
	{
		for (int vert = 0; vert < numRings + 1; vert++)
		{
			// rotate the vertex positions of the original ring around the Y axis
			float angle = (float)(Utils::toRadians(ring * 360.0f / numRings));
			glm::mat4 rMat = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f));
			vertices[ring * (numRings + 1) + vert] = glm::vec3(rMat * glm::vec4(vertices[vert], 1.0f));

			// compute the texture coordinates for the vertices in the new rings
			texCoords[ring * (numRings + 1) + vert] = glm::vec2((float)ring * 2.0f / (float)numRings, texCoords[vert].t);
			
			// rotate the tangent and bitangent vectors around the Y axis
			rMat = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f));
			sTangents[ring * (numRings + 1) + vert] = glm::vec3(rMat * glm::vec4(sTangents[vert], 1.0f));
			rMat = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f));
			tTangents[ring * (numRings + 1) + vert] = glm::vec3(rMat * glm::vec4(tTangents[vert], 1.0f));
			
			// rotate the normal vector around the Y axis
			rMat = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f));
			normals[ring * (numRings + 1) + vert] = glm::vec3(rMat * glm::vec4(normals[vert], 1.0f));
		}
	}

	// calculate triangle indices corresponding to the two triangles built per vertex
	for (int ring = 0; ring < numRings; ring++)
	{
		for (int vert = 0; vert < numRings; vert++)
		{
			indices[((ring * numRings + vert) * 2) * 3 + 0] = ring * (numRings + 1) + vert;
			indices[((ring * numRings + vert) * 2) * 3 + 1] = (ring + 1) * (numRings + 1) + vert;
			indices[((ring * numRings + vert) * 2) * 3 + 2] = ring * (numRings + 1) + vert + 1;
			indices[((ring * numRings + vert) * 2 + 1) * 3 + 0] = ring * (numRings + 1) + vert + 1;
			indices[((ring * numRings + vert) * 2 + 1) * 3 + 1] = (ring + 1) * (numRings + 1) + vert;
			indices[((ring * numRings + vert) * 2 + 1) * 3 + 2] = (ring + 1) * (numRings + 1) + vert + 1;
		}
	}
}

