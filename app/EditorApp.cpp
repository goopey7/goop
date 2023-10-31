#include "EditorApp.h"
#include <iostream>

#define RENDERER_VULKAN
#include <goop/Core.h>
#include <goop/sys/Renderer.h>
#include <goop/sys/Sfx.h>

goop::App* goop::createApp(int argc, char** argv) { return new EditorApp(); }

void EditorApp::init()
{
	std::cout << "Hello from EditorApp!" << std::endl;

	// TODO dynamically load and unload meshes and handle instances etc
	// TODO user should never be using goop::sys
	goop::rm->loadMesh("res/viking_room.obj");
	goop::sys::gRenderer->initialize(); // TODO once that's done move this to Core.cpp

	goop::rm->loadSfx("res/blast.mp3");
	goop::rm->playSfx(goop::res::LAZER);

	/* TODO -------------
	   - store EVERYTHING contiguously - decide on a data structure - (week 7)
	   - dynamically generate a header file for resource enums
	   
	   - End user should not worry about loading / unloading resources
	   		- should be abstracted once ECS is in -- but sfx??
   */
}

void EditorApp::update(float dt)
{
	std::cout << "delta time: " << dt << std::endl;
}
