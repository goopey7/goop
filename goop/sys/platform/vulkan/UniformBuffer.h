// Sam Collier 2023
#pragma once

#include <goop/sys/platform/vulkan/Context.h>
#include <cstdint>
#include <vector>
#include <volk.h>
#include <glm/glm.hpp>
namespace goop::sys::platform::vulkan
{
struct UniformBufferObject
{
	glm::mat4 view;
	glm::mat4 proj;
};
class UniformBuffer
{
  public:
	UniformBuffer(const UniformBuffer&) = delete;
	UniformBuffer& operator=(const UniformBuffer&) = delete;
	UniformBuffer(Context* ctx);
	~UniformBuffer();

	void update(uint8_t currentFrame, const UniformBufferObject& ubo);
	VkBuffer getBuffer(uint8_t currentFrame) const { return uniformBuffers[currentFrame]; }

  private:
	void createUniformBuffers();

	Context* ctx;
	uint8_t maxFramesInFlight;
	std::vector<VkBuffer> uniformBuffers;
	std::vector<VkDeviceMemory> uniformBuffersMemory;
	std::vector<void*> uniformBuffersData;
};
} // namespace goop::sys::platform::vulkan
