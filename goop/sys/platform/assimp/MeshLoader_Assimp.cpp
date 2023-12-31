// Sam Collier 2023

#include "MeshLoader_Assimp.h"
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <cassert>
#include <iostream>
#include <unordered_map>

using namespace goop::sys::platform::assimp;

int MeshLoader_Assimp::initialize()
{
	MeshLoader::initialize();
	bIsInitialized = true;
	return 0;
}

int MeshLoader_Assimp::destroy()
{
	bIsInitialized = false;
	return 0;
}

uint32_t MeshLoader_Assimp::load(const std::string& path)
{
	int index = data->size();

	if (unloadedSlots.size() > 0)
	{
		index = unloadedSlots.back();
		unloadedSlots.pop_back();
	}

	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs |
													   aiProcess_JoinIdenticalVertices);
	const auto mesh = scene->mMeshes[0];

	// aiProcess_JoinIdenticalVertices should have taken care of this
	// But for some reason it doesn't. It does mostly, but not completely
	std::unordered_map<Vertex, uint32_t> uniqueVertices{};
	std::unordered_map<uint32_t, uint32_t> dupIndexToUniqueIndex{};

	MeshImportData* mid = &data->emplace(index, MeshImportData{}).first->second;
	mid->path = path.c_str();

	mid->vertices.reserve(mesh->mNumVertices);

	for (uint32_t i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex{};
		vertex.pos = {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};
		if (mesh->mTextureCoords[0] != nullptr)
		{
			vertex.texCoord = {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};
		}

		if (uniqueVertices.count(vertex) == 0)
		{
			uniqueVertices[vertex] = static_cast<uint32_t>(mid->vertices.size());
			dupIndexToUniqueIndex[i] = static_cast<uint32_t>(mid->vertices.size());
			mid->vertices.push_back(vertex);
		}
		else
		{
			dupIndexToUniqueIndex[i] = uniqueVertices[vertex];
		}
	}
	mid->vertices.resize(mid->vertices.size());

	for (uint32_t i = 0; i < mid->vertices.size(); i++)
	{
		for (uint32_t j = 0; j < mid->vertices.size(); j++)
		{
			if (mid->vertices[i] == mid->vertices[j] && i != j)
			{
				std::cout << "Duplicate vertex found at " << i << " and " << j << std::endl;
			}
		}
	}

	mid->indices.reserve(mesh->mNumFaces * 3);
	for (uint32_t i = 0; i < mesh->mNumFaces; i++)
	{
		const auto& face = mesh->mFaces[i];

		// aiProcess_Triangulate should have taken care of this
		assert(face.mNumIndices == 3);

		for (uint32_t j = 0; j < face.mNumIndices; j++)
		{
			mid->indices.push_back(dupIndexToUniqueIndex[face.mIndices[j]]);
		}
	}

	loadedMeshes[path] = index;
	return index;
}

MeshLoader_Assimp::MeshLoader_Assimp() {}

MeshLoader_Assimp::~MeshLoader_Assimp() {}

uint32_t MeshLoader_Assimp::unload(const std::string& path)
{
	unloadedSlots.push_back(loadedMeshes[path]);
	data->erase(loadedMeshes[path]);
	loadedMeshes.erase(path);
	return 0;
}

