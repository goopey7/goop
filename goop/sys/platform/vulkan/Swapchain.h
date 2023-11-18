// Sam Collier 2023
#pragma once

#include "Context.h"
#include <vector>
#include <volk.h>
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
	VkRenderPass getViewportRenderPass() const { return viewportRenderPass; }
	VkExtent2D getViewportExtent() const { return viewportExtent; }
	VkFramebuffer getViewportFramebuffer() const { return viewportFramebuffer; }
	VkImageView getViewportImageView() const { return viewportImageView; }
	VkImage getViewportImage() const { return viewportImage; }
	void recreateViewport(float width, float height);

  private:
	void createSwapchain(Swapchain* oldSwapchain);
	VkSurfaceFormatKHR selectSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR selectPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D selectExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	void createImageViews();
	void createDepthResources();
	void createRenderPass();
	void createFramebuffers();
	void createViewportRenderPass();
	void createViewportImage();
	void createViewportFramebuffer();

	void destroySwapchainDependents();
	void destroyViewportDependents();


	VkSwapchainKHR swapchain;
	std::vector<VkImage> swapchainImages;
	VkFormat swapchainImageFormat;
	VkExtent2D swapchainExtent;
	std::vector<VkImageView> swapchainImageViews;
	std::vector<VkFramebuffer> swapchainFramebuffers;
	VkRenderPass renderPass;

	VkImage depthImage;
	VkDeviceMemory depthImageMemory;
	VkImageView depthImageView;

	VkExtent2D viewportExtent = { 1280, 720 };
	VkRenderPass viewportRenderPass;
	VkImageView viewportImageView;
	VkImage viewportImage;
	VkDeviceMemory viewportImageMemory;
	VkFramebuffer viewportFramebuffer;

	Context* ctx;
};
} // namespace goop::sys::platform::vulkan
