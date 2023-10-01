// Sam Collier 2023
#pragma once

#include <vector>
#include <vulkan/vulkan.h>
#include <goop/sys/platform/vulkan/Descriptor.h>

namespace goop::sys::platform::vulkan
{
class Pipeline
{
  public:
	Pipeline(const Pipeline&) = delete;
	Pipeline& operator=(const Pipeline&) = delete;
	Pipeline(Context* ctx, VkExtent2D swapchainExtent, VkRenderPass renderPass,
			 Descriptor* descriptor);
	~Pipeline();

	VkPipeline getPipeline() const { return pipeline; }
	VkPipelineLayout getPipelineLayout() const { return pipelineLayout; }

  private:
	VkPipeline pipeline;
	VkPipelineLayout pipelineLayout;
	VkDevice device;
	VkShaderModule createShaderModule(const std::vector<char>& code);
	void createGraphicsPipeline(VkExtent2D swapchainExtent, VkRenderPass renderPass);
	void createPipelineLayout(VkDescriptorSetLayout descriptorSetLayout);
};
} // namespace goop::sys::platform::vulkan