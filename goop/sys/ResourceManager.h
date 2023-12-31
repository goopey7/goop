// Sam Collier 2023
#pragma once

#include "goop/sys/Sfx.h"
#include <goop/sys/MeshLoader.h>
#include <goop/sys/Subsystem.h>
#include <goop/Components.h>
#include <map>
#include <string>

namespace goop::sys
{
class ResourceManager : public Subsystem
{
  public:
	virtual int initialize() final;
	virtual int destroy() final;

	virtual bool loadMesh(MeshComponent& mesh, const char* oldPath = nullptr);
	virtual bool unloadMesh(MeshComponent& mesh);
	virtual bool loadSfx(const std::string& path);

	void playSfx(uint32_t id) const;

	const MeshLoader* getMeshLoader() const { return meshLoader.get(); }
	MeshLoader* getMeshLoader() { return meshLoader.get(); }

  private:
	std::unique_ptr<MeshLoader> meshLoader;
	std::unique_ptr<Sfx> sfx;
	std::map<std::string, uint32_t> loadedMeshes;
	std::map<std::string, uint32_t> numLoadedMeshes;
};
} // namespace goop::sys
