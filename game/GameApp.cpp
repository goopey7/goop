#include "GameApp.h"
#include <imgui.h>
#include <iostream>

#define GOOP_RENDERER_VULKAN
#include "goop/sys/platform/vulkan/Renderer_Vulkan.h"
#include <goop/Core.h>
#include <goop/sys/Renderer.h>
#include <goop/sys/Sfx.h>

goop::App* goop::createGame(int argc, char** argv) { return new GameApp(); }

void GameApp::init()
{
	std::cout << "Hello from GameApp!" << std::endl;

	// TODO dynamically load and unload meshes and handle instances etc
	// TODO user should never be using goop::sys
	goop::rm->loadMesh("res/viking_room.obj");
	goop::rm->loadMesh("res/cow.obj");

	// goop::sys::gRenderer->addToRenderQueue(goop::res::COW, goop::rm->getMeshLoader());
	// goop::sys::gRenderer->addToRenderQueue(goop::res::VIKING_ROOM, goop::rm->getMeshLoader());
	goop::rm->loadSfx("res/blast.mp3");
	goop::rm->playSfx(goop::res::LAZER);

	/* TODO -------------
	   - store EVERYTHING contiguously - decide on a data structure - (week 7)
	   - dynamically generate a header file for resource enums

	   - End user should not worry about loading / unloading resources
			- should be abstracted once ECS is in -- but sfx??
   */
}

void GameApp::update(float dt)
{
	// std::cout << "delta time: " << dt << std::endl;
}

void GameApp::gui()
{
	ImGui::Begin("hello");
		ImGui::Text("Hello from GameApp!");
		ImGui::Button("Button");
	ImGui::End();
}

void GameApp::render()
{
	goop::sys::gRenderer->addToRenderQueue(goop::res::VIKING_ROOM, goop::rm->getMeshLoader());
	goop::sys::gRenderer->addToRenderQueue(goop::res::COW, goop::rm->getMeshLoader());
}
