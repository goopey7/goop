// Sam Collier 2023

#include "Renderer_Vulkan.h"
#include "Utils.h"
#include "goop/sys/MeshLoader.h"
#include "goop/sys/platform/vulkan/QueueFamilyIndices.h"
#include "goop/sys/platform/vulkan/SwapchainSupportInfo.h"
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <imgui.h>
#include <imgui_impl_vulkan.h>

#define GLM_FORCE_RADIANS
// Vulkan uses a range of 0 to 1 for depth, not -1 to 1 like OpenGL
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <set>
#include <stdexcept>
#include <vector>

#include <volk.h>

#ifdef GOOP_RENDERER_VULKAN
const std::unique_ptr<goop::sys::Renderer> goop::sys::gRenderer =
	std::make_unique<goop::sys::platform::vulkan::Renderer_Vulkan>();
#endif

using namespace goop::sys::platform::vulkan;

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

void Renderer_Vulkan::initImGui()
{
	VkDescriptorPoolSize pool_sizes[] = {{VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
										 {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
										 {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
										 {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
										 {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
										 {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
										 {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
										 {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
										 {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
										 {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
										 {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}};

	VkDescriptorPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	pool_info.maxSets = 1000;
	pool_info.poolSizeCount = std::size(pool_sizes);
	pool_info.pPoolSizes = pool_sizes;
	if (vkCreateDescriptorPool(*ctx, &pool_info, nullptr, &imguiPool) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create imgui descriptor pool!");
	}

	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.DescriptorPool = imguiPool;
	SwapchainSupportInfo swapchainSupport =
		getSwapchainSupportInfo(ctx->getPhysicalDevice(), ctx->getSurface());
	init_info.MinImageCount = swapchainSupport.capabilities.minImageCount;
	init_info.ImageCount = init_info.MinImageCount;
	init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

	auto funcLoader = [](const char* funcName, void* ctx_ptr)
	{
		Context* ctx = reinterpret_cast<Context*>(ctx_ptr);
		PFN_vkVoidFunction instanceAddr = vkGetInstanceProcAddr(ctx->getInstance(), funcName);
		PFN_vkVoidFunction deviceAddr = vkGetDeviceProcAddr(ctx->getDevice(), funcName);
		return deviceAddr ? deviceAddr : instanceAddr;
	};
	const bool funcsLoaded = ImGui_ImplVulkan_LoadFunctions(funcLoader, ctx);

	ImGui_ImplVulkan_Init(ctx, &init_info, swapchain->getRenderPass());

	// Upload the fonts for DearImgui
	VkCommandBuffer cb = beginSingleTimeCommands(ctx);
	ImGui_ImplVulkan_CreateFontsTexture(cb);
	endSingleTimeCommands(ctx, cb);
	ImGui_ImplVulkan_DestroyFontUploadObjects();

#ifdef GOOP_APPTYPE_EDITOR
	imgSet = ImGui_ImplVulkan_AddTexture(texture->getSampler(), texture->getImageView(),
										 VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
#endif
}

int Renderer_Vulkan::initialize()
{
	ctx = new Context();
	swapchain = new Swapchain(ctx);
	uniformBuffer = new UniformBuffer(ctx);
	texture = new Texture(ctx, "res/viking_room.png");
	descriptor = new Descriptor(ctx, uniformBuffer, texture);
	pipeline = new Pipeline(ctx, swapchain, descriptor);
	buffers = new Buffers(ctx);
	sync = new Sync(ctx);
	bIsInitialized = true;
	initImGui();
	return 0;
}

int Renderer_Vulkan::destroy()
{
	vkDeviceWaitIdle(*ctx);
	ImGui_ImplVulkan_Shutdown();
	vkDestroyDescriptorPool(*ctx, imguiPool, nullptr);
	delete texture;
	delete buffers;
	delete swapchain;
	delete descriptor;
	delete uniformBuffer;
	delete sync;
	delete pipeline;
	delete ctx;
	bIsInitialized = false;
	return 0;
}

void Renderer_Vulkan::beginFrame() { ImGui_ImplVulkan_NewFrame(); }

void Renderer_Vulkan::updateUniformBuffer(uint32_t currentFrame)
{
	static auto startTime = std::chrono::high_resolution_clock::now();
	auto currentTime = std::chrono::high_resolution_clock::now();

	float time =
		std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	UniformBufferObject ubo{};

	ubo.model = glm::rotate(glm::mat4(1.f), time + glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));

	ubo.view =
		glm::lookAt(glm::vec3(2.f, 2.f, 2.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));

#ifdef GOOP_APPTYPE_EDITOR
	ubo.proj = glm::perspective(glm::radians(45.f),
								swapchain->getViewportExtent().width /
									(float)swapchain->getViewportExtent().height,
								0.1f, 10.f);
#else
	ubo.proj = glm::perspective(glm::radians(45.f),
								swapchain->getExtent().width / (float)swapchain->getExtent().height,
								0.1f, 10.f);
#endif

	// flip y coordinate, glm was designed for OpenGL which has an inverted y coordinate
	ubo.proj[1][1] *= -1;

	uniformBuffer->update(currentFrame, ubo);
}

void Renderer_Vulkan::addToRenderQueue(goop::res::mesh mesh, MeshLoader* meshLoader)
{
	Renderer::addToRenderQueue(mesh, meshLoader);
}

void Renderer_Vulkan::updateBuffers()
{
	if (meshLoader != nullptr)
	{
		oldQueue = meshQueue;
		MeshImportData combinedMesh;
		while (!meshQueue.empty())
		{
			goop::res::mesh id = meshQueue.front();
			MeshImportData mesh = (*meshLoader->getData())[id];

			for (uint32_t i = 0; i < mesh.indices.size(); i++)
			{
				mesh.indices[i] += combinedMesh.vertices.size();
			}

			combinedMesh.vertices.insert(combinedMesh.vertices.end(), mesh.vertices.begin(),
										 mesh.vertices.end());
			combinedMesh.indices.insert(combinedMesh.indices.end(), mesh.indices.begin(),
										mesh.indices.end());
			meshQueue.pop();
		}
		buffers->updateBuffers(currentFrame, combinedMesh.vertices.data(),
							   combinedMesh.vertices.size(), combinedMesh.indices.data(),
							   combinedMesh.indices.size());
	}
	for (size_t i = 0; i < meshQueue.size(); i++)
	{
		meshQueue.pop();
	}
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

#ifndef GOOP_APPTYPE_EDITOR
void Renderer_Vulkan::render()
{
	vkDeviceWaitIdle(*ctx);
	buffers->swapBuffers(currentFrame);

	// wait for the fence to signal that the frame is finished
	sync->waitForFrame(currentFrame);

	// Acquire image from swapchain
	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(*ctx, *swapchain, UINT64_MAX,
											sync->getImageAvailableSemaphore(currentFrame),
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
	sync->resetFrameFence(currentFrame);

	updateUniformBuffer(currentFrame);

	// Submit command buffer
	vkResetCommandBuffer(*ctx->getCommandBuffer(currentFrame), 0);
	recordCommandBuffer(*ctx->getCommandBuffer(currentFrame), imageIndex);
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &sync->getImageAvailableSemaphore(currentFrame);
	VkPipelineStageFlags waitStages[] = {
		// wait at the stage of the pipeline where we draw color
		// this means theoretically we could start drawing geometry while we wait for the image
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
	};
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = ctx->getCommandBuffer(currentFrame);
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &sync->getRenderFinishedSemaphore(currentFrame);
	if (vkQueueSubmit(ctx->getGraphicsQueue(), 1, &submitInfo,
					  sync->getInFlightFence(currentFrame)) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	// Presentation
	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &sync->getRenderFinishedSemaphore(currentFrame);
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
	currentFrame = (currentFrame + 1) % ctx->getMaxFramesInFlight();
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

	// order of clear values must match order of attachments in render pass
	std::array<VkClearValue, 2> clearValues{};
	clearValues[0].color = {{0.f, 0.f, 0.f, 1.f}};
	clearValues[1].depthStencil = {1.f, 0};

	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->getPipeline());

	if (buffers->getVertexCount(currentFrame) && buffers->getIndexCount(currentFrame) != 0 &&
		buffers->getVertexBuffer(currentFrame) != VK_NULL_HANDLE &&
		buffers->getIndexBuffer(currentFrame) != VK_NULL_HANDLE)
	{
		VkDeviceSize offset = 0;
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers->getVertexBuffer(currentFrame),
							   &offset);
		vkCmdBindIndexBuffer(commandBuffer, buffers->getIndexBuffer(currentFrame), 0,
							 VK_INDEX_TYPE_UINT32);
	}

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

	if (buffers->getIndexCount(currentFrame) != 0 &&
		buffers->getIndexBuffer(currentFrame) != nullptr)
	{
		vkCmdDrawIndexed(commandBuffer, buffers->getIndexCount(currentFrame), 1, 0, 0, 0);
	}

	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);

	vkCmdEndRenderPass(commandBuffer);

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to record command buffer!");
	}
}

void Renderer_Vulkan::endFrame() { updateBuffers(); }

#endif

#ifdef GOOP_APPTYPE_EDITOR
bool Renderer_Vulkan::renderScene(uint32_t width, uint32_t height, uint32_t imageIndex)
{
	vkDeviceWaitIdle(*ctx);
	buffers->swapBuffers(currentFrame);

	VkCommandBuffer cb = beginSingleTimeCommands(ctx);
	VkViewport viewport{};
	viewport.x = 0.f;
	viewport.y = 0.f;
	viewport.width = static_cast<float>(width);
	viewport.height = static_cast<float>(height);
	viewport.minDepth = 0.f;
	viewport.maxDepth = 1.f;
	vkCmdSetViewport(cb, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = {0, 0};
	scissor.extent = swapchain->getViewportExtent();
	vkCmdSetScissor(cb, 0, 1, &scissor);

	std::array<VkClearValue, 2> clearValues{};
	clearValues[0].color = {{0.f, 0.f, 0.f, 1.f}};
	clearValues[1].depthStencil = {1.f, 0};

	VkRenderPassBeginInfo beginRenderPassInfo{};
	beginRenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	beginRenderPassInfo.renderPass = swapchain->getViewportRenderPass();
	beginRenderPassInfo.framebuffer = swapchain->getViewportFramebuffer();
	beginRenderPassInfo.renderArea.offset = {0, 0};
	beginRenderPassInfo.renderArea.extent = swapchain->getViewportExtent();
	beginRenderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	beginRenderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(cb, &beginRenderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->getPipeline());

	if (buffers->getVertexCount(currentFrame) && buffers->getIndexCount(currentFrame) != 0 &&
		buffers->getVertexBuffer(currentFrame) != VK_NULL_HANDLE &&
		buffers->getIndexBuffer(currentFrame) != VK_NULL_HANDLE)
	{
		VkDeviceSize offset = 0;
		vkCmdBindVertexBuffers(cb, 0, 1, buffers->getVertexBuffer(currentFrame), &offset);
		vkCmdBindIndexBuffer(cb, buffers->getIndexBuffer(currentFrame), 0, VK_INDEX_TYPE_UINT32);
	}

	vkCmdBindDescriptorSets(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->getPipelineLayout(), 0,
							1, descriptor->getSet(currentFrame), 0, nullptr);

	if (buffers->getIndexCount(currentFrame) != 0)
	{
		vkCmdDrawIndexed(cb, buffers->getIndexCount(currentFrame), 1, 0, 0, 0);
	}

	vkCmdEndRenderPass(cb);

	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = swapchain->getViewportImage();
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;
	barrier.srcAccessMask = 0;
	barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

	vkCmdPipelineBarrier(cb, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
						 VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1,
						 &barrier);

	endSingleTimeCommands(ctx, cb);

	vkDeviceWaitIdle(*ctx);

	return true;
}

void Renderer_Vulkan::renderFrame(uint32_t imageIndex)
{
	// Submit command buffer
	vkResetCommandBuffer(*ctx->getCommandBuffer(currentFrame), 0);
	recordCommandBuffer(*ctx->getCommandBuffer(currentFrame), imageIndex);
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &sync->getImageAvailableSemaphore(currentFrame);
	VkPipelineStageFlags waitStages[] = {
		// wait at the stage of the pipeline where we draw color
		// this means theoretically we could start drawing geometry while we wait for the image
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
	};
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = ctx->getCommandBuffer(currentFrame);
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &sync->getRenderFinishedSemaphore(currentFrame);
	if (vkQueueSubmit(ctx->getGraphicsQueue(), 1, &submitInfo,
					  sync->getInFlightFence(currentFrame)) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	// Presentation
	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &sync->getRenderFinishedSemaphore(currentFrame);
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = *swapchain;
	presentInfo.pImageIndices = &imageIndex;
	VkResult result = vkQueuePresentKHR(ctx->getPresentQueue(), &presentInfo);

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
	currentFrame = (currentFrame + 1) % ctx->getMaxFramesInFlight();
	updateBuffers();
}

void Renderer_Vulkan::render(float width, float height)
{
	// wait for the fence to signal that the frame is finished
	sync->waitForFrame(currentFrame);

	// Acquire image from swapchain
	VkResult result = vkAcquireNextImageKHR(*ctx, *swapchain, UINT64_MAX,
											sync->getImageAvailableSemaphore(currentFrame),
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
	sync->resetFrameFence(currentFrame);

	if (width <= 0.f || height <= 0.f)
	{
		width = swapchain->getExtent().width;
		height = swapchain->getExtent().height;
	}

	if (width != oldWidth || height != oldHeight)
	{
		oldWidth = width;
		oldHeight = height;
		vkDeviceWaitIdle(*ctx);
		swapchain->recreateViewport(width, height);
	}

	if (renderScene(width, height, imageIndex))
	{
		ImGui_ImplVulkan_RemoveTexture(imgSet);
		imgSet =
			ImGui_ImplVulkan_AddTexture(texture->getSampler(), swapchain->getViewportImageView(),
										VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}

	updateUniformBuffer(currentFrame);
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

	// order of clear values must match order of attachments in render pass
	std::array<VkClearValue, 2> clearValues{};
	clearValues[0].color = {{0.f, 0.f, 0.f, 1.f}};
	clearValues[1].depthStencil = {1.f, 0};

	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
	vkCmdEndRenderPass(commandBuffer);

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to record command buffer!");
	}
}

void Renderer_Vulkan::endFrame() { renderFrame(imageIndex); }
#endif
