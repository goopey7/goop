// Sam Collier 2023
#pragma once

#include <goop/sys/Audio.h>
#include <map>
#include <soloud.h>
#include <soloud_wav.h>
#include <vector>

namespace goop::sys::platform::soloud
{
class Audio_SoLoud : public Audio
{
  public:
	  // Subsystem interface
	  virtual int initialize() final;
	  virtual int destroy() final;

	  // Audio interface
	  virtual uint32_t loadSfx(const char* path) final;
	  virtual void playSfx(float dt, uint32_t id) final;
	  virtual uint32_t loadMusic(const char* path) final;
	  virtual void playMusic(uint32_t id) final;

  private:
	  SoLoud::Soloud engine;
	  std::vector<std::unique_ptr<SoLoud::Wav>> sfx;
};
} // namespace goop::sys::platform::soloud
