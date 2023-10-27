#include "EditorApp.h"
#include <iostream>

#define RENDERER_VULKAN
#include <goop/Core.h>
#include <goop/sys/Renderer.h>
#include <goop/sys/Audio.h>

goop::App* goop::createApp(int argc, char** argv) { return new EditorApp(); }

void EditorApp::init()
{
	std::cout << "Hello from EditorApp!" << std::endl;

	// TODO dynamically load and unload meshes and handle instances etc
	// TODO user should never be using goop::sys
	goop::rm->loadMesh("res/viking_room.obj");
	goop::sys::gRenderer->initialize(); // TODO once that's done move this to Core.cpp

	uint32_t beep_id = goop::sys::gAudio->loadSfx("res/beep.wav");
	uint32_t lazer_id = goop::sys::gAudio->loadSfx("res/blast.mp3");
	goop::sys::gAudio->playSfx(0.f, lazer_id);
}

void EditorApp::update(float dt)
{
	std::cout << "delta time: " << dt << std::endl;
}
