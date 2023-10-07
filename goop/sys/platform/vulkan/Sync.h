// Sam Collier 2023
#pragma once

#include <vector>
#include <vulkan/vulkan.h>

namespace goop::sys::platform::vulkan
{
class Context;
class Sync
{
  public:
	Sync(const Sync&) = delete;
	Sync& operator=(const Sync&) = delete;
	Sync(Context* ctx, uint8_t maxFramesInFlight);
	~Sync();

	inline VkSemaphore& getImageAvailableSemaphore(uint8_t index)
	{
		return imageAvailableSemaphores[index];
	}

	inline VkSemaphore& getRenderFinishedSemaphore(uint8_t index)
	{
		return renderFinishedSemaphores[index];
	}

	inline VkFence& getInFlightFence(uint8_t index)
	{
		return inFlightFences[index];
	}

	void waitForFrame(uint8_t index);
	void resetFrameFence(uint8_t index);

  private:
	void createSyncObjects();

	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;

	Context* ctx;
	uint8_t maxFramesInFlight;
};
} // namespace goop::sys::platform::vulkan
