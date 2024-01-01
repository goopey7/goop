// Sam Collier 2023
#pragma once

#include "Sampler.h"
#include "Texture.h"
#include "UniformBuffer.h"
#include <map>
#include <vector>
#include <volk.h>

namespace goop::sys::platform::vulkan
{
class Descriptor
{
  public:
	Descriptor(const Descriptor&) = delete;
	Descriptor& operator=(const Descriptor&) = delete;
	Descriptor(Context* ctx, UniformBuffer* ub, Sampler* sampler);
	~Descriptor();

	VkDescriptorSetLayout getLayout() const { return descriptorSetLayout; }
	VkDescriptorPool getPool() const { return descriptorPool; }
	VkDescriptorSet* getSet(uint8_t index, Texture* texture)
	{
		return &descriptorSets[texture][index];
	}

	void createDescriptorSet(Texture* texture);

  private:
	void createDescriptorSetLayout();
	void createDescriptorPool();

	VkDevice device;
	uint8_t maxFramesInFlight;
	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorPool descriptorPool;
	std::map<Texture*, std::vector<VkDescriptorSet>> descriptorSets;
	Sampler* sampler;
	UniformBuffer* ub;
};
} // namespace goop::sys::platform::vulkan
