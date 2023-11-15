// Sam Collier 2023

#include "Sync.h"
#include "Context.h"
#include <stdexcept>

using namespace goop::sys::platform::vulkan;

Sync::Sync(Context* ctx)
	: ctx(ctx), maxFramesInFlight(ctx->getMaxFramesInFlight())
{
	createSyncObjects();
}

Sync::~Sync()
{
	for (size_t i = 0; i < maxFramesInFlight; i++)
	{
		vkDestroyFence(*ctx, inFlightFences[i], nullptr);
		vkDestroySemaphore(*ctx, renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(*ctx, imageAvailableSemaphores[i], nullptr);
	}
}

void Sync::createSyncObjects()
{
	imageAvailableSemaphores.resize(maxFramesInFlight);
	renderFinishedSemaphores.resize(maxFramesInFlight);
	inFlightFences.resize(maxFramesInFlight);

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

	// start the fence signalled so that we don't wait on it indefinately on the first frame
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < maxFramesInFlight; i++)
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

void Sync::waitForFrame(uint8_t index)
{
	vkWaitForFences(*ctx, 1, &inFlightFences[index], VK_TRUE, UINT64_MAX);
}

void Sync::resetFrameFence(uint8_t index)
{
	vkResetFences(*ctx, 1, &inFlightFences[index]);
}

