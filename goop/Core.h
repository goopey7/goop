// Sam Collier 2023
#pragma once

#include "goop/Scene.h"
#include <goop/App.h>
#include <goop/sys/Renderer.h>
#include <goop/sys/Window.h>
#include <goop/sys/Sfx.h>
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
	Scene scene;
};
extern const std::unique_ptr<sys::ResourceManager> rm;
} // namespace goop
