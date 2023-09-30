// Sam Collier 2023

#include "UniformBuffer.h"

#include "BufferHelpers.h"
#include <cstring>

using namespace goop::sys::platform::vulkan;

UniformBuffer::UniformBuffer(VkDevice device, VkPhysicalDevice physicalDevice, uint8_t maxFramesInFlight)
	: device(device), maxFramesInFlight(maxFramesInFlight)
{
	createUniformBuffers(physicalDevice);
}

void UniformBuffer::createUniformBuffers(VkPhysicalDevice physicalDevice)
{
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);

	uniformBuffers.resize(maxFramesInFlight);
	uniformBuffersMemory.resize(maxFramesInFlight);
	uniformBuffersData.resize(maxFramesInFlight);

	for (size_t i = 0; i < maxFramesInFlight; i++)
	{
		createBuffer(device, physicalDevice, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
					 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
					 uniformBuffers[i], uniformBuffersMemory[i]);

		// persistently map the buffer memory so that we can update it without unmapping it
		vkMapMemory(device, uniformBuffersMemory[i], 0, bufferSize, 0, &uniformBuffersData[i]);
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
		vkDestroyBuffer(device, uniformBuffers[i], nullptr);
		vkFreeMemory(device, uniformBuffersMemory[i], nullptr);
	}
}

