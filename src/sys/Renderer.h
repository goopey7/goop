// Sam Collier 2023
#pragma once

#include "Subsystem.h"
#include "Window.h"

namespace goop::sys
{
class Renderer : public Subsystem
{
  public:
	virtual ~Renderer() = default;
	virtual void render() = 0;
};

// global pointer to goop's renderer
extern Renderer* gRenderer;
} // namespace goop::sys
