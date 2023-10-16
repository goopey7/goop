// Sam Collier 2023

#include "MeshLoader_Assimp.h"
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <cassert>
#include <unordered_map>
#include <iostream>

#ifdef ML_ASSIMP
std::unique_ptr<goop::sys::MeshLoader> goop::sys::gMeshLoader = std::make_unique<goop::sys::platform::assimp::MeshLoader_Assimp>();
#endif

using namespace goop::sys::platform::assimp;

int MeshLoader_Assimp::initialize() { 
	MeshLoader::initialize();
	bIsInitialized = true;
	return 0; }

int MeshLoader_Assimp::destroy() { 
	bIsInitialized = false;
	return 0; }

void MeshLoader_Assimp::loadModel(const std::string& path)
{
	const aiScene* scene =
		importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs |
													 aiProcess_JoinIdenticalVertices);
	const auto mesh = scene->mMeshes[0];

	// aiProcess_JoinIdenticalVertices should have taken care of this
	// But for some reason it doesn't. It does mostly, but not completely
	std::unordered_map<Vertex, uint32_t> uniqueVertices{};
	std::unordered_map<uint32_t, uint32_t> dupIndexToUniqueIndex{};

	data->vertices.reserve(mesh->mNumVertices);
	for (uint32_t i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex{};
		vertex.pos = {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};
		vertex.texCoord = {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};

		if (uniqueVertices.count(vertex) == 0)
		{
			uniqueVertices[vertex] = static_cast<uint32_t>(data->vertices.size());
			dupIndexToUniqueIndex[i] = static_cast<uint32_t>(data->vertices.size());
			data->vertices.push_back(vertex);
		}
		else
		{
			dupIndexToUniqueIndex[i] = uniqueVertices[vertex];
		}
	}
	data->vertices.resize(data->vertices.size());

	for (uint32_t i=0; i < data->vertices.size(); i++)
	{
		for (uint32_t j = 0; j < data->vertices.size(); j++)
		{
			if (data->vertices[i] == data->vertices[j] && i != j)
			{
				std::cout << "Duplicate vertex found at " << i << " and " << j << std::endl;
			}
		}
	}

	data->indices.reserve(mesh->mNumFaces * 3);
	for (uint32_t i = 0; i < mesh->mNumFaces; i++)
	{
		const auto& face = mesh->mFaces[i];

		// aiProcess_Triangulate should have taken care of this
		assert(face.mNumIndices == 3);

		for (uint32_t j = 0; j < face.mNumIndices; j++)
		{
			data->indices.push_back(dupIndexToUniqueIndex[face.mIndices[j]]);
		}
	}
}

MeshLoader_Assimp::MeshLoader_Assimp()
{
}

MeshLoader_Assimp::~MeshLoader_Assimp()
{
}

