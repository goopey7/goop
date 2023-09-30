// Sam Collier 2023

#include "Renderer_Vulkan.h"
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <set>
#include <stdexcept>
#include <vector>
#include "BufferHelpers.h"

#ifdef RENDERER_VULKAN
goop::sys::platform::vulkan::Renderer_Vulkan gRendererVulkan;
goop::sys::Renderer* goop::sys::gRenderer = &gRendererVulkan;
#endif

using namespace goop::sys::platform::vulkan;

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

const uint8_t MAX_FRAMES_IN_FLIGHT = 2;

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

int Renderer_Vulkan::initialize()
{
	ctx = new Context(enableValidationLayers);
	createSwapchain();
	createImageViews();
	createRenderPass();
	uniformBuffer = new UniformBuffer(ctx, MAX_FRAMES_IN_FLIGHT);
	descriptor = new Descriptor(ctx, MAX_FRAMES_IN_FLIGHT, uniformBuffer);
	pipeline = new Pipeline(ctx, swapchainExtent, renderPass, descriptor);
	createFramebuffers();
	createCommandPool();
	createVertexBuffer();
	createIndexBuffer();
	createCommandBuffers();
	createSyncObjects();
	return 0;
}

int Renderer_Vulkan::destroy()
{
	vkDeviceWaitIdle(*ctx);
	destroySwapchainDependents();
	vkDestroySwapchainKHR(*ctx, swapchain, nullptr);
	delete descriptor;
	delete uniformBuffer;
	vkDestroyBuffer(*ctx, indexBuffer, nullptr);
	vkFreeMemory(*ctx, indexBufferMemory, nullptr);
	vkDestroyBuffer(*ctx, vertexBuffer, nullptr);
	vkFreeMemory(*ctx, vertexBufferMemory, nullptr);
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		vkDestroyFence(*ctx, inFlightFences[i], nullptr);
		vkDestroySemaphore(*ctx, renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(*ctx, imageAvailableSemaphores[i], nullptr);
	}
	delete pipeline;
	vkDestroyCommandPool(*ctx, commandPool, nullptr);
	vkDestroyRenderPass(*ctx, renderPass, nullptr);
	delete ctx;
	return 0;
}

void Renderer_Vulkan::render()
{
	// wait for the fence to signal that the frame is finished
	vkWaitForFences(*ctx, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

	// Acquire image from swapchain
	uint32_t imageIndex;
	VkResult result =
		vkAcquireNextImageKHR(*ctx, swapchain, UINT64_MAX, imageAvailableSemaphores[currentFrame],
							  VK_NULL_HANDLE, &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		recreateSwapchain();
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("failed to acquire swapchain image!");
	}

	// Only reset the fence if we're definitely submitting work to gpu
	// which is at this point
	vkResetFences(*ctx, 1, &inFlightFences[currentFrame]);

	updateUniformBuffer(currentFrame);

	// Submit command buffer
	vkResetCommandBuffer(commandBuffers[currentFrame], 0);
	recordCommandBuffer(commandBuffers[currentFrame], imageIndex);
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &imageAvailableSemaphores[currentFrame];
	VkPipelineStageFlags waitStages[] = {
		// wait at the stage of the pipeline where we draw color
		// this means theoretically we could start drawing geometry while we wait for the image
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
	};
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffers[currentFrame];
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &renderFinishedSemaphores[currentFrame];
	if (vkQueueSubmit(ctx->getGraphicsQueue(), 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	// Presentation
	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &renderFinishedSemaphores[currentFrame];
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &swapchain;
	presentInfo.pImageIndices = &imageIndex;
	result = vkQueuePresentKHR(ctx->getPresentQueue(), &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || frameBufferResized)
	{
		frameBufferResized = false;
		recreateSwapchain();
	}
	else if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to present swapchain image!");
	}

	// advance frame (modulo will wrap around)
	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Renderer_Vulkan::createSwapchain(VkSwapchainKHR oldSwapchain)
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
	createInfo.oldSwapchain = oldSwapchain;

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

	destroySwapchainDependents();
	vkDestroySwapchainKHR(*ctx, oldSwapchain, nullptr);
}

void Renderer_Vulkan::createImageViews()
{
	swapchainImageViews.resize(swapchainImages.size());

	for (size_t i = 0; i < swapchainImages.size(); i++)
	{
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = swapchainImages[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = swapchainImageFormat;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(*ctx, &createInfo, nullptr, &swapchainImageViews[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create image views!");
		}
	}
}

void Renderer_Vulkan::createRenderPass()
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

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	// We must wait for the swapchain to finish reading from the image before we can access it
	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	if (vkCreateRenderPass(*ctx, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create render pass!");
	}
}

void Renderer_Vulkan::createFramebuffers()
{
	swapchainFramebuffers.resize(swapchainImageViews.size());
	for (size_t i = 0; i < swapchainImageViews.size(); i++)
	{
		VkImageView attachments[] = {swapchainImageViews[i]};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
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

void Renderer_Vulkan::createCommandPool()
{
	QueueFamilyIndices queueFamilyIndices = findQueueFamilies(*ctx, ctx->getSurface());

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphics.value();

	if (vkCreateCommandPool(*ctx, &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create command pool!");
	}
}

void Renderer_Vulkan::createVertexBuffer()
{
	VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(ctx, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				 stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(*ctx, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, vertices.data(), (size_t)bufferSize);
	vkUnmapMemory(*ctx, stagingBufferMemory);

	createBuffer(ctx, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);

	copyBuffer(ctx, commandPool, ctx->getGraphicsQueue(), stagingBuffer, vertexBuffer, bufferSize);

	vkDestroyBuffer(*ctx, stagingBuffer, nullptr);
	vkFreeMemory(*ctx, stagingBufferMemory, nullptr);
}

void Renderer_Vulkan::createIndexBuffer()
{
	VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(ctx, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				 stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(*ctx, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, indices.data(), (size_t)bufferSize);
	vkUnmapMemory(*ctx, stagingBufferMemory);

	createBuffer(ctx, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
				 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

	copyBuffer(ctx, commandPool, ctx->getGraphicsQueue(), stagingBuffer, indexBuffer, bufferSize);

	vkDestroyBuffer(*ctx, stagingBuffer, nullptr);
	vkFreeMemory(*ctx, stagingBufferMemory, nullptr);
}

void Renderer_Vulkan::createCommandBuffers()
{
	commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

	if (vkAllocateCommandBuffers(*ctx, &allocInfo, commandBuffers.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate command buffers!");
	}
}

void Renderer_Vulkan::createSyncObjects()
{
	imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

	// start the fence signalled so that we don't wait on it indefinately on the first frame
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		if (vkCreateSemaphore(*ctx, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) !=
				VK_SUCCESS ||
			vkCreateSemaphore(*ctx, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) !=
				VK_SUCCESS ||
			vkCreateFence(*ctx, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create sync objects!");
		}
	}
}

void Renderer_Vulkan::destroySwapchainDependents()
{
	for (auto framebuffer : swapchainFramebuffers)
	{
		vkDestroyFramebuffer(*ctx, framebuffer, nullptr);
	}
	for (auto imageView : swapchainImageViews)
	{
		vkDestroyImageView(*ctx, imageView, nullptr);
	}
}

VkSurfaceFormatKHR
Renderer_Vulkan::selectSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
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
Renderer_Vulkan::selectPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
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

VkExtent2D Renderer_Vulkan::selectExtent(const VkSurfaceCapabilitiesKHR& capabilities)
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

void Renderer_Vulkan::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0;
	beginInfo.pInheritanceInfo = nullptr;

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to begin recording command buffer!");
	}

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = renderPass;
	renderPassInfo.framebuffer = swapchainFramebuffers[imageIndex];
	renderPassInfo.renderArea.offset = {0, 0};
	renderPassInfo.renderArea.extent = swapchainExtent;
	VkClearValue clearColor = {{{0.f, 0.f, 0.f, 1.f}}};
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->getPipeline());

	VkDeviceSize offset = 0;
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer, &offset);
	vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);

	VkViewport viewport{};
	viewport.x = 0.f;
	viewport.y = 0.f;
	viewport.width = static_cast<float>(swapchainExtent.width);
	viewport.height = static_cast<float>(swapchainExtent.height);
	viewport.minDepth = 0.f;
	viewport.maxDepth = 1.f;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = {0, 0};
	scissor.extent = swapchainExtent;
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
							pipeline->getPipelineLayout(), 0, 1, descriptor->getSet(currentFrame),
							0, nullptr);

	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

	vkCmdEndRenderPass(commandBuffer);

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to record command buffer!");
	}
}

void Renderer_Vulkan::updateUniformBuffer(uint32_t currentFrame)
{
	static auto startTime = std::chrono::high_resolution_clock::now();
	auto currentTime = std::chrono::high_resolution_clock::now();

	float time =
		std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	UniformBufferObject ubo{};

	ubo.model = glm::rotate(glm::mat4(1.f), time + glm::radians(90.f), glm::vec3(0.f, 0.f, 1.f));

	ubo.view =
		glm::lookAt(glm::vec3(2.f, 2.f, 2.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 1.f));

	ubo.proj = glm::perspective(glm::radians(45.f),
								swapchainExtent.width / (float)swapchainExtent.height, 0.1f, 10.f);

	// flip y coordinate, glm was designed for OpenGL which has an inverted y coordinate
	ubo.proj[1][1] *= -1;

	uniformBuffer->update(currentFrame, ubo);
}

void Renderer_Vulkan::recreateSwapchain()
{
	int width = 0, height = 0;
	sys::gWindow->getFramebufferSize(&width, &height);
	// wait until the window is actually visible
	// because it has been minimized or something
	while (width == 0 || height == 0)
	{
		sys::gWindow->getFramebufferSize(&width, &height);
		sys::gWindow->waitEvents();
	}

	vkDeviceWaitIdle(*ctx);

	createSwapchain(swapchain);
	createImageViews();
	createFramebuffers();
}
