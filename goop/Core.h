// Sam Collier 2023
#pragma once

#include <goop/sys/Renderer.h>
#include <goop/sys/Window.h>
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
	void openWindow(uint32_t width, uint32_t height, const char* title);
	int handleEvents();
	void update();
	void render();
};
} // namespace goop
