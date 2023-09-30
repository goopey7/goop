// Sam Collier 2023
#include "Core.h"
#include <goop/sys/platform/vulkan/Renderer_Vulkan.h>
#include <goop/sys/platform/glfw/Window_GLFW.h>

#include <rustlib.h>

namespace goop
{

Core::Core() { sys::gWindow->initialize(); openWindow(800, 600, "Goop");}

void Core::openWindow(uint32_t width, uint32_t height, const char* title)
{
	sys::gWindow->openWindow(width, height, title);
	sys::gRenderer->initialize();
}

void Core::run()
{
	say_hello("Sam");

	while (!sys::gWindow->shouldClose())
	{
		sys::gWindow->pollEvents();
		sys::gRenderer->render();
	}

	sys::gRenderer->destroy();
	sys::gWindow->destroy();
}

} // namespace goop
