// Sam Collier 2023
#pragma once

#include <vector>
#include <vulkan/vulkan.h>

namespace goop::sys::platform::vulkan
{
class Descriptor
{
  public:
	Descriptor(const Descriptor&) = delete;
	Descriptor& operator=(const Descriptor&) = delete;
	Descriptor(VkDevice device, uint8_t maxFramesInFlight, VkBuffer* uniformBuffers);
	~Descriptor();

	VkDescriptorSetLayout getLayout() const { return descriptorSetLayout; }
	VkDescriptorPool getPool() const { return descriptorPool; }
	VkDescriptorSet* getSet(uint8_t index) { return &descriptorSets[index]; }

  private:
	void createDescriptorSetLayout();
	void createDescriptorPool();
	void createDescriptorSets(VkBuffer* uniformBuffers);

	VkDevice device;
	uint8_t maxFramesInFlight;
	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorPool descriptorPool;
	std::vector<VkDescriptorSet> descriptorSets;
};
} // namespace goop::sys::platform::vulkan
