// Sam Collier 2023
#pragma once

#include <vector>
#include <vulkan/vulkan.h>

namespace goop::sys::platform::vulkan
{
class Pipeline
{
  public:
	Pipeline(const Pipeline&) = delete;
	Pipeline& operator=(const Pipeline&) = delete;
	Pipeline(VkDevice device, VkExtent2D swapchainExtent, VkRenderPass renderPass,
			 VkDescriptorSetLayout descriptorSetLayout);
	VkPipeline getPipeline() const { return pipeline; }
	VkPipelineLayout getPipelineLayout() const { return pipelineLayout; }
	~Pipeline();

  private:
	VkPipeline pipeline;
	VkPipelineLayout pipelineLayout;
	VkDevice device;
	VkShaderModule createShaderModule(const std::vector<char>& code);
	void createGraphicsPipeline(VkExtent2D swapchainExtent, VkRenderPass renderPass);
	void createPipelineLayout(VkDescriptorSetLayout descriptorSetLayout);
};
} // namespace goop::sys::platform::vulkan
