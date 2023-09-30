// Sam Collier 2023
#include "Core.h"

#include <rustlib.h>

using namespace goop;

Core::Core()
{
	sys::gWindow->initialize();
	openWindow(800, 600, "Goop", GOOP_WINDOW_DEFAULT);
}

void Core::openWindow(uint32_t width, uint32_t height, const char* title, uint32_t flags)
{
	sys::gWindow->openWindow(width, height, title, flags);
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

