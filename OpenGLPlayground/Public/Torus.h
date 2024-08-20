#pragma once
#include <cmath>
#include <vector>
#include <glm/glm.hpp>

class Torus
{
public:
	Torus();
	Torus(float innerRadius, float outerRadius, int numRings);

	// accessors
	int getNumVertices() { return numVertices; }
	int getNumIndices() { return numIndices; }
	std::vector<int> getIndices() { return indices; }
	std::vector<glm::vec3> getVertices() { return vertices; }
	std::vector<glm::vec2> getTexCoords() { return texCoords; }
	std::vector<glm::vec3> getNormals() { return normals; }
	std::vector<glm::vec3> getStangents() { return sTangents; }
	std::vector<glm::vec3> getTtangents() { return tTangents; }

private:
	int numVertices;
	int numIndices;
	int numRings;
	float innerRadius;
	float outerRadius;
	std::vector<int> indices;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> texCoords;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> sTangents;
	std::vector<glm::vec3> tTangents;

	void init();
};