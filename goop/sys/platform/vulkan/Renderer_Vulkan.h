// Sam Collier 2023
#pragma once

#include "Vertex.h"
#include <array>
#include <glm/glm.hpp>
#include <goop/sys/Renderer.h>
#include <goop/sys/platform/vulkan/Context.h>
#include <goop/sys/platform/vulkan/Pipeline.h>
#include <goop/sys/platform/vulkan/Swapchain.h>
#include <goop/sys/platform/vulkan/Buffers.h>
#include <memory>
#include <optional>
#include <vector>
#include <vulkan/vulkan_core.h>

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

  private:
	void createSyncObjects();

	std::vector<const char*> getRequiredExtensions();
	VkExtent2D selectExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	bool checkValidationLayerSupport();
	bool isDeviceSuitable(VkPhysicalDevice device);
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	bool checkDeviceCompatibility(VkPhysicalDevice device);
	VkSurfaceFormatKHR selectSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR selectPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	void destroySwapchainDependents();
	VkShaderModule createShaderModule(const std::vector<char>& bytecode);
	void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
	void updateUniformBuffer(uint32_t currentFrame);

	void recreateSwapchain();

	Context* ctx;
	UniformBuffer* uniformBuffer;
	Descriptor* descriptor;
	Pipeline* pipeline;
	Swapchain* swapchain;
	Buffers* buffers;

	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	uint32_t currentFrame = 0;
	bool frameBufferResized = false;
};
} // namespace goop::sys::platform::vulkan
