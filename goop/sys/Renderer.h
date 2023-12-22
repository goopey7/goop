// Sam Collier 2023
#pragma once

#include "Subsystem.h"
#include "Window.h"
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
	virtual void render(float width = -1.f, float height = -1.f) = 0;
#else
	virtual void render() = 0;
#endif
	virtual void addToRenderQueue(goop::res::mesh mesh, MeshLoader* meshLoader);
	bool isMeshQueueEmpty() const { return meshQueue.empty(); }

	virtual ImTextureID getViewTexture() const = 0;

  protected:
	std::queue<goop::res::mesh> meshQueue;
	MeshLoader* meshLoader;
};

// global pointer to goop's renderer
extern const std::unique_ptr<Renderer> gRenderer;
} // namespace goop::sys
