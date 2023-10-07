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
	ctx = new Context(enableValidationLayers, MAX_FRAMES_IN_FLIGHT);
	swapchain = new Swapchain(ctx);
	uniformBuffer = new UniformBuffer(ctx, MAX_FRAMES_IN_FLIGHT);
	descriptor = new Descriptor(ctx, MAX_FRAMES_IN_FLIGHT, uniformBuffer);
	pipeline = new Pipeline(ctx, swapchain, descriptor);
	buffers = new Buffers(ctx);
	createSyncObjects();
	return 0;
}

int Renderer_Vulkan::destroy()
{
	vkDeviceWaitIdle(*ctx);
	delete buffers;
	delete swapchain;
	delete descriptor;
	delete uniformBuffer;
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		vkDestroyFence(*ctx, inFlightFences[i], nullptr);
		vkDestroySemaphore(*ctx, renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(*ctx, imageAvailableSemaphores[i], nullptr);
	}
	delete pipeline;
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
		vkAcquireNextImageKHR(*ctx, *swapchain, UINT64_MAX, imageAvailableSemaphores[currentFrame],
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
	vkResetCommandBuffer(*ctx->getCommandBuffer(currentFrame), 0);
	recordCommandBuffer(*ctx->getCommandBuffer(currentFrame), imageIndex);
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
	submitInfo.pCommandBuffers = ctx->getCommandBuffer(currentFrame);
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
	presentInfo.pSwapchains = *swapchain;
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
	renderPassInfo.renderPass = swapchain->getRenderPass();
	renderPassInfo.framebuffer = swapchain->getFramebuffer(imageIndex);
	renderPassInfo.renderArea.offset = {0, 0};
	renderPassInfo.renderArea.extent = swapchain->getExtent();
	VkClearValue clearColor = {{{0.f, 0.f, 0.f, 1.f}}};
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->getPipeline());

	VkDeviceSize offset = 0;
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers->getVertexBuffer(), &offset);
	vkCmdBindIndexBuffer(commandBuffer, buffers->getIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);

	VkViewport viewport{};
	viewport.x = 0.f;
	viewport.y = 0.f;
	viewport.width = static_cast<float>(swapchain->getExtent().width);
	viewport.height = static_cast<float>(swapchain->getExtent().height);
	viewport.minDepth = 0.f;
	viewport.maxDepth = 1.f;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = {0, 0};
	scissor.extent = swapchain->getExtent();
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
								swapchain->getExtent().width / (float)swapchain->getExtent().height, 0.1f, 10.f);

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

	// the new swapchain will handle destroying the old one
	swapchain = new Swapchain(ctx, swapchain);
}
