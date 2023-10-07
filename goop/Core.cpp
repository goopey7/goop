// Sam Collier 2023
#include "Core.h"

#include <chrono>
#include <rustlib.h>

using namespace goop;

Core::Core()
{
	sys::gAudio->initialize();
	sys::gWindow->initialize();
	openWindow(800, 600, "Goop", GOOP_WINDOW_DEFAULT);
}

void Core::openWindow(uint32_t width, uint32_t height, const char* title, uint32_t flags)
{
	sys::gWindow->openWindow(width, height, title, flags);
	sys::gRenderer->initialize();
	uint32_t beep_id = sys::gAudio->loadSfx("beep.wav");
	uint32_t lazer_id = sys::gAudio->loadSfx("blast.mp3");
	sys::gAudio->playSfx(0.f, beep_id);
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

		blastTime += dt;

		printf("dt: %f\n", dt);

		if (blastTime > 2.f)
		{
			sys::gAudio->playSfx(dt, 1);
			blastTime = 0.f;
		}

		sys::gWindow->pollEvents();
		sys::gRenderer->render();

		last = now;
	}

	sys::gAudio->destroy();
	sys::gRenderer->destroy();
	sys::gWindow->destroy();
}
