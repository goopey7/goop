// Sam Collier 2023
#include "Core.h"
#include "sys/platform/Renderer_Vulkan.h"
#include "sys/platform/Window_GLFW.h"

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
	while (!sys::gWindow->shouldClose())
	{
		sys::gWindow->pollEvents();
		sys::gRenderer->render();
	}

	sys::gRenderer->destroy();
	sys::gWindow->destroy();
}

} // namespace goop
