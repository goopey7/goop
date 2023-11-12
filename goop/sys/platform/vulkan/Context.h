// Sam Collier 2023
#pragma once

#include <vector>

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>

namespace goop::sys::platform::vulkan
{
// Handle vulkan instance creation and device selection
class Context
{
  public:
	Context(const Context&) = delete;
	Context& operator=(const Context&) = delete;
	Context(const uint8_t maxFramesInFlight);
	~Context();

	operator VkDevice() const { return device; }
	operator VkInstance() const { return instance; }
	operator VkPhysicalDevice() const { return physicalDevice; }

	VkInstance getInstance() const { return instance; }
	VkDevice getDevice() const { return device; }
	VkPhysicalDevice getPhysicalDevice() const { return physicalDevice; }
	VkSurfaceKHR getSurface() const { return surface; }
	VkQueue getGraphicsQueue() const { return graphicsQueue; }
	VkQueue getPresentQueue() const { return presentQueue; }
	VkCommandPool getCommandPool() const { return commandPool; }
	const VkCommandBuffer* getCommandBuffer(uint8_t frame) const { return &commandBuffers[frame]; }

  private:
	const std::vector<const char*> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

#ifndef NDEBUG
	const std::vector<const char*> validationLayers = {"VK_LAYER_KHRONOS_validation"};
	bool checkValidationLayerSupport();
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	void createDebugMessenger();
	VkDebugUtilsMessengerEXT debugMessenger;
#endif

	std::vector<const char*> getRequiredExtensions();

	static VKAPI_ATTR VkBool32 VKAPI_CALL
	debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
				  VkDebugUtilsMessageTypeFlagsEXT messageType,
				  const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

	bool checkDeviceCompatibility(VkPhysicalDevice device);
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);

	void createInstance();
	void createSurface();
	void selectPhysicalDevice();
	void createLogicalDevice();

	void createCommandPool();
	void createCommandBuffers(uint8_t maxFramesInFlight);

	VkInstance instance = VK_NULL_HANDLE;
	VkSurfaceKHR surface = VK_NULL_HANDLE;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice device = VK_NULL_HANDLE;
	VkQueue graphicsQueue = VK_NULL_HANDLE;
	VkQueue presentQueue = VK_NULL_HANDLE;

	VkCommandPool commandPool;
	std::vector<VkCommandBuffer> commandBuffers;
};
} // namespace goop::sys::platform::vulkan
