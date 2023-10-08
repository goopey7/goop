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

	vertices.reserve(mesh->mNumVertices);
	for (uint32_t i = 0; i < mesh->mNumVertices; i++)
	{
		vertices.push_back({{mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z},
							{1.f, 1.f, 1.f},
							{mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y}});
	}

	uint32_t numDuplicateVertices = 0;
	for (uint32_t i = 0; i < mesh->mNumVertices; i++)
	{
		const auto& vertex = vertices[i];
		for (uint32_t j = i + 1; j < mesh->mNumVertices; j++)
		{
			if (vertex == vertices[j])
			{
				// Print or log duplicate vertices for debugging
				std::cout << "----------------------------------------------------------"
						  << std::endl;
				std::cout << "Duplicate vertices found at indices " << i << " and " << j
						  << std::endl;
				std::cout << "Position: " << vertex.pos.x << ", " << vertex.pos.y << ", "
						  << vertex.pos.z << std::endl;
				std::cout << "Color: " << vertex.color.r << ", " << vertex.color.g << ", "
						  << vertex.color.b << std::endl;
				std::cout << "Texcoord: " << vertex.texCoord.x << ", " << vertex.texCoord.y
						  << std::endl;

				std::cout << "Other Position: " << vertices[j].pos.x << ", " << vertices[j].pos.y
						  << ", " << vertices[j].pos.z << std::endl;
				std::cout << "Other Color: " << vertices[j].color.r << ", " << vertices[j].color.g
						  << ", " << vertices[j].color.b << std::endl;
				std::cout << "Other Texcoord: " << vertices[j].texCoord.x << ", "
						  << vertices[j].texCoord.y << std::endl;
				std::cout << "----------------------------------------------------------"
						  << std::endl;
				numDuplicateVertices++;
			}
		}
	}

	std::cout << "Number of duplicate vertices: " << numDuplicateVertices << std::endl;

	// check for duplicate vertices and throw error if found
	std::set<Vertex> uniqueVertices(vertices.begin(), vertices.end());
	//assert(uniqueVertices.size() == vertices.size());

	indices.reserve(mesh->mNumFaces * 3);
	for (uint32_t i = 0; i < mesh->mNumFaces; i++)
	{
		const auto& face = mesh->mFaces[i];

		// aiProcess_Triangulate should have taken care of this
		assert(face.mNumIndices == 3);

		indices.push_back(face.mIndices[0]);
		indices.push_back(face.mIndices[1]);
		indices.push_back(face.mIndices[2]);
	}
}
