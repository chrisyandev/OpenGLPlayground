#pragma once
#include <cmath>
#include <vector>
#include <glm/glm.hpp>

class Sphere
{
public:
	Sphere();
	Sphere(int prec);

	// accessors
	int getNumVertices() { return numVertices; }
	int getNumIndices() { return numIndices; }
	std::vector<int> getIndices() { return indices; }
	std::vector<glm::vec3> getVertices() { return vertices; }
	std::vector<glm::vec2> getTexCoords() { return texCoords; }
	std::vector<glm::vec3> getNormals() { return normals; }

private:
	int numVertices;
	int numIndices;
	std::vector<int> indices;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> texCoords;
	std::vector<glm::vec3> normals;

	void init(int);
	float toRadians(float degrees);
};