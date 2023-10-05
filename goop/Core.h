// Sam Collier 2023
#pragma once

#include <goop/sys/Renderer.h>
#include <goop/sys/Window.h>
#include <goop/sys/Audio.h>
#include <cstdint>
#include <memory>

namespace goop
{
class Core
{
  public:
	Core();
	Core(const Core&) = delete;
	Core& operator=(const Core&) = delete;

	void run();

  private:
	void openWindow(uint32_t width, uint32_t height, const char* title, uint32_t flags);
	int handleEvents();
	void update();
	void render();
};
} // namespace goop
