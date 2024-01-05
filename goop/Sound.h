#include "goop/sys/ResourceManager.h"
#include <goop/sys/Sfx.h>
namespace goop
{
	static void loadSfx(const std::string& path)
	{
		goop::rm->loadSfx(path);
	}

	static void playSfx(const std::string& path)
	{
		goop::rm->playSfx(path);
	}
}
