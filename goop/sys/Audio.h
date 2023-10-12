// Sam Collier 2023
#pragma once

#include "Subsystem.h"
#include <cstdint>
#include <memory>

namespace goop::sys
{
class Audio : public Subsystem
{
  public:
	virtual uint32_t loadSfx(const char* path) = 0;
	virtual void playSfx(float dt, uint32_t id) = 0;

	virtual uint32_t loadMusic(const char* path) = 0;
	virtual void playMusic(uint32_t id) = 0;

  private:
};
// Global pointer to goop's audio
extern std::unique_ptr<Audio> gAudio;
} // namespace goop::sys
