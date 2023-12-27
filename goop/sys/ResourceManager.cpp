// Sam Collier 2023
#include "ResourceManager.h"

#ifdef GOOP_MESHLOADER_ASSIMP
#include <goop/sys/platform/assimp/MeshLoader_Assimp.h>
#endif

#ifdef GOOP_AUDIO_SOLOUD
#include "goop/sys/platform/soloud/Sfx_SoLoud.h"
#endif

#include <iostream>

using namespace goop::sys;

int ResourceManager::initialize()
{
#ifdef GOOP_MESHLOADER_ASSIMP
	meshLoader = std::make_unique<platform::assimp::MeshLoader_Assimp>();
#endif

#ifdef GOOP_AUDIO_SOLOUD
	sfx = std::make_unique<platform::soloud::Sfx_SoLoud>();
#endif

	meshLoader->initialize();
	sfx->initialize();

	bIsInitialized = true;
	return 0;
}

bool ResourceManager::loadMesh(MeshComponent& mesh)
{
	if (loadedMeshes[mesh.path] != 0)
	{
		std::cout << "Mesh " << mesh.path << " already loaded" << std::endl;
		mesh.id = loadedMeshes[mesh.path];
		return true;
	}
	mesh.id = meshLoader->load(mesh.path);
	loadedMeshes[mesh.path] = mesh.id;
	std::cout << "Loaded mesh " << mesh.path << " with id " << mesh.id << std::endl;
	return true;
}
bool ResourceManager::loadSfx(const std::string& path) { return sfx->load(path); }

int ResourceManager::destroy()
{
	sfx->destroy();
	meshLoader->destroy();
	return 0;
}

void ResourceManager::playSfx(uint32_t id) const { sfx->playSfx(id); }
