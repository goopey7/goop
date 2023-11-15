// Sam Collier 2023
#pragma once

#include "goop/sys/Sfx.h"
#include <goop/sys/MeshLoader.h>
#include <goop/sys/Subsystem.h>
#include <string>

namespace goop::res
{
	enum mesh
	{
		VIKING_ROOM,
		COW,
	};
	enum sfx
	{
		LAZER,
		BEEP,
	};
}

namespace goop::sys
{
class ResourceManager : public Subsystem
{
  public:
	virtual int initialize() final;
	virtual int destroy() final;

	virtual bool loadMesh(const std::string& path);
	virtual bool loadSfx(const std::string& path);

	void playSfx(uint32_t id) const;

	const MeshLoader* getMeshLoader() const { return meshLoader.get(); }
	MeshLoader* getMeshLoader() { return meshLoader.get(); }

  private:
	std::unique_ptr<MeshLoader> meshLoader;
	std::unique_ptr<Sfx> sfx;
};
} // namespace goop::sys
