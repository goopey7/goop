// Sam Collier 2023

#include "Buffers.h"
#include "Context.h"
#include "Utils.h"
#include <cstring>

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <iostream>
#include <set>

using namespace goop::sys::platform::vulkan;

Buffers::Buffers(Context* ctx) : ctx(ctx)
{
	loadModel();
	createVertexBuffer();
	createIndexBuffer();
}

Buffers::~Buffers()
{
	vkDestroyBuffer(*ctx, indexBuffer, nullptr);
	vkFreeMemory(*ctx, indexBufferMemory, nullptr);
	vkDestroyBuffer(*ctx, vertexBuffer, nullptr);
	vkFreeMemory(*ctx, vertexBufferMemory, nullptr);
}

void Buffers::createVertexBuffer()
{
	VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(ctx, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				 stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(*ctx, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, vertices.data(), (size_t)bufferSize);
	vkUnmapMemory(*ctx, stagingBufferMemory);

	createBuffer(ctx, bufferSize,
				 VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);

	copyBuffer(ctx, ctx->getCommandPool(), ctx->getGraphicsQueue(), stagingBuffer, vertexBuffer,
			   bufferSize);

	vkDestroyBuffer(*ctx, stagingBuffer, nullptr);
	vkFreeMemory(*ctx, stagingBufferMemory, nullptr);
}

void Buffers::createIndexBuffer()
{
	VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(ctx, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				 stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(*ctx, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, indices.data(), (size_t)bufferSize);
	vkUnmapMemory(*ctx, stagingBufferMemory);

	createBuffer(ctx, bufferSize,
				 VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
				 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

	copyBuffer(ctx, ctx->getCommandPool(), ctx->getGraphicsQueue(), stagingBuffer, indexBuffer,
			   bufferSize);

	vkDestroyBuffer(*ctx, stagingBuffer, nullptr);
	vkFreeMemory(*ctx, stagingBufferMemory, nullptr);
}

void Buffers::loadModel()
{
	const aiScene* scene =
		importer.ReadFile("res/viking_room.obj", aiProcess_Triangulate | aiProcess_FlipUVs |
													 aiProcess_JoinIdenticalVertices);

	const auto mesh = scene->mMeshes[0];

	// aiProcess_JoinIdenticalVertices should have taken care of this
	// But for some reason it doesn't. It does mostly, but not completely
	std::unordered_map<Vertex, uint32_t> uniqueVertices{};
	std::unordered_map<uint32_t, uint32_t> dupIndexToUniqueIndex{};

	for (uint32_t i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex{};
		vertex.pos = {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};
		vertex.color = {1.f, 1.f, 1.f};
		vertex.texCoord = {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};

		if (uniqueVertices.count(vertex) == 0)
		{
			uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
			dupIndexToUniqueIndex[i] = static_cast<uint32_t>(vertices.size());
			vertices.push_back(vertex);
		}
		else
		{
			dupIndexToUniqueIndex[i] = uniqueVertices[vertex];
		}
	}

	for (uint32_t i=0; i < vertices.size(); i++)
	{
		for (uint32_t j = 0; j < vertices.size(); j++)
		{
			if (vertices[i] == vertices[j] && i != j)
			{
				std::cout << "Duplicate vertex found at " << i << " and " << j << std::endl;
			}
		}
	}

	indices.reserve(mesh->mNumFaces * 3);
	for (uint32_t i = 0; i < mesh->mNumFaces; i++)
	{
		const auto& face = mesh->mFaces[i];

		// aiProcess_Triangulate should have taken care of this
		assert(face.mNumIndices == 3);

		for (uint32_t j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(dupIndexToUniqueIndex[face.mIndices[j]]);
		}
	}
}
