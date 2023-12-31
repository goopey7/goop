// Sam Collier 2023
#pragma once

#include <assimp/Importer.hpp>
#include <goop/sys/MeshLoader.h>
#include <map>

namespace goop::sys::platform::assimp
{
class MeshLoader_Assimp : public MeshLoader
{
  public:
	MeshLoader_Assimp(const MeshLoader_Assimp&) = delete;
	MeshLoader_Assimp& operator=(const MeshLoader_Assimp&) = delete;
	MeshLoader_Assimp();
	~MeshLoader_Assimp();

	// Subsystem Interface
	int initialize() final;
	int destroy() final;

	// ResourceSubsystem Interface
	uint32_t load(const std::string& path) final;
	uint32_t unload(const std::string& path) final;

  private:
	Assimp::Importer importer;
	std::map<std::string, uint32_t> loadedMeshes;
};
} // namespace goop::sys::platform::assimp
