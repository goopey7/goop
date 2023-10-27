// Sam Collier 2023
#include "Core.h"

#include <chrono>
#include <rustlib.h>

using namespace goop;

const std::unique_ptr<goop::sys::ResourceManager> goop::rm =
	std::make_unique<goop::sys::ResourceManager>();

Core::Core(int argc, char** argv) : app(createApp(argc, argv))
{
	sys::gAudio->initialize();
	sys::gWindow->initialize();
	sys::gWindow->openWindow(800, 600, "Goop", GOOP_WINDOW_DEFAULT);
	rm->initialize();

	app->init();
}

void Core::run()
{
	say_hello("Sam");

	auto last = std::chrono::high_resolution_clock::now();
	float blastTime = 0.f;

	while (!sys::gWindow->shouldClose())
	{
		// get delta time with chrono
		auto now = std::chrono::high_resolution_clock::now();
		auto dt = std::chrono::duration<float>(now - last).count();
		sys::gWindow->pollEvents();

		app->update(dt);

		sys::gRenderer->render();

		last = now;
	}

	sys::gAudio->destroy();
	sys::gRenderer->destroy();
	sys::gWindow->destroy();
}
