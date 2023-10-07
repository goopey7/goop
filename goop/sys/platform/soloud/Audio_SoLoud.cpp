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
	for (auto& wav : sfx)
	{
		delete wav;
	}
	engine.deinit();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);
	return 0;
}

uint32_t Audio_SoLoud::loadSfx(const char* path)
{
	sfx.push_back(new SoLoud::Wav());
	sfx.back()->load(path);
	return sfx.size() - 1;
}

void Audio_SoLoud::playSfx(float dt, uint32_t id) { engine.playClocked(dt, *sfx[id]); }

uint32_t Audio_SoLoud::loadMusic(const char* path) { return 0; }

void Audio_SoLoud::playMusic(uint32_t id) {}
