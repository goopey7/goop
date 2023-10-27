#include "EditorApp.h"
#include <iostream>

#define RENDERER_VULKAN
#include <goop/sys/Renderer.h>
#include <goop/sys/ResourceManager.h>

goop::App* goop::createApp(int argc, char** argv) { return new EditorApp(); }

void EditorApp::init()
{
	std::cout << "Hello from EditorApp!" << std::endl;
	goop::sys::gResourceManager->loadMesh("res/viking_room.obj");
	goop::sys::gRenderer->initialize();
}

void EditorApp::update(float dt) { std::cout << "delta time: " << dt << std::endl; }
