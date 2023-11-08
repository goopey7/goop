// Sam Collier 2023

#include "Buffers.h"
#include "Context.h"
#include "Utils.h"
#include <cstring>
#include <goop/Core.h>
#include <iostream>
#include <set>
#include <stdexcept>

using namespace goop::sys::platform::vulkan;

Buffers::Buffers(Context* ctx) : ctx(ctx)
{
	if (!goop::rm->getMeshLoader())
	{
		throw std::runtime_error(
			"mesh loader not initialized. mesh loader must be initialized before the renderer");
	}
}

void Buffers::swapBuffers()
{
	if (!bReadyToSwap)
	{
		return;
	}
	vertexBuffer = vertexBufferStaging;
	vertexBufferMemory = vertexBufferMemoryStaging;
	indexBuffer = indexBufferStaging;
	indexBufferMemory = indexBufferMemoryStaging;

	vertexBufferStaging = VK_NULL_HANDLE;
	vertexBufferMemoryStaging = VK_NULL_HANDLE;
	indexBufferStaging = VK_NULL_HANDLE;
	indexBufferMemoryStaging = VK_NULL_HANDLE;
	bReadyToSwap = false;
}

void Buffers::updateBuffers(const Vertex* vertices, uint32_t vertexCount, const uint32_t* indices,
							uint32_t indexCount)
{
	if (bReadyToSwap)
	{
		clearBuffers();
	}
	createVertexBuffer(vertices, vertexCount);
	createIndexBuffer(indices, indexCount);
	this->vertexCount = vertexCount;
	this->indexCount = indexCount;
	bReadyToSwap = true;
}

Buffers::~Buffers() { clearBuffers(); }

void Buffers::clearBuffers()
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

	if (vertexBufferStaging != VK_NULL_HANDLE)
	{
		vkDestroyBuffer(*ctx, vertexBufferStaging, nullptr);
	}
	if (vertexBufferMemoryStaging != VK_NULL_HANDLE)
	{
		vkFreeMemory(*ctx, vertexBufferMemoryStaging, nullptr);
	}
	if (indexBufferStaging != VK_NULL_HANDLE)
	{
		vkDestroyBuffer(*ctx, indexBufferStaging, nullptr);
	}
	if (indexBufferMemoryStaging != VK_NULL_HANDLE)
	{
		vkFreeMemory(*ctx, indexBufferMemoryStaging, nullptr);
	}
}

void Buffers::createVertexBuffer(const Vertex* vertices, uint32_t vertexCount)
{
	std::cout << "VERTEX COUNT: " << vertexCount << std::endl;

	vertexBufferSize = sizeof(vertices[0]) * vertexCount;

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(ctx, vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				 stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(*ctx, stagingBufferMemory, 0, vertexBufferSize, 0, &data);
	memcpy(data, vertices, (size_t)vertexBufferSize);
	vkUnmapMemory(*ctx, stagingBufferMemory);

	createBuffer(ctx, vertexBufferSize,
				 VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBufferStaging, vertexBufferMemoryStaging);

	copyBuffer(ctx, ctx->getCommandPool(), ctx->getGraphicsQueue(), stagingBuffer,
			   vertexBufferStaging, vertexBufferSize);

	vkDestroyBuffer(*ctx, stagingBuffer, nullptr);
	vkFreeMemory(*ctx, stagingBufferMemory, nullptr);
}

void Buffers::createIndexBuffer(const uint32_t* indices, uint32_t indexCount)
{
	indexBufferSize = sizeof(indices[0]) * indexCount;

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(ctx, indexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				 stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(*ctx, stagingBufferMemory, 0, indexBufferSize, 0, &data);
	memcpy(data, indices, (size_t)indexBufferSize);
	vkUnmapMemory(*ctx, stagingBufferMemory);

	createBuffer(ctx, indexBufferSize,
				 VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
				 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBufferStaging, indexBufferMemoryStaging);

	copyBuffer(ctx, ctx->getCommandPool(), ctx->getGraphicsQueue(), stagingBuffer,
			   indexBufferStaging, indexBufferSize);

	vkDestroyBuffer(*ctx, stagingBuffer, nullptr);
	vkFreeMemory(*ctx, stagingBufferMemory, nullptr);
}

