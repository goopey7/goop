// Sam Collier 2023
#pragma once

#include "Texture.h"
#include "UniformBuffer.h"
#include <vector>
#include <vulkan/vulkan.h>

namespace goop::sys::platform::vulkan
{
class Descriptor
{
  public:
	Descriptor(const Descriptor&) = delete;
	Descriptor& operator=(const Descriptor&) = delete;
	Descriptor(Context* ctx, uint8_t maxFramesInFlight, UniformBuffer* ub, Texture* texture);
	~Descriptor();

	VkDescriptorSetLayout getLayout() const { return descriptorSetLayout; }
	VkDescriptorPool getPool() const { return descriptorPool; }
	VkDescriptorSet* getSet(uint8_t index) { return &descriptorSets[index]; }

  private:
	void createDescriptorSetLayout();
	void createDescriptorPool();
	void createDescriptorSets(UniformBuffer* ub);

	VkDevice device;
	uint8_t maxFramesInFlight;
	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorPool descriptorPool;
	std::vector<VkDescriptorSet> descriptorSets;
	Texture* texture;
};
} // namespace goop::sys::platform::vulkan
