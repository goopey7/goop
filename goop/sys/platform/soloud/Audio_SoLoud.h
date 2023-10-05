// Sam Collier 2023
#pragma once

#include <goop/sys/Audio.h>
#include <soloud.h>
#include <soloud_wav.h>

namespace goop::sys::platform::soloud
{
class Audio_SoLoud : public Audio
{
  public:
	  Audio_SoLoud() = default;
	  // Subsystem interface
	  virtual int initialize() final;
	  virtual int destroy() final;

	  // Audio interface
	  virtual uint32_t loadSfx(const char* path) final;
	  virtual void playSfx(uint32_t id) final;
	  virtual uint32_t loadMusic(const char* path) final;
	  virtual void playMusic(uint32_t id) final;

  private:
	  SoLoud::Soloud engine;
	  SoLoud::Wav sfx;
};
} // namespace goop::sys::platform::soloud
