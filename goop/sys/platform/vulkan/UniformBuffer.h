// Sam Collier 2023
#pragma once

#include <cstdint>
#include <vector>
#include <vulkan/vulkan_core.h>
#include <glm/glm.hpp>
namespace goop::sys::platform::vulkan
{
struct UniformBufferObject
{
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
};
class UniformBuffer
{
  public:
	UniformBuffer(const UniformBuffer&) = delete;
	UniformBuffer& operator=(const UniformBuffer&) = delete;
	UniformBuffer(VkDevice device, VkPhysicalDevice physicalDevice, uint8_t maxFramesInFlight);
	~UniformBuffer();

	void update(uint8_t currentFrame, const UniformBufferObject& ubo);
	VkBuffer getBuffer(uint8_t currentFrame) const { return uniformBuffers[currentFrame]; }

  private:
	void createUniformBuffers(VkPhysicalDevice physicalDevice);

	VkDevice device;
	uint8_t maxFramesInFlight;
	std::vector<VkBuffer> uniformBuffers;
	std::vector<VkDeviceMemory> uniformBuffersMemory;
	std::vector<void*> uniformBuffersData;
};
} // namespace goop::sys::platform::vulkan
