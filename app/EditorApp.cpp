#include "EditorApp.h"
#include <imgui.h>
#include <iostream>

#define GOOP_APPTYPE_EDITOR

#define GOOP_RENDERER_VULKAN
#include "goop/sys/platform/vulkan/Renderer_Vulkan.h"
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
	goop::rm->loadMesh("res/cow.obj");

	//goop::sys::gRenderer->addToRenderQueue(goop::res::COW, goop::rm->getMeshLoader());
	//goop::sys::gRenderer->addToRenderQueue(goop::res::VIKING_ROOM, goop::rm->getMeshLoader());
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
	// std::cout << "delta time: " << dt << std::endl;
}

void EditorApp::gui()
{
	auto r = (goop::sys::platform::vulkan::Renderer_Vulkan*)goop::sys::gRenderer.get();
	ImGui::Begin("Viewport");
	ImVec2 viewportSize = ImGui::GetContentRegionAvail();
	ImGui::Image((ImTextureID)r->getImageDescriptorSet(), viewportSize);
	ImGui::End();

	ImGui::Begin("Inspector");
	if (ImGui::Button("Toggle House"))
	{
		shouldSpawnHouse = !shouldSpawnHouse;
	}
	if (ImGui::Button("Toggle Cow"))
	{
		shouldSpawnCow = !shouldSpawnCow;
	}
	if (ImGui::Button("Play Sfx"))
	{
		goop::rm->playSfx(goop::res::LAZER);
	}
	ImGui::End();
}

void EditorApp::render()
{
	if (shouldSpawnHouse)
		goop::sys::gRenderer->addToRenderQueue(goop::res::VIKING_ROOM, goop::rm->getMeshLoader());
	if (shouldSpawnCow)
		goop::sys::gRenderer->addToRenderQueue(goop::res::COW, goop::rm->getMeshLoader());
}

