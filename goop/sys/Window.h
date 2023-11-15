// Sam Collier 2023
#pragma once

#include "Subsystem.h"
#include <cstdint>
#include <memory>

#ifndef GOOP_RENDERER_VULKAN
struct VkInstance;
struct VkSurfaceKHR;
#else
#include <volk.h>
#endif

namespace goop
{
typedef enum WindowFlags
{
	GOOP_WINDOW_RESIZABLE = 1 << 0,

	// Defaults: Resizeable Window
	GOOP_WINDOW_DEFAULT = GOOP_WINDOW_RESIZABLE,
} WindowFlags;
}

namespace goop::sys
{

class Window : public Subsystem
{
  public:
	virtual ~Window() = default;
	virtual int openWindow(uint32_t width, uint32_t height, const char* title, uint32_t flags) = 0;
	virtual const char** getRequiredExtensions(uint32_t* count) = 0;
	virtual void createVulkanSurface(VkInstance instance, VkSurfaceKHR* surface) = 0;
	virtual void getFramebufferSize(int* width, int* height) = 0;
	virtual void waitEvents() = 0;
	virtual bool shouldClose() = 0;
	virtual void pollEvents() = 0;

  private:
};

// Global pointer to goop's window
extern const std::unique_ptr<Window> gWindow;
} // namespace goop::sys
