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
	vertexBuffers.resize(ctx->getMaxFramesInFlight());
	vertexBufferMemories.resize(ctx->getMaxFramesInFlight());
	indexBuffers.resize(ctx->getMaxFramesInFlight());
	indexBufferMemories.resize(ctx->getMaxFramesInFlight());

	vertexBufferStaging.resize(ctx->getMaxFramesInFlight());
	vertexBufferMemoryStaging.resize(ctx->getMaxFramesInFlight());
	indexBufferStaging.resize(ctx->getMaxFramesInFlight());
	indexBufferMemoryStaging.resize(ctx->getMaxFramesInFlight());

	vertexCounts.resize(ctx->getMaxFramesInFlight());
	indexCounts.resize(ctx->getMaxFramesInFlight());
	oldVertexCounts.resize(ctx->getMaxFramesInFlight());
	oldIndexCounts.resize(ctx->getMaxFramesInFlight());

	indexOffsets.resize(ctx->getMaxFramesInFlight());

	for (size_t i = 0; i < oldVertexCounts.size(); i++)
	{
		oldVertexCounts[i] = -1;
		oldIndexCounts[i] = -1;
	}
}

void Buffers::swapBuffers(uint32_t currentFrame)
{
	if (!bReadyToSwap)
	{
		return;
	}

	if (vertexBuffers[currentFrame] != VK_NULL_HANDLE)
	{
		vkDestroyBuffer(*ctx, vertexBuffers[currentFrame], nullptr);
	}
	if (vertexBufferMemories[currentFrame] != VK_NULL_HANDLE)
	{
		vkFreeMemory(*ctx, vertexBufferMemories[currentFrame], nullptr);
	}
	if (indexBuffers[currentFrame] != VK_NULL_HANDLE)
	{
		vkDestroyBuffer(*ctx, indexBuffers[currentFrame], nullptr);
	}
	if (indexBufferMemories[currentFrame] != VK_NULL_HANDLE)
	{
		vkFreeMemory(*ctx, indexBufferMemories[currentFrame], nullptr);
	}

	vertexBuffers[currentFrame] = vertexBufferStaging[currentFrame];
	vertexBufferMemories[currentFrame] = vertexBufferMemoryStaging[currentFrame];
	indexBuffers[currentFrame] = indexBufferStaging[currentFrame];
	indexBufferMemories[currentFrame] = indexBufferMemoryStaging[currentFrame];

	vertexBufferStaging[currentFrame] = VK_NULL_HANDLE;
	vertexBufferMemoryStaging[currentFrame] = VK_NULL_HANDLE;
	indexBufferStaging[currentFrame] = VK_NULL_HANDLE;
	indexBufferMemoryStaging[currentFrame] = VK_NULL_HANDLE;
	bReadyToSwap = false;
}

void Buffers::updateBuffers(uint32_t currentFrame, const Vertex* vertices, uint32_t vertexCount,
							const uint32_t* indices, uint32_t indexCount,
							const std::vector<uint32_t>* indexOffsets,
							const std::vector<uint32_t>* indexCounts)
{
	if (vertexCount == oldVertexCounts[currentFrame] && indexCount == oldIndexCounts[currentFrame])
	{
		return;
	}
	oldVertexCounts[currentFrame] = vertexCount;
	oldIndexCounts[currentFrame] = indexCount;
	if (vertexCount == 0 || indexCount == 0)
	{
		// create one vertex and index to keep the renderer happy
		Vertex v{};
		v.pos = glm::vec3(0.0f);
		v.color = glm::vec3(0.0f);
		v.texCoord = glm::vec2(0.0f);
		uint32_t i = 0;
		createVertexBuffer(currentFrame, &v, 1);
		createIndexBuffer(currentFrame, &i, 1);
	}
	else
	{
		createVertexBuffer(currentFrame, vertices, vertexCount);
		createIndexBuffer(currentFrame, indices, indexCount);
	}
	this->vertexCounts[currentFrame] = vertexCount;
	this->indexCounts[currentFrame] = *indexCounts;
	this->indexOffsets[currentFrame] = *indexOffsets;
	bReadyToSwap = true;
}

Buffers::~Buffers() { clearBuffers(); }

void Buffers::clearBuffers()
{
	for (size_t i = 0; i < vertexBuffers.size(); i++)
	{
		if (vertexBuffers[i] != VK_NULL_HANDLE)
		{
			vkDestroyBuffer(*ctx, vertexBuffers[i], nullptr);
		}
		if (vertexBufferMemories[i] != VK_NULL_HANDLE)
		{
			vkFreeMemory(*ctx, vertexBufferMemories[i], nullptr);
		}
		if (indexBuffers[i] != VK_NULL_HANDLE)
		{
			vkDestroyBuffer(*ctx, indexBuffers[i], nullptr);
		}
		if (indexBufferMemories[i] != VK_NULL_HANDLE)
		{
			vkFreeMemory(*ctx, indexBufferMemories[i], nullptr);
		}
	}
}

void Buffers::createVertexBuffer(uint32_t currentFrame, const Vertex* vertices,
								 uint32_t vertexCount)
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
				 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBufferStaging[currentFrame],
				 vertexBufferMemoryStaging[currentFrame]);

	copyBuffer(ctx, ctx->getCommandPool(), ctx->getGraphicsQueue(), stagingBuffer,
			   vertexBufferStaging[currentFrame], vertexBufferSize);

	vkDestroyBuffer(*ctx, stagingBuffer, nullptr);
	vkFreeMemory(*ctx, stagingBufferMemory, nullptr);
}

void Buffers::createIndexBuffer(uint32_t currentFrame, const uint32_t* indices, uint32_t indexCount)
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
				 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBufferStaging[currentFrame],
				 indexBufferMemoryStaging[currentFrame]);

	copyBuffer(ctx, ctx->getCommandPool(), ctx->getGraphicsQueue(), stagingBuffer,
			   indexBufferStaging[currentFrame], indexBufferSize);

	vkDestroyBuffer(*ctx, stagingBuffer, nullptr);
	vkFreeMemory(*ctx, stagingBufferMemory, nullptr);
}
