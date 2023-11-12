// Sam Collier 2023
#pragma once

#include <goop/sys/Vertex.h>
#include <array>
#include <glm/glm.hpp>
#include <goop/sys/Renderer.h>
#include <goop/sys/platform/vulkan/Context.h>
#include <goop/sys/platform/vulkan/Pipeline.h>
#include <goop/sys/platform/vulkan/Swapchain.h>
#include <goop/sys/platform/vulkan/Buffers.h>
#include <goop/sys/platform/vulkan/Sync.h>
#include <goop/sys/platform/vulkan/Texture.h>
#include <memory>
#include <optional>
#include <thread>
#include <vector>

#include <volk.h>

#include <goop/sys/platform/vulkan/QueueFamilyIndices.h>
#include <goop/sys/platform/vulkan/SwapchainSupportInfo.h>

namespace goop::sys::platform::vulkan
{
class Renderer_Vulkan : public Renderer
{
  public:
	// Subsystem interface
	int initialize() final;
	int destroy() final;

	// Renderer interface
	void render() final;
	void addToRenderQueue(goop::res::mesh mesh, MeshLoader* meshLoader) final;
	void updateBuffersThread();

  private:
	std::vector<const char*> getRequiredExtensions();
	VkExtent2D selectExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
	void updateUniformBuffer(uint32_t currentFrame);

	void recreateSwapchain();

	Context* ctx;
	UniformBuffer* uniformBuffer;
	Descriptor* descriptor;
	Pipeline* pipeline;
	Swapchain* swapchain;
	Texture* texture;
	Buffers* buffers;
	Sync* sync;

	uint32_t currentFrame = 0;
	bool frameBufferResized = false;

	std::thread updateBuffers;
	std::mutex meshQueueMutex;
	VkDescriptorPool imguiPool;
};
} // namespace goop::sys::platform::vulkan
