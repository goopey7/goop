// Sam Collier 2023
#pragma once

#include "Subsystem.h"
#include <cstdint>

#ifndef RENDERER_VULKAN
class VkInstance;
class VkSurfaceKHR;
#else
#include <vulkan/vulkan_core.h>
#endif

namespace goop::sys
{
class Window : public Subsystem
{
  public:
	virtual ~Window() = default;
	virtual int openWindow(uint32_t width, uint32_t height, const char* title) = 0;
	virtual const char** getRequiredExtensions(uint32_t* count) = 0;
	virtual void createVulkanSurface(VkInstance instance, VkSurfaceKHR* surface) = 0;
	virtual void getFramebufferSize(int* width, int* height) = 0;
	virtual void waitEvents() = 0;
	virtual bool shouldClose() = 0;
	virtual void pollEvents() = 0;
  private:
};

// Global pointer to goop's window
extern Window* gWindow;
} // namespace goop::sys
