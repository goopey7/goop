// Sam Collier 2023
#pragma once

#include "Vertex.h"
#include <array>
#include <glm/glm.hpp>
#include <goop/sys/Renderer.h>
#include <goop/sys/platform/vulkan/Context.h>
#include <goop/sys/platform/vulkan/Pipeline.h>
#include <memory>
#include <optional>
#include <vector>
#include <vulkan/vulkan_core.h>

#include <goop/sys/platform/vulkan/QueueFamilyIndices.h>
#include <goop/sys/platform/vulkan/SwapchainSupportInfo.h>

namespace goop::sys::platform::vulkan
{

const std::vector<Vertex> vertices = {
	{{-0.5f, -0.5f}, {1.f, 0.f, 0.f}},
	{{0.5f, -0.5f}, {0.f, 1.f, 0.f}},
	{{0.5f, 0.5f}, {0.f, 0.f, 1.f}},
	{{-0.5f, 0.5f}, {1.f, 1.f, 0.f}},
};

const std::vector<uint32_t> indices = {
	0, 1, 2, 2, 3, 0,
};

class Renderer_Vulkan : public Renderer
{
  public:
	// Subsystem interface
	int initialize() final;
	int destroy() final;

	// Renderer interface
	void render() final;

  private:
	void createSwapchain(VkSwapchainKHR oldSwapchain = VK_NULL_HANDLE);
	void createImageViews();
	void createRenderPass();
	void createFramebuffers();
	void createCommandPool();
	void createVertexBuffer();
	void createIndexBuffer();
	void createCommandBuffers();
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

	VkSwapchainKHR swapchain;
	std::vector<VkImage> swapchainImages;
	VkFormat swapchainImageFormat;
	VkExtent2D swapchainExtent;
	std::vector<VkImageView> swapchainImageViews;
	VkRenderPass renderPass;

	Context* ctx;
	UniformBuffer* uniformBuffer;
	Descriptor* descriptor;
	Pipeline* pipeline;

	std::vector<VkFramebuffer> swapchainFramebuffers;
	VkCommandPool commandPool;
	std::vector<VkCommandBuffer> commandBuffers;
	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	uint32_t currentFrame = 0;
	bool frameBufferResized = false;

	// TODO driver developers recommend storing vertex and index buffers into a single VkBuffer and
	// use offsets. That way the data is more cache friendly
	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;
	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;
};
} // namespace goop::sys::platform::vulkan
