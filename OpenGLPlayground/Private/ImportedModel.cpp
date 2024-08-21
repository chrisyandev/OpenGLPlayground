#include <fstream>
#include <sstream>
#include <glm/glm.hpp>
#include "Utils.h"
#include "ImportedModel.h"

// ------------ Imported Model class
ImportedModel::ImportedModel(const std::string& fileName)
{
	ModelImporter modelImporter = ModelImporter();
	modelImporter.parseOBJ(fileName); // uses modelImporter to get vertex information
	numVertices = modelImporter.getNumVertices();
	std::vector<float> verts = modelImporter.getVertices();
	std::vector<float> tcs = modelImporter.getTextureCoordinates();
	std::vector<float> norms = modelImporter.getNormals();

	for (int i = 0; i < numVertices; i++)
	{
		vertices.push_back(glm::vec3(verts[i * 3], verts[i * 3 + 1], verts[i * 3 + 2]));
		texCoords.push_back(glm::vec2(tcs[i * 2], tcs[i * 2 + 1]));
		normals.push_back(glm::vec3(norms[i * 3], norms[i * 3 + 1], norms[i * 3 + 2]));
	}
}

// -------------- Model Importer class
ModelImporter::ModelImporter() {}

void ModelImporter::parseOBJ(const std::string& filename)
{
	std::string filePath = Utils::getResourcePath() + filename;
	float x, y, z;
	std::string content;
	std::ifstream fileStream(filePath.c_str(), std::ios::in);
	std::string line = "";

	while (!fileStream.eof())
	{
		getline(fileStream, line);

		if (line.compare(0, 2, "v ") == 0) // vertex position ("v" case)
		{
			std::stringstream ss(line.erase(0, 2));
			ss >> x; ss >> y; ss >> z; // extract the vertex position values
			vertVals.push_back(x);
			vertVals.push_back(y);
			vertVals.push_back(z);
		}
		if (line.compare(0, 2, "vt") == 0) // texture coordinates ("vt" case)
		{
			std::stringstream ss(line.erase(0, 2));
			ss >> x; ss >> y; // extract texture coordinate values
			stVals.push_back(x);
			stVals.push_back(y);
		}
		if (line.compare(0, 2, "vn") == 0) // vertex normals ("vn" case)
		{
			std::stringstream ss(line.erase(0, 2));
			ss >> x; ss >> y; ss >> z; // extract the normal vector values
			normVals.push_back(x);
			normVals.push_back(y);
			normVals.push_back(z);
		}
		if (line.compare(0, 2, "f ") == 0) // triangle faces ("f" case)
		{
			std::string oneCorner, v, t, n;
			std::stringstream ss(line.erase(0, 2));
			for (int i = 0; i < 3; i++) {
				getline(ss, oneCorner, ' '); // extract triangle face references
				std::stringstream oneCornerSS(oneCorner);
				getline(oneCornerSS, v, '/');
				getline(oneCornerSS, t, '/');
				getline(oneCornerSS, n, '/');
				int vertRef = (stoi(v) - 1) * 3; // "stoi" converts string to integer
				int tcRef = (stoi(t) - 1) * 2;
				int normRef = (stoi(n) - 1) * 3;
				triangleVerts.push_back(vertVals[vertRef]); // build vector of vertices
				triangleVerts.push_back(vertVals[vertRef + 1]);
				triangleVerts.push_back(vertVals[vertRef + 2]);
				textureCoords.push_back(stVals[tcRef]); // build vector of texture coords
				textureCoords.push_back(stVals[tcRef + 1]);
				normals.push_back(normVals[normRef]); //… and normals
				normals.push_back(normVals[normRef + 1]);
				normals.push_back(normVals[normRef + 2]);
			}
		}
	}
}