// Sam Collier 2023
#pragma once

#include "Context.h"
#include <vector>
#include <vulkan/vulkan_core.h>
namespace goop::sys::platform::vulkan
{
class Swapchain
{
  public:
	Swapchain(const Swapchain&) = delete;
	Swapchain& operator=(const Swapchain&) = delete;
	Swapchain(Context* ctx, Swapchain* oldSwapchain = nullptr);
	~Swapchain();

	operator VkSwapchainKHR() const { return swapchain; }
	operator VkSwapchainKHR*() { return &swapchain; }

	VkFormat getFormat() const { return swapchainImageFormat; }
	VkExtent2D getExtent() const { return swapchainExtent; }
	VkRenderPass getRenderPass() const { return renderPass; }
	VkFramebuffer getFramebuffer(int index) const { return swapchainFramebuffers[index]; }

  private:
	void createSwapchain(Swapchain* oldSwapchain);
	VkSurfaceFormatKHR selectSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR selectPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D selectExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	void createImageViews();
	void createRenderPass();
	void createFramebuffers();

	void destroySwapchainDependents();

	VkSwapchainKHR swapchain;
	std::vector<VkImage> swapchainImages;
	VkFormat swapchainImageFormat;
	VkExtent2D swapchainExtent;
	std::vector<VkImageView> swapchainImageViews;
	std::vector<VkFramebuffer> swapchainFramebuffers;
	VkRenderPass renderPass;

	Context* ctx;
};
} // namespace goop::sys::platform::vulkan
