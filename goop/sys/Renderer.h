// Sam Collier 2023
#pragma once

#include "Subsystem.h"
#include "Window.h"
#include "goop/Scene.h"
#include "goop/sys/ResourceManager.h"
#include "imgui.h"
#include <goop/sys/MeshLoader.h>
#include <queue>

namespace goop::sys
{
class Renderer : public Subsystem
{
  public:
	Renderer() = default;
	virtual void beginFrame() = 0;
	virtual void endFrame() = 0;
#ifdef GOOP_APPTYPE_EDITOR
	virtual void render(Scene* scene, float width = -1.f, float height = -1.f) = 0;
#else
	virtual void render(Scene* scene) = 0;
#endif
	virtual void addToRenderQueue(uint32_t mesh, MeshLoader* meshLoader);
	bool isMeshQueueEmpty() const { return meshQueue.empty(); }

	virtual ImTextureID getViewTexture() const = 0;

  protected:
	std::queue<uint32_t> meshQueue;
	MeshLoader* meshLoader;
	Scene* scene;
};

// global pointer to goop's renderer
extern const std::unique_ptr<Renderer> gRenderer;
} // namespace goop::sys
