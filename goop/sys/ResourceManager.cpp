// Sam Collier 2023

#include "ResourceManager.h"

#ifdef ML_ASSIMP
#include <goop/sys/platform/assimp/MeshLoader_Assimp.h>
#endif

using namespace goop::sys;

int ResourceManager::initialize()
{
#ifdef ML_ASSIMP
	meshLoader = std::make_unique<platform::assimp::MeshLoader_Assimp>();
#endif
	meshLoader->initialize();

	bIsInitialized = true;
	return 0;
}

bool ResourceManager::loadMesh(const std::string& path) { return meshLoader->load(path); }

int ResourceManager::destroy()
{
	meshLoader->destroy();
	return 0;
}

