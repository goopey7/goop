// Sam Collier 2023
#pragma once

#include <goop/sys/Window.h>

struct GLFWwindow;

namespace goop::sys::platform::glfw
{
class Window_GLFW : public Window
{
  public:
	// Subsystem Interface
	int initialize() final;
	int destroy() final;

	// Window Interface
	int openWindow(uint32_t width, uint32_t height, const char* title, uint32_t flags) final;
	const char** getRequiredExtensions(uint32_t* count) final;
	void createVulkanSurface(VkInstance instance, VkSurfaceKHR* surface) final;
	void getFramebufferSize(int* width, int* height) final;
	void waitEvents() final;
	bool shouldClose() final;
	void pollEvents() final;
	void hideCursor(bool hide) final;
	void grabCursor(bool grab) final;

  private:
	GLFWwindow* window = nullptr;
};
} // namespace goop::sys::platform::glfw
