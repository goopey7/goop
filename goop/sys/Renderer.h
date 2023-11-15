// Sam Collier 2023
#pragma once

#include "Subsystem.h"
#include "Window.h"
#include <goop/sys/MeshLoader.h>
#include "goop/sys/ResourceManager.h"
#include <queue>

namespace goop::sys
{
class Renderer : public Subsystem
{
  public:
	Renderer() = default;
	virtual void beginFrame() = 0;
	virtual void render() = 0;
	virtual void addToRenderQueue(goop::res::mesh mesh, MeshLoader* meshLoader);
	bool isMeshQueueEmpty() const { return meshQueue.empty(); }

  protected:
	std::queue<goop::res::mesh> meshQueue;
	MeshLoader* meshLoader;
};

// global pointer to goop's renderer
extern const std::unique_ptr<Renderer> gRenderer;
} // namespace goop::sys
