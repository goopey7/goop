// Sam Collier 2023
#include "ResourceManager.h"

#ifdef GOOP_MESHLOADER_ASSIMP
#include <goop/sys/platform/assimp/MeshLoader_Assimp.h>
#endif

#ifdef GOOP_AUDIO_SOLOUD
#include <goop/sys/platform/soloud/Sfx_SoLoud.h>
#endif

#ifdef GOOP_IMGLOADER_STB
#include <goop/sys/platform/stb/ImageLoader_STB.h>
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

#ifdef GOOP_IMGLOADER_STB
	imgLoader = std::make_unique<platform::stb::ImageLoader_STB>();
#endif

	meshLoader->initialize();
	imgLoader->initialize();
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
			if (numLoadedMeshes[oldPath] <= 0)
			{
				std::cout << "Unloading mesh " << oldPath << std::endl;
				meshLoader->unload(oldPath);
				loadedMeshes.erase(oldPath);
				numLoadedMeshes.erase(oldPath);
			}
		}
	}

	// if primitive is not set - load from path provided
	if (std::holds_alternative<std::monostate>(mesh.primitive))
	{
		if (numLoadedMeshes[mesh.path] > 0)
		{
			std::cout << "Mesh " << mesh.path << " already loaded" << std::endl;
			mesh.id = loadedMeshes[mesh.path];
			numLoadedMeshes[mesh.path]++;
			std::cout << "Loaded meshid: " << mesh.id << std::endl;
			return true;
		}
		mesh.id = meshLoader->load(mesh.path);
	}
	else // load primitive
	{
		if (numLoadedMeshes[mesh.path] > 0)
		{
			std::cout << "Mesh " << mesh.path << " already loaded" << std::endl;
			mesh.id = loadedMeshes[mesh.path];
			numLoadedMeshes[mesh.path]++;
			std::cout << "Loaded meshid: " << mesh.id << std::endl;
			return true;
		}
		mesh.id = meshLoader->loadPrimitive(mesh.primitive);
	}
	loadedMeshes[mesh.path] = mesh.id;
	numLoadedMeshes[mesh.path]++;
	std::cout << "Loaded mesh " << mesh.path << " with id " << mesh.id << std::endl;
	return true;
}

bool ResourceManager::unloadMesh(MeshComponent& mesh)
{
	numLoadedMeshes[mesh.path]--;
	if (numLoadedMeshes[mesh.path] <= 0)
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

bool ResourceManager::loadTexture(MeshComponent& mesh, const char* oldPath)
{
	if (oldPath != nullptr)
	{
		if (numLoadedTextures[oldPath] == 0)
		{
			std::cout << "Texture " << oldPath << " not loaded" << std::endl;
		}
		else
		{
			numLoadedTextures[oldPath]--;
			if (numLoadedTextures[oldPath] <= 0)
			{
				std::cout << "Unloading texture " << oldPath << std::endl;
				imgLoader->unload(oldPath);
				numLoadedTextures.erase(oldPath);
			}
		}
	}

	if (numLoadedTextures[mesh.texturePath] > 0)
	{
		std::cout << "Texture " << mesh.texturePath << " already loaded" << std::endl;
		numLoadedTextures[mesh.texturePath]++;
		return true;
	}
	imgLoader->load(mesh.texturePath);
	numLoadedTextures[mesh.texturePath]++;
	std::cout << "Loaded texture " << mesh.texturePath << std::endl;
	return true;
}

bool ResourceManager::unloadTexture(MeshComponent& mesh)
{
	numLoadedTextures[mesh.texturePath]--;
	if (numLoadedTextures[mesh.texturePath] <= 0)
	{
		std::cout << "Unloading texture " << mesh.texturePath << std::endl;
		imgLoader->unload(mesh.texturePath);
	}
	return true;
}
