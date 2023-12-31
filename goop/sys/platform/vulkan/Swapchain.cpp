// Sam Collier 2023

#include "Swapchain.h"
#include "QueueFamilyIndices.h"
#include "SwapchainSupportInfo.h"
#include "Utils.h"
#include <algorithm>
#include <array>
#include <goop/sys/Window.h>
#include <limits>
#include <stdexcept>

using namespace goop::sys::platform::vulkan;

Swapchain::Swapchain(Context* ctx, Swapchain* oldSwapchain) : ctx(ctx)
{
#ifdef GOOP_APPTYPE_EDITOR
	if (oldSwapchain != nullptr)
	{
		uint32_t oldWidth = oldSwapchain->getViewportExtent().width;
		uint32_t oldHeight = oldSwapchain->getViewportExtent().height;
		if (oldWidth != 0 && oldHeight != 0)
		{
			viewportExtent.width = oldWidth;
			viewportExtent.height = oldHeight;
		}
	}
#endif

	createSwapchain(oldSwapchain);
	createImageViews();
	createRenderPass();
	createDepthResources();
	createFramebuffers();

#ifdef GOOP_APPTYPE_EDITOR
	createViewportImage();
	createViewportRenderPass();
	createViewportFramebuffer();
#endif
}

Swapchain::~Swapchain()
{
	destroySwapchainDependents();
#ifdef GOOP_APPTYPE_EDITOR
	destroyViewportDependents();
#endif
	vkDestroySwapchainKHR(*ctx, swapchain, nullptr);
	vkDestroyRenderPass(*ctx, renderPass, nullptr);
#ifdef GOOP_APPTYPE_EDITOR
	vkDestroyRenderPass(*ctx, viewportRenderPass, nullptr);
#endif
}

void Swapchain::createSwapchain(Swapchain* oldSwapchain)
{
	SwapchainSupportInfo supportInfo = getSwapchainSupportInfo(*ctx, ctx->getSurface());
	VkSurfaceFormatKHR surfaceFormat = selectSurfaceFormat(supportInfo.formats);
	VkPresentModeKHR presentMode = selectPresentMode(supportInfo.presentModes);
	VkExtent2D extent = selectExtent(supportInfo.capabilities);

	// it's recommended to request at least one more image than the minimum. that way we won't
	// have to worry about sometimes waiting on the drive to complete internal operations before
	// acquiring another image to render to
	uint32_t imageCount = supportInfo.capabilities.minImageCount + 1;

	if (supportInfo.capabilities.maxImageCount > 0 &&
		imageCount > supportInfo.capabilities.maxImageCount)
	{
		imageCount = supportInfo.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = ctx->getSurface();
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1; // always 1 unless we doing some kinda stereoscopic 3D
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	createInfo.preTransform = supportInfo.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE; // don't bother with pixels that are being obstructed by other
								  // windows (I think)

	if (oldSwapchain != nullptr)
	{
		createInfo.oldSwapchain = *oldSwapchain;
	}

	QueueFamilyIndices indices = findQueueFamilies(*ctx, ctx->getSurface());
	uint32_t queueFamilyIndices[] = {indices.graphics.value(), indices.present.value()};

	if (indices.graphics != indices.present)
	{
		// Concurrent mode is less performant than using EXCLUSIVE
		// but for now it's easier. TODO transition to EXCLUSIVE
		// need to specify the queue families that would be sharing ownership for CONCURRENT
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	if (vkCreateSwapchainKHR(*ctx, &createInfo, nullptr, &swapchain) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create swapchain!");
	}

	vkGetSwapchainImagesKHR(*ctx, swapchain, &imageCount, nullptr);
	swapchainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(*ctx, swapchain, &imageCount, swapchainImages.data());

	swapchainImageFormat = surfaceFormat.format;
	swapchainExtent = extent;

	delete oldSwapchain;
}

VkSurfaceFormatKHR
Swapchain::selectSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	for (const auto& availableFormat : availableFormats)
	{
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
			availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			return availableFormat;
		}
	}

	return availableFormats[0];
}

VkPresentModeKHR
Swapchain::selectPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
	for (const auto& availablePresentMode : availablePresentModes)
	{
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			return availablePresentMode;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D Swapchain::selectExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
	// if extent.width is ui32 max then we are taking up max space in the window
	// otherwise we must've specified a specific size
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		return capabilities.currentExtent;
	}
	else
	{
		int width, height;
		sys::gWindow->getFramebufferSize(&width, &height);

		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height),
		};

		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width,
										capabilities.maxImageExtent.width);

		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height,
										 capabilities.maxImageExtent.height);

		return actualExtent;
	}
}

void Swapchain::createImageViews()
{
	swapchainImageViews.resize(swapchainImages.size());

	for (size_t i = 0; i < swapchainImages.size(); i++)
	{
		swapchainImageViews[i] = createImageView(ctx, swapchainImages[i], swapchainImageFormat,
												 VK_IMAGE_ASPECT_COLOR_BIT);
	}
}

void Swapchain::createRenderPass()
{
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = swapchainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0; // layout(location = 0) out vec4 outColor; in frag shader
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

#ifndef GOOP_APPTYPE_EDITOR
	VkAttachmentDescription depthAttachment{};
	depthAttachment.format = findDepthFormat(ctx);
	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; // after drawing is finished we
																// don't care about the depth data
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentRef{};
	depthAttachmentRef.attachment = 1; // layout(location = 1) out vec4 outColor; in frag shader
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
#endif

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
#ifndef GOOP_APPTYPE_EDITOR
	subpass.pDepthStencilAttachment = &depthAttachmentRef;
#endif

	// We must wait for the swapchain to finish reading from the image before we can access it
	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
#ifndef GOOP_APPTYPE_EDITOR
	dependency.srcStageMask =
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.dstStageMask =
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstAccessMask =
		VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
#else
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
#endif

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
#ifdef GOOP_APPTYPE_EDITOR
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
#else
	std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments = attachments.data();
#endif
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	if (vkCreateRenderPass(*ctx, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create render pass!");
	}
}

void Swapchain::createFramebuffers()
{
	swapchainFramebuffers.resize(swapchainImageViews.size());
	for (size_t i = 0; i < swapchainImageViews.size(); i++)
	{
#ifndef GOOP_APPTYPE_EDITOR
		std::array<VkImageView, 2> attachments = {swapchainImageViews[i], depthImageView};
#else
		std::array<VkImageView, 1> attachments = {swapchainImageViews[i]};
#endif
		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = swapchainExtent.width;
		framebufferInfo.height = swapchainExtent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(*ctx, &framebufferInfo, nullptr, &swapchainFramebuffers[i]) !=
			VK_SUCCESS)
		{
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
}

void Swapchain::destroySwapchainDependents()
{
#ifndef GOOP_APPTYPE_EDITOR
	vkDestroyImageView(*ctx, depthImageView, nullptr);
	vkDestroyImage(*ctx, depthImage, nullptr);
	vkFreeMemory(*ctx, depthImageMemory, nullptr);
#endif

	for (auto framebuffer : swapchainFramebuffers)
	{
		vkDestroyFramebuffer(*ctx, framebuffer, nullptr);
	}
	for (auto imageView : swapchainImageViews)
	{
		vkDestroyImageView(*ctx, imageView, nullptr);
	}
}

void Swapchain::createDepthResources()
{
	VkFormat depthFormat = findDepthFormat(ctx);

#ifdef GOOP_APPTYPE_EDITOR
	createImage(ctx, viewportExtent.width, viewportExtent.height, depthFormat,
				VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage, depthImageMemory);
#else
	createImage(ctx, swapchainExtent.width, swapchainExtent.height, depthFormat,
				VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage, depthImageMemory);
#endif

	depthImageView = createImageView(ctx, depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
}

#ifdef GOOP_APPTYPE_EDITOR
void Swapchain::createViewportRenderPass()
{
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = swapchainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0; // layout(location = 0) out vec4 outColor; in frag shader
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentDescription depthAttachment{};
	depthAttachment.format = findDepthFormat(ctx);
	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; // after drawing is finished we
																// don't care about the depth data
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentRef{};
	depthAttachmentRef.attachment = 1; // layout(location = 1) out vec4 outColor; in frag shader
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.pDepthStencilAttachment = &depthAttachmentRef;

	std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};

	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask =
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask =
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.dstAccessMask =
		VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	if (vkCreateRenderPass(*ctx, &renderPassInfo, nullptr, &viewportRenderPass) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create offscreen render pass!");
	}
}

void Swapchain::createViewportImage()
{
	createImage(ctx, viewportExtent.width, viewportExtent.height, swapchainImageFormat,
				VK_IMAGE_TILING_OPTIMAL,
				VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, viewportImage, viewportImageMemory);
	viewportImageView =
		createImageView(ctx, viewportImage, swapchainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
}

void Swapchain::createViewportFramebuffer()
{
	std::array<VkImageView, 2> attachments = {viewportImageView, depthImageView};

	VkFramebufferCreateInfo framebufferInfo{};
	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.renderPass = viewportRenderPass;
	framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	framebufferInfo.pAttachments = attachments.data();
	framebufferInfo.width = viewportExtent.width;
	framebufferInfo.height = viewportExtent.height;
	framebufferInfo.layers = 1;

	if (vkCreateFramebuffer(*ctx, &framebufferInfo, nullptr, &viewportFramebuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create viewport framebuffer!");
	}
}

void Swapchain::destroyViewportDependents()
{
	vkDestroyImageView(*ctx, viewportImageView, nullptr);
	vkDestroyImage(*ctx, viewportImage, nullptr);
	vkFreeMemory(*ctx, viewportImageMemory, nullptr);
	vkDestroyFramebuffer(*ctx, viewportFramebuffer, nullptr);
	vkDestroyImageView(*ctx, depthImageView, nullptr);
	vkDestroyImage(*ctx, depthImage, nullptr);
	vkFreeMemory(*ctx, depthImageMemory, nullptr);
}

void Swapchain::recreateViewport(float width, float height)
{
	destroyViewportDependents();
	viewportExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
	createDepthResources();
	createViewportImage();
	createViewportFramebuffer();
}
#endif
