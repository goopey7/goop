// Sam Collier 2023

#include "Window_GLFW.h"
#include <vulkan/vulkan_core.h>
#ifdef RENDERER_VULKAN
#define GLFW_INCLUDE_VULKAN
#endif
#include <GLFW/glfw3.h>
#include <stdexcept>

#ifdef WINDOW_GLFW
goop::sys::platform::Window_GLFW gWindow_GLFW;
goop::sys::Window* goop::sys::gWindow = &gWindow_GLFW;
#endif

using namespace goop::sys::platform;

int Window_GLFW::initialize()
{
	glfwInit();
	return 0;
}

int Window_GLFW::destroy()
{
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

int Window_GLFW::openWindow(uint32_t width, uint32_t height, const char* title, uint32_t flags)
{
#ifndef RENDERER_OPENGL
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
#endif
	glfwWindowHint(GLFW_RESIZABLE, (flags & GOOP_WINDOW_RESIZABLE) ? GLFW_TRUE : GLFW_FALSE);
	window = glfwCreateWindow(width, height, title, nullptr, nullptr);
	return 0;
}

void Window_GLFW::createVulkanSurface(VkInstance instance, VkSurfaceKHR* surface)
{
#ifdef RENDERER_VULKAN
	if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create window surface!");
	}
#else
	throw std::runtime_error("Vulkan not enabled!");
#endif
}

const char** Window_GLFW::getRequiredExtensions(uint32_t* count)
{
	return glfwGetRequiredInstanceExtensions(count);
}

void Window_GLFW::getFramebufferSize(int* width, int* height)
{
	glfwGetFramebufferSize(window, width, height);
}

void Window_GLFW::waitEvents()
{
	glfwWaitEvents();
}

bool Window_GLFW::shouldClose()
{
	return glfwWindowShouldClose(window);
}

void Window_GLFW::pollEvents()
{
	glfwPollEvents();
}

