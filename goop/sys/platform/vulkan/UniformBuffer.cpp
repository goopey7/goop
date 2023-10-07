// Sam Collier 2023

#include "UniformBuffer.h"

#include "Utils.h"
#include <cstring>

using namespace goop::sys::platform::vulkan;

UniformBuffer::UniformBuffer(Context* ctx, uint8_t maxFramesInFlight)
	: ctx(ctx), maxFramesInFlight(maxFramesInFlight)
{
	createUniformBuffers();
}

void UniformBuffer::createUniformBuffers()
{
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);

	uniformBuffers.resize(maxFramesInFlight);
	uniformBuffersMemory.resize(maxFramesInFlight);
	uniformBuffersData.resize(maxFramesInFlight);

	for (size_t i = 0; i < maxFramesInFlight; i++)
	{
		createBuffer(ctx, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
					 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
					 uniformBuffers[i], uniformBuffersMemory[i]);

		// persistently map the buffer memory so that we can update it without unmapping it
		vkMapMemory(*ctx, uniformBuffersMemory[i], 0, bufferSize, 0, &uniformBuffersData[i]);
	}
}

void UniformBuffer::update(uint8_t currentFrame, const UniformBufferObject& ubo)
{
	memcpy(uniformBuffersData[currentFrame], &ubo, sizeof(ubo));
}

UniformBuffer::~UniformBuffer()
{
	for (size_t i = 0; i < maxFramesInFlight; i++)
	{
		vkDestroyBuffer(*ctx, uniformBuffers[i], nullptr);
		vkFreeMemory(*ctx, uniformBuffersMemory[i], nullptr);
	}
}

