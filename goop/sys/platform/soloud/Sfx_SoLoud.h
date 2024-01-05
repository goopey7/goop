// Sam Collier 2023
#pragma once

#include <goop/sys/Sfx.h>
#include <map>
#include <soloud.h>
#include <soloud_wav.h>
#include <vector>
#include <queue>

namespace goop::sys::platform::soloud
{
class Sfx_SoLoud : public Sfx
{
  public:
	  // Subsystem interface
	  virtual int initialize() final;
	  virtual int destroy() final;

	  // Resource Subsystem interface
	  virtual uint32_t load(const std::string& path) final;
	  virtual uint32_t unload(const std::string& path) final;

	  // Sfx interface
	  virtual void playSfx(uint32_t id) final;

  private:
	  SoLoud::Soloud engine;
	  std::map<uint32_t, SoLoud::Wav> sfx;
	  std::queue<uint32_t> unloadedSfxSlots;
};
} // namespace goop::sys::platform::soloud
