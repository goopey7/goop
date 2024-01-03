// Sam Collier 2023

#include <goop/sys/platform/assimp/MeshLoader_Assimp.h>

using namespace goop::sys;

int MeshLoader::initialize()
{
	data = std::make_unique<std::map<uint32_t, MeshImportData>>();
	return 0;
}

int MeshLoader::loadPrimitive(Primitive primitive)
{
	int index = data->size();
	if (unloadedSlots.size() > 0)
	{
		index = unloadedSlots.back();
		unloadedSlots.pop_back();
	}
	MeshImportData* mid = &data->emplace(index, MeshImportData{}).first->second;

	if (std::holds_alternative<Box>(primitive))
	{
		Box box = std::get<Box>(primitive);
		mid->vertices.reserve(8);
		mid->indices.reserve(36);

		std::vector<float> cubeVertices = {
			// Front face
			box.x, box.y, box.z,   // Vertex 0
			box.x, -box.y, box.z,  // Vertex 1
			-box.x, -box.y, box.z, // Vertex 2
			-box.x, box.y, box.z,  // Vertex 3
			// Back face
			box.x, box.y, -box.z,	// Vertex 4
			box.x, -box.y, -box.z,	// Vertex 5
			-box.x, -box.y, -box.z, // Vertex 6
			-box.x, box.y, -box.z	// Vertex 7
		};

		std::vector<unsigned int> cubeIndices = {// Front face
												 2, 1, 0, 0, 3, 2,
												 // Right face
												 1, 5, 4, 4, 0, 1,
												 // Back face
												 5, 6, 7, 7, 4, 5,
												 // Left face
												 6, 2, 3, 3, 7, 6,
												 // Bottom face
												 6, 5, 1, 1, 2, 6,
												 // Top face
												 3, 0, 4, 4, 7, 3};

		std::vector<float> cubeTextureCoords = {
			0.0f, 1.0f, // Front bottom-left
			1.0f, 1.0f, // Front bottom-right
			1.0f, 0.0f, // Front top-right
			0.0f, 0.0f, // Front top-left

			1.0f, 1.0f, // Back bottom-left
			0.0f, 1.0f, // Back bottom-right
			0.0f, 0.0f, // Back top-right
			1.0f, 0.0f	// Back top-left
		};

		int texCoordIndex = 0;
		for (int i = 0; i < cubeVertices.size(); i += 3)
		{
			Vertex vertex{};
			vertex.pos = glm::vec3(cubeVertices[i], cubeVertices[i + 1], cubeVertices[i + 2]);
			vertex.texCoord =
				glm::vec2(cubeTextureCoords[texCoordIndex], cubeTextureCoords[texCoordIndex + 1]);
			vertex.color = glm::vec3(1.0f, 1.0f, 1.0f);
			mid->vertices.push_back(vertex);
			texCoordIndex += 2;
		}

		for (int i = 0; i < cubeIndices.size(); i++)
		{
			mid->indices.push_back(cubeIndices[i]);
		}
	}
	return index;
}
