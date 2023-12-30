#include "EditorApp.h"
#include <imgui.h>
#include <iostream>

#include <goop/Core.h>
#include <goop/sys/Renderer.h>
#include <goop/sys/Sfx.h>

#include <json.hpp>
using json = nlohmann::json;

goop::App* goop::createEditor(int argc, char** argv, App* game, goop::Scene* scene)
{
	return new EditorApp(game, scene);
}

void EditorApp::init() 
{
	auto cfgOpt = loadJson("cfg.json");
	if (!cfgOpt.has_value())
	{
		std::cout << "Failed to load cfg.json" << std::endl;

		// create a default cfg.json
		json cfg = {
			{"game_file", "lvl.json"},
			{"start_scene", "lvl_1"},
		};

		std::ofstream file("cfg.json");
		file << cfg;
		file.close();

		cfgOpt = loadJson("cfg.json");
	}

	json cfg = cfgOpt.value();

	auto lvlOpt = loadJson(cfg["game_file"]);
	if (!lvlOpt.has_value())
	{
		std::cout << "Failed to load " << cfg["game_file"] << std::endl;

		// create a default game file
		json lvl;
		json sceneObj = {{"name", "lvl_1"}};
		json scenesArr = {sceneObj};

		lvl["scenes"] = scenesArr;

		std::string gameFile = cfg["game_file"];
		std::ofstream file(gameFile.c_str());
		file << lvl;
		file.close();

		lvlOpt = loadJson(cfg["game_file"]);
	}
	json lvl = lvlOpt.value();

	json startScene;

	// find starting scene
	for (json& scene : lvl["scenes"])
	{
		if (scene["name"] == cfg["start_scene"])
		{
			startScene = scene;
			break;
		}
	}

	scene->loadScene(startScene);
}

void EditorApp::update(float dt)
{
	if (shouldPlay)
	{
		game->update(dt);
	}
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

	ImGui::BeginMainMenuBar();
	if (ImGui::Button(shouldPlay ? "Pause" : "Play"))
	{
		shouldPlay = !shouldPlay;
	}
	ImGui::EndMainMenuBar();

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
