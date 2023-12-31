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

bool ResourceManager::loadMesh(MeshComponent& mesh, const char* oldPath)
{
	if (oldPath != nullptr)
	{
		if (numLoadedMeshes[oldPath] == 0)
		{
			std::cout << "Mesh " << oldPath << " not loaded" << std::endl;
		}
		else
		{
			numLoadedMeshes[oldPath]--;
			if (numLoadedMeshes[oldPath] == 0)
			{
				std::cout << "Unloading mesh " << oldPath << std::endl;
				meshLoader->unload(oldPath);
				loadedMeshes.erase(oldPath);
			}
		}
	}

	if (numLoadedMeshes[mesh.path] > 0)
	{
		std::cout << "Mesh " << mesh.path << " already loaded" << std::endl;
		mesh.id = loadedMeshes[mesh.path];
		numLoadedMeshes[mesh.path]++;
		return true;
	}
	mesh.id = meshLoader->load(mesh.path);
	loadedMeshes[mesh.path] = mesh.id;
	numLoadedMeshes[mesh.path]++;
	std::cout << "Loaded mesh " << mesh.path << " with id " << mesh.id << std::endl;
	return true;
}

bool ResourceManager::unloadMesh(MeshComponent& mesh)
{
	numLoadedMeshes[mesh.path]--;
	if (numLoadedMeshes[mesh.path] == 0)
	{
		std::cout << "Unloading mesh " << mesh.path << std::endl;
		meshLoader->unload(mesh.path);
		loadedMeshes.erase(mesh.path);
	}
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

