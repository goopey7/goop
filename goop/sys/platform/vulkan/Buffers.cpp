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
	instanceBuffers.resize(ctx->getMaxFramesInFlight());
	instanceBufferMemories.resize(ctx->getMaxFramesInFlight());
	indexBuffers.resize(ctx->getMaxFramesInFlight());
	indexBufferMemories.resize(ctx->getMaxFramesInFlight());

	vertexBufferStaging.resize(ctx->getMaxFramesInFlight());
	vertexBufferMemoryStaging.resize(ctx->getMaxFramesInFlight());
	instanceBufferStaging.resize(ctx->getMaxFramesInFlight());
	instanceBufferMemoryStaging.resize(ctx->getMaxFramesInFlight());
	indexBufferStaging.resize(ctx->getMaxFramesInFlight());
	indexBufferMemoryStaging.resize(ctx->getMaxFramesInFlight());

	vertexCounts.resize(ctx->getMaxFramesInFlight());
	instanceCounts.resize(ctx->getMaxFramesInFlight());
	indexCounts.resize(ctx->getMaxFramesInFlight());
	oldVertexCounts.resize(ctx->getMaxFramesInFlight());
	oldIndexCounts.resize(ctx->getMaxFramesInFlight());
	oldInstanceCounts.resize(ctx->getMaxFramesInFlight());

	instanceOffsets.resize(ctx->getMaxFramesInFlight());
	meshIDs.resize(ctx->getMaxFramesInFlight());
	indexOffsets.resize(ctx->getMaxFramesInFlight());

	for (size_t i = 0; i < oldVertexCounts.size(); i++)
	{
		oldVertexCounts[i] = -1;
		oldIndexCounts[i] = -1;
		oldInstanceCounts[i] = -1;
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
	if (instanceBuffers[currentFrame] != VK_NULL_HANDLE)
	{
		vkDestroyBuffer(*ctx, instanceBuffers[currentFrame], nullptr);
	}
	if (instanceBufferMemories[currentFrame] != VK_NULL_HANDLE)
	{
		vkFreeMemory(*ctx, instanceBufferMemories[currentFrame], nullptr);
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
	instanceBuffers[currentFrame] = instanceBufferStaging[currentFrame];
	instanceBufferMemories[currentFrame] = instanceBufferMemoryStaging[currentFrame];
	indexBuffers[currentFrame] = indexBufferStaging[currentFrame];
	indexBufferMemories[currentFrame] = indexBufferMemoryStaging[currentFrame];

	vertexBufferStaging[currentFrame] = VK_NULL_HANDLE;
	vertexBufferMemoryStaging[currentFrame] = VK_NULL_HANDLE;
	instanceBufferStaging[currentFrame] = VK_NULL_HANDLE;
	instanceBufferMemoryStaging[currentFrame] = VK_NULL_HANDLE;
	indexBufferStaging[currentFrame] = VK_NULL_HANDLE;
	indexBufferMemoryStaging[currentFrame] = VK_NULL_HANDLE;
	bReadyToSwap = false;
}

void Buffers::updateBuffers(uint32_t currentFrame, const Vertex* vertices, uint32_t vertexCount,
							const uint32_t* indices, uint32_t indexCount,
							const std::vector<uint32_t>* indexOffsets,
							const std::vector<uint32_t>* indexCounts,
							const std::map<uint32_t, std::vector<Instance>>* instances,
							uint32_t instanceCount, bool forceUpdate)
{
	if (!forceUpdate && (vertexCount == oldVertexCounts[currentFrame] &&
		indexCount == oldIndexCounts[currentFrame] &&
		instanceCount == oldInstanceCounts[currentFrame]))
	{
		return;
	}

	// clear vectors
	this->instanceOffsets[currentFrame].clear();
	this->meshIDs[currentFrame].clear();
	this->instanceCounts[currentFrame].clear();
	this->indexOffsets[currentFrame].clear();
	this->indexCounts[currentFrame].clear();
	this->vertexCounts[currentFrame] = 0;

	oldVertexCounts[currentFrame] = vertexCount;
	oldIndexCounts[currentFrame] = indexCount;
	oldInstanceCounts[currentFrame] = instanceCount;

	if (vertexCount == 0 || indexCount == 0 || instances->empty())
	{
		// create one vertex and index to keep the renderer happy
		Vertex v{};
		v.pos = glm::vec3(0.0f);
		v.color = glm::vec3(0.0f);
		v.texCoord = glm::vec2(0.0f);
		uint32_t i = 0;
		Instance instance{};
		createVertexBuffer(currentFrame, &v, 1);
		createInstanceBuffer(currentFrame, &instance, 1);
		createIndexBuffer(currentFrame, &i, 1);
	}
	else
	{
		createVertexBuffer(currentFrame, vertices, vertexCount);
		std::vector<Instance> instancesToRender;
		size_t instanceCount = instances->size();
		for (auto& [key, value] : *instances)
		{
			instanceOffsets[currentFrame].push_back(instancesToRender.size());
			instancesToRender.insert(instancesToRender.end(), value.begin(),
									 value.end());
			meshIDs[currentFrame].push_back(value.begin()->meshID);
			instanceCounts[currentFrame].push_back(value.size());
		}
		createInstanceBuffer(currentFrame, instancesToRender.data(), instancesToRender.size());
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
		if (instanceBuffers[i] != VK_NULL_HANDLE)
		{
			vkDestroyBuffer(*ctx, instanceBuffers[i], nullptr);
		}
		if (instanceBufferMemories[i] != VK_NULL_HANDLE)
		{
			vkFreeMemory(*ctx, instanceBufferMemories[i], nullptr);
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
	if (vertexBuffers[currentFrame] != VK_NULL_HANDLE)
	{
		vkDestroyBuffer(*ctx, vertexBuffers[currentFrame], nullptr);
		vkFreeMemory(*ctx, vertexBufferMemories[currentFrame], nullptr);
		vertexBuffers[currentFrame] = VK_NULL_HANDLE;
		vertexBufferMemories[currentFrame] = VK_NULL_HANDLE;
	}
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

void Buffers::createInstanceBuffer(uint32_t currentFrame, const Instance* instances,
								   uint32_t instanceCount)
{
	if (instanceBuffers[currentFrame] != VK_NULL_HANDLE)
	{
		vkDestroyBuffer(*ctx, instanceBuffers[currentFrame], nullptr);
		vkFreeMemory(*ctx, instanceBufferMemories[currentFrame], nullptr);
		instanceBuffers[currentFrame] = VK_NULL_HANDLE;
		instanceBufferMemories[currentFrame] = VK_NULL_HANDLE;
	}
	std::cout << "INSTANCE COUNT: " << instanceCount << std::endl;

	instanceBufferSize = sizeof(instances[0]) * instanceCount;

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(ctx, instanceBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				 stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(*ctx, stagingBufferMemory, 0, instanceBufferSize, 0, &data);
	memcpy(data, instances, (size_t)instanceBufferSize);
	vkUnmapMemory(*ctx, stagingBufferMemory);

	createBuffer(ctx, instanceBufferSize,
				 VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, instanceBufferStaging[currentFrame],
				 instanceBufferMemoryStaging[currentFrame]);

	copyBuffer(ctx, ctx->getCommandPool(), ctx->getGraphicsQueue(), stagingBuffer,
			   instanceBufferStaging[currentFrame], instanceBufferSize);

	vkDestroyBuffer(*ctx, stagingBuffer, nullptr);
	vkFreeMemory(*ctx, stagingBufferMemory, nullptr);
}

void Buffers::createIndexBuffer(uint32_t currentFrame, const uint32_t* indices, uint32_t indexCount)
{
	if (indexBuffers[currentFrame] != VK_NULL_HANDLE)
	{
		vkDestroyBuffer(*ctx, indexBuffers[currentFrame], nullptr);
		vkFreeMemory(*ctx, indexBufferMemories[currentFrame], nullptr);
		indexBuffers[currentFrame] = VK_NULL_HANDLE;
		indexBufferMemories[currentFrame] = VK_NULL_HANDLE;
	}
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
