// Sam Collier 2023
#pragma once

#include "goop/sys/ImageLoader.h"
#include "goop/sys/Sfx.h"
#include <goop/sys/MeshLoader.h>
#include <goop/sys/Subsystem.h>
#include <map>
#include <string>
#include <goop/Components.h>

namespace goop::sys
{
class ResourceManager : public Subsystem
{
  public:
	virtual int initialize() final;
	virtual int destroy() final;

	bool loadMesh(MeshComponent* mesh, const char* oldPath = nullptr);
	bool unloadMesh(MeshComponent* mesh);

	bool loadTexture(MeshComponent* mesh, const char* oldPath = nullptr);
	bool unloadTexture(MeshComponent* mesh);

	bool loadSfx(const std::string& path);
	bool unloadSfx(const std::string& path);
	void playSfx(const std::string& path) const;

	const MeshLoader* getMeshLoader() const { return meshLoader.get(); }
	MeshLoader* getMeshLoader() { return meshLoader.get(); }

  private:
	std::unique_ptr<MeshLoader> meshLoader;
	std::unique_ptr<ImageLoader> imgLoader;
	std::unique_ptr<Sfx> sfx;
	std::map<std::string, uint32_t> loadedMeshes;
	std::map<std::string, uint32_t> numLoadedMeshes;
	std::map<std::string, uint32_t> numLoadedTextures;
	std::map<std::string, uint32_t> loadedSfx;
	std::queue<uint32_t> unloadedSfxSlots;
};
} // namespace goop::sys
namespace goop
{
extern const std::unique_ptr<sys::ResourceManager> rm;
}
