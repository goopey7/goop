// Sam Collier 2023
#pragma once

#include <goop/sys/Sfx.h>
#include <map>
#include <soloud.h>
#include <soloud_wav.h>
#include <vector>

namespace goop::sys::platform::soloud
{
class Sfx_SoLoud : public Sfx
{
  public:
	  // Subsystem interface
	  virtual int initialize() final;
	  virtual int destroy() final;

	  // Resource Subsystem interface
	  virtual bool load(const std::string& path) final;

	  // Sfx interface
	  virtual void playSfx(uint32_t id) final;

  private:
	  SoLoud::Soloud engine;
	  std::vector<SoLoud::Wav> sfx;
};
} // namespace goop::sys::platform::soloud
