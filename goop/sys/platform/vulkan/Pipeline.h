// Sam Collier 2023
#pragma once

#include "Descriptor.h"
#include "Swapchain.h"
#include <vector>
#include <vulkan/vulkan.h>

namespace goop::sys::platform::vulkan
{
class Pipeline
{
  public:
	Pipeline(const Pipeline&) = delete;
	Pipeline& operator=(const Pipeline&) = delete;
	Pipeline(Context* ctx, Swapchain* swapchain, Descriptor* descriptor);
	~Pipeline();

	VkPipeline getPipeline() const { return pipeline; }
	VkPipelineLayout getPipelineLayout() const { return pipelineLayout; }

  private:
	VkPipeline pipeline;
	VkPipelineLayout pipelineLayout;
	VkShaderModule createShaderModule(const std::vector<char>& code);
	void createGraphicsPipeline(Swapchain* swapchain);
	void createPipelineLayout(VkDescriptorSetLayout descriptorSetLayout);
	Context* ctx;
};
} // namespace goop::sys::platform::vulkan
