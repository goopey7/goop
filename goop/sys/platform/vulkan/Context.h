// Sam Collier 2023
#pragma once

#include <vector>
#include <volk.h>

namespace goop::sys::platform::vulkan
{
// Handle vulkan instance creation and device selection
class Context
{
  public:
	Context(const Context&) = delete;
	Context& operator=(const Context&) = delete;
	Context(bool enableValidationLayers, const uint8_t maxFramesInFlight);
	~Context();

	operator VkDevice() const { return device; }
	operator VkInstance() const { return instance; }
	operator VkPhysicalDevice() const { return physicalDevice; }

	VkDevice getDevice() const { return device; }
	VkPhysicalDevice getPhysicalDevice() const { return physicalDevice; }
	VkSurfaceKHR getSurface() const { return surface; }
	VkQueue getGraphicsQueue() const { return graphicsQueue; }
	VkQueue getPresentQueue() const { return presentQueue; }
	VkCommandPool getCommandPool() const { return commandPool; }
	const VkCommandBuffer* getCommandBuffer(uint8_t frame) const { return &commandBuffers[frame]; }

  private:
	const std::vector<const char*> validationLayers = {"VK_LAYER_KHRONOS_validation"};
	const std::vector<const char*> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
	bool checkValidationLayerSupport();
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	std::vector<const char*> getRequiredExtensions(bool enableValidationLayers);

	static VKAPI_ATTR VkBool32 VKAPI_CALL
	debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
				  VkDebugUtilsMessageTypeFlagsEXT messageType,
				  const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

	bool checkDeviceCompatibility(VkPhysicalDevice device);
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);

	void createInstance(bool enableValidationLayers);
	void createDebugMessenger(bool enableValidationLayers);
	void createSurface();
	void selectPhysicalDevice();
	void createLogicalDevice(bool enableValidationLayers);

	void createCommandPool();
	void createCommandBuffers(uint8_t maxFramesInFlight);

	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;
	VkSurfaceKHR surface;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice device;
	VkQueue graphicsQueue;
	VkQueue presentQueue;

	VkCommandPool commandPool;
	std::vector<VkCommandBuffer> commandBuffers;
};
} // namespace goop::sys::platform::vulkan
