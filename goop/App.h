// Sam Collier 2023
#pragma once

#include "goop/Scene.h"
#ifdef GOOP_APPTYPE_EDITOR
#include <imgui.h>
#endif

namespace goop
{
class App
{
  public:
	App(Scene* scene = nullptr) : scene(scene) {}
	virtual void init() = 0;
	virtual void update(float dt) = 0;
	virtual void gui() = 0;
#ifdef GOOP_APPTYPE_EDITOR
	ImVec2 getViewportSize() const { return viewportSize; }
#endif

  protected:
	goop::Scene* scene;
#ifdef GOOP_APPTYPE_EDITOR
	ImVec2 viewportSize;
#endif
};
#ifdef GOOP_APPTYPE_EDITOR
extern App* createEditor(int argc, char** argv, App* game);
#endif
extern App* createGame(int argc, char** argv, Scene* scene);
} // namespace goop
