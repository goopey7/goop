#include "EditorApp.h"
#include <imgui.h>
#include <iostream>

#define GOOP_RENDERER_VULKAN
#include "goop/sys/platform/vulkan/Renderer_Vulkan.h"
#include <goop/Core.h>
#include <goop/sys/Renderer.h>
#include <goop/sys/Sfx.h>

goop::App* goop::createEditor(int argc, char** argv, App* game) { return new EditorApp(game); }

void EditorApp::init()
{
	game->init();
}

void EditorApp::update(float dt)
{
	game->update(dt);
}

void EditorApp::gui()
{
	// build dockspace
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

	ImGui::Begin("Dockspace", nullptr, windowFlags);
	ImGui::PopStyleVar(2);
	ImGuiID dockspaceID = ImGui::GetID("Dockspace");
	ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), dockspaceFlags);
	ImGui::End();

	auto r = goop::sys::gRenderer.get();
	ImGui::Begin("Viewport");
	ImVec2 max = ImGui::GetWindowContentRegionMax();
	ImVec2 min = ImGui::GetWindowContentRegionMin();
	max.x += ImGui::GetWindowPos().x;
	max.y += ImGui::GetWindowPos().y;
	min.x += ImGui::GetWindowPos().x;
	min.y += ImGui::GetWindowPos().y;
	viewportSize = {max.x - min.x, max.y - min.y};
	ImGui::Image(r->getViewTexture(), viewportSize);
	ImGui::End();

	game->gui();
}
