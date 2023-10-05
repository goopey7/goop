// Sam Collier 2023

#include "Audio_SoLoud.h"
#include <SDL2/SDL.h>

#ifdef AUDIO_SOLOUD
goop::sys::platform::soloud::Audio_SoLoud gAudio_SoLoud;
goop::sys::Audio* goop::sys::gAudio = &gAudio_SoLoud;
#endif

using namespace goop::sys::platform::soloud;

int Audio_SoLoud::initialize()
{
	int sdl_result = SDL_Init(SDL_INIT_AUDIO);
	int soloud_result = engine.init();
	return sdl_result || soloud_result;
}

int Audio_SoLoud::destroy()
{
	SDL_QuitSubSystem(SDL_INIT_AUDIO);
	engine.deinit();
	return 0;
}

uint32_t Audio_SoLoud::loadSfx(const char* path)
{
	sfx.load(path);
	return 0;
}

void Audio_SoLoud::playSfx(uint32_t id) { engine.play(sfx); }

uint32_t Audio_SoLoud::loadMusic(const char* path) { return 0; }

void Audio_SoLoud::playMusic(uint32_t id) {}
