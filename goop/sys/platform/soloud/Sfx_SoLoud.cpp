// Sam Collier 2023
#include "Sfx_SoLoud.h"
#include <SDL2/SDL.h>

using namespace goop::sys::platform::soloud;

int Sfx_SoLoud::initialize()
{
	int sdl_result = SDL_InitSubSystem(SDL_INIT_AUDIO);
	int soloud_result = engine.init();
	bIsInitialized = true;
	return sdl_result || soloud_result;
}

int Sfx_SoLoud::destroy()
{
	engine.deinit();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);
	bIsInitialized = false;
	return 0;
}

uint32_t Sfx_SoLoud::load(const std::string& path)
{
	if (!unloadedSfxSlots.empty())
	{
		uint32_t id = unloadedSfxSlots.front();
		unloadedSfxSlots.pop();
		sfx[id].load(path.c_str());
		return id;
	}
	else
	{
		uint32_t id = sfx.size();
		sfx[id].load(path.c_str());
		return id;
	}
}

void Sfx_SoLoud::playSfx(uint32_t id) { engine.play(sfx[id]); }

uint32_t Sfx_SoLoud::unload(const std::string& path)
{
	return 0;
}

