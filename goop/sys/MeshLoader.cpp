// Sam Collier 2023

#include <goop/sys/platform/assimp/MeshLoader_Assimp.h>

using namespace goop::sys;

int MeshLoader::initialize()
{
	data = std::make_unique<std::map<uint32_t, MeshImportData>>();
	return 0;
}

