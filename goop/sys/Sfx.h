// Sam Collier 2023
#pragma once

#include "ResourceSubsystem.h"
#include <cstdint>
#include <memory>

namespace goop::sys
{
class Sfx : public ResourceSubsystem
{
  public:
	virtual void playSfx(uint32_t id) = 0;
};
} // namespace goop::sys
