// Sam Collier 2023

#include "Buffers.h"
#include "Context.h"
#include "Utils.h"
#include <goop/Core.h>
#include <cstring>
#include <iostream>
#include <set>
#include <stdexcept>

using namespace goop::sys::platform::vulkan;

Buffers::Buffers(Context* ctx) : ctx(ctx)
{
	/*
	if (!goop::sys::gResourceManager->getMeshLoader()->isInitialized())
	{
		throw std::runtime_error(
			"mesh loader not initialized. mesh loader must be initialized before the renderer");
	}
	*/
	if (!goop::rm->getMeshLoader())
	{
		throw std::runtime_error(
			"mesh loader not initialized. mesh loader must be initialized before the renderer");
	}

	/*
	const MeshImportData* mid = &goop::rm->getMeshLoader()
									 ->getData()
									 ->data()[goop::res::VIKING_ROOM];
	vertexCount = mid->vertices.size();
	indexCount = mid->indices.size();
	createVertexBuffer(mid);
	createIndexBuffer(mid);
	*/
}

Buffers::~Buffers()
{
	if (vertexBuffer != VK_NULL_HANDLE)
	{
		vkDestroyBuffer(*ctx, vertexBuffer, nullptr);
	}
	if (vertexBufferMemory != VK_NULL_HANDLE)
	{
		vkFreeMemory(*ctx, vertexBufferMemory, nullptr);
	}
	if (indexBuffer != VK_NULL_HANDLE)
	{
		vkDestroyBuffer(*ctx, indexBuffer, nullptr);
	}
	if (indexBufferMemory != VK_NULL_HANDLE)
	{
		vkFreeMemory(*ctx, indexBufferMemory, nullptr);
	}
}

void Buffers::createVertexBuffer(const MeshImportData* mid)
{
	std::cout << "VERTEX BUFFER SIZE: " << mid->vertices.size() << std::endl;

	VkDeviceSize bufferSize = sizeof(mid->vertices[0]) * mid->vertices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(ctx, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				 stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(*ctx, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, mid->vertices.data(), (size_t)bufferSize);
	vkUnmapMemory(*ctx, stagingBufferMemory);

	createBuffer(ctx, bufferSize,
				 VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);

	copyBuffer(ctx, ctx->getCommandPool(), ctx->getGraphicsQueue(), stagingBuffer, vertexBuffer,
			   bufferSize);

	vkDestroyBuffer(*ctx, stagingBuffer, nullptr);
	vkFreeMemory(*ctx, stagingBufferMemory, nullptr);
}

void Buffers::createIndexBuffer(const MeshImportData* mid)
{
	VkDeviceSize bufferSize = sizeof(mid->indices[0]) * mid->indices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(ctx, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				 stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(*ctx, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, mid->indices.data(), (size_t)bufferSize);
	vkUnmapMemory(*ctx, stagingBufferMemory);

	createBuffer(ctx, bufferSize,
				 VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
				 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

	copyBuffer(ctx, ctx->getCommandPool(), ctx->getGraphicsQueue(), stagingBuffer, indexBuffer,
			   bufferSize);

	vkDestroyBuffer(*ctx, stagingBuffer, nullptr);
	vkFreeMemory(*ctx, stagingBufferMemory, nullptr);
}
