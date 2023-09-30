// Sam Collier 2023
#pragma once

#include <goop/sys/Window.h>

class GLFWwindow;

namespace goop::sys::platform
{
class Window_GLFW : public Window
{
  public:
	// Subsystem Interface
	int initialize() final;
	int destroy() final;

	// Window Interface
	int openWindow(uint32_t width, uint32_t height, const char* title) final;
	const char** getRequiredExtensions(uint32_t* count) final;
	void createVulkanSurface(VkInstance instance, VkSurfaceKHR* surface) final;
	void getFramebufferSize(int* width, int* height) final;
	void waitEvents() final;
	bool shouldClose() final;
	void pollEvents() final;

  private:
	GLFWwindow* window = nullptr;
};
} // namespace goop::sys::platform
