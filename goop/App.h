// Sam Collier 2023
#pragma once

#include "imgui.h"
namespace goop
{
class App
{
  public:
	virtual void init() = 0;
	virtual void update(float dt) = 0;
	virtual void gui() = 0;
	virtual void render() = 0;

	ImVec2 getViewportSize() const { return viewportSize; }

  protected:
	ImVec2 viewportSize;
};
extern App* createApp(int argc, char** argv);
} // namespace goop
