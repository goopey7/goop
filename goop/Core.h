// Sam Collier 2023
#pragma once

#include <goop/App.h>
#include <goop/sys/Renderer.h>
#include <goop/sys/Window.h>
#include <goop/sys/Audio.h>
#include <goop/sys/ResourceManager.h>
#include <cstdint>
#include <memory>

namespace goop
{
class Core
{
  public:
	Core(int argc, char** argv);
	Core(const Core&) = delete;
	Core& operator=(const Core&) = delete;

	void run();

  private:
	int handleEvents();
	void update();
	void render();

	const float blastFreq = 5.0f;

	App* app;
};
} // namespace goop
