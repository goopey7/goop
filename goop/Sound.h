#include "goop/sys/ResourceManager.h"
#include <goop/sys/Sfx.h>
namespace goop
{
	inline void loadSfx(const std::string& path)
	{
		goop::rm->loadSfx(path);
	}

	inline void playSfx(const std::string& path)
	{
		goop::rm->playSfx(path);
	}
}
