#include "goop/sys/ResourceManager.h"
#include <goop/sys/Sfx.h>
namespace goop
{
	static std::map<std::string, int> sfxMap;
	static void loadSfx(const std::string& path)
	{
		goop::rm->loadSfx(path);
		sfxMap[path] = sfxMap.size();
	}

	static void playSfx(const std::string& path)
	{
		goop::rm->playSfx(sfxMap[path]);
	}
}
