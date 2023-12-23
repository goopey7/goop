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
	sfx.push_back(SoLoud::Wav());
	sfx.back().load(path.c_str());
	return sfx.size() - 1;
}

void Sfx_SoLoud::playSfx(uint32_t id) { engine.play(sfx[id]); }
