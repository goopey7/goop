#include "EditorApp.h"
#include <imgui.h>
#include <iostream>

#include <goop/Core.h>
#include <goop/Entity.h>
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
		file << cfg.dump(2);
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

		std::ofstream file(cfg["game_file"].get<std::string>().c_str());
		file << lvl.dump(2);
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

	// Menu bar (top of window)
	ImGui::BeginMainMenuBar();
	if (ImGui::Button(shouldPlay ? "Pause" : "Play"))
	{
		shouldPlay = !shouldPlay;
	}
	if (ImGui::Button("Save"))
	{
		json sceneJson = scene->saveScene();
		json cfg = loadJson("cfg.json").value();
		json lvl = loadJson(cfg["game_file"]).value();
		for (json& sceneObj : lvl["scenes"])
		{
			if (sceneObj["name"] == scene->getScene()["name"])
			{
				sceneObj = sceneJson;
				break;
			}
		}
		std::ofstream file(cfg["game_file"].get<std::string>().c_str());
		file << lvl.dump(2);
		file.close();
	}
	ImGui::Text("%s", scene->getScene()["name"].get<std::string>().c_str());
	ImGui::EndMainMenuBar();

	// Game Viewport
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

	// Scene View
	ImGui::Begin("Scene");
	ImGui::Text("Scene View");
	if (ImGui::Button("Add Entity"))
	{
		addEntityPopupOpen = true;
		ImVec2 centerPos = ImVec2(viewport->WorkPos.x + viewport->WorkSize.x * 0.5f,
								  viewport->WorkPos.y + viewport->WorkSize.y * 0.5f);
		ImGui::SetNextWindowPos(centerPos, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
	}

	if (addEntityPopupOpen)
	{
		ImGui::OpenPopup("AddEntity");

		ImVec2 centerPos = ImVec2(viewport->WorkPos.x + viewport->WorkSize.x * 0.5f,
								  viewport->WorkPos.y + viewport->WorkSize.y * 0.5f);

		ImVec2 popupSize = ImVec2(200, 100); // Adjust the size if needed
		ImVec2 popupPos =
			ImVec2(centerPos.x - popupSize.x * 0.5f, centerPos.y - popupSize.y * 0.5f);
		ImGui::SetNextWindowPos(popupPos, ImGuiCond_Always);
		ImGui::SetNextWindowSize(popupSize, ImGuiCond_Always);
	}

	// AddEntity Popup
	if (ImGui::BeginPopup("AddEntity"))
	{
		ImGui::Text("Add Entity");
		ImGui::InputText("Name", entityName, 256);
		if (ImGui::Button("Add"))
		{
			auto e = scene->createEntity(entityName);
			e.addComponent<goop::TransformComponent>();
			ImGui::CloseCurrentPopup();
			addEntityPopupOpen = false;
		}
		if (ImGui::Button("Cancel"))
		{
			ImGui::CloseCurrentPopup();
			addEntityPopupOpen = false;
		}
		ImGui::EndPopup();
	}

	auto view = scene->view<goop::TagComponent>();
	for (auto entity : view)
	{
		auto e = goop::Entity(entity, scene);
		auto& tag = view.get<goop::TagComponent>(entity).tag;
		if (ImGui::Selectable(tag.c_str(), selectedEntity.has_value() &&
											   selectedEntity.value().getUID() == (uint32_t)entity))
		{
			selectedEntity = e;
		}
	}
	ImGui::End();

	// Inspector
	ImGui::Begin("Inspector");
	ImGui::Text("%s", !selectedEntity.has_value()
						  ? "No Entity Selected"
						  : selectedEntity.value().getComponent<goop::TagComponent>().tag.c_str());
	if (selectedEntity.has_value())
	{
		auto e = selectedEntity.value();
		if (ImGui::Button("Delete Entity"))
		{
			scene->destroyEntity(e);
			addEntityPopupOpen = false;
			addComponentPopupOpen = false;
			selectedEntity = std::nullopt;
			ImGui::End();
			return;
		}
		if (e.hasComponent<goop::TransformComponent>())
		{
			ImGui::Text("Transform");
			auto& transform = e.getComponent<goop::TransformComponent>().transform;
			ImGui::DragFloat3("Position", &transform[3][0], 0.1f);
		}
		if (e.hasComponent<goop::MeshComponent>())
		{
			ImGui::Text("Mesh");
			auto& mesh = e.getComponent<goop::MeshComponent>();
			ImGui::Text("%s", mesh.path.c_str());
			if (ImGui::Button("Change Mesh"))
			{
				changeMeshPopupOpen = true;
				ImVec2 centerPos = ImVec2(viewport->WorkPos.x + viewport->WorkSize.x * 0.5f,
										  viewport->WorkPos.y + viewport->WorkSize.y * 0.5f);
				ImGui::SetNextWindowPos(centerPos, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
			}
			if (changeMeshPopupOpen)
			{
				ImGui::OpenPopup("ChangeMesh");

				ImVec2 centerPos = ImVec2(viewport->WorkPos.x + viewport->WorkSize.x * 0.5f,
										  viewport->WorkPos.y + viewport->WorkSize.y * 0.5f);

				ImVec2 popupSize = ImVec2(200, 100); // Adjust the size if needed
				ImVec2 popupPos =
					ImVec2(centerPos.x - popupSize.x * 0.5f, centerPos.y - popupSize.y * 0.5f);
				ImGui::SetNextWindowPos(popupPos, ImGuiCond_Always);
				ImGui::SetNextWindowSize(popupSize, ImGuiCond_Always);
			}
			if (ImGui::BeginPopup("ChangeMesh"))
			{
				ImGui::Text("Change Mesh");
				ImGui::InputText("Path", mesh.path.data(), 256);
				if (ImGui::Button("Change"))
				{
					goop::rm->loadMesh(mesh);
					ImGui::CloseCurrentPopup();
					changeMeshPopupOpen = false;
				}
				if (ImGui::Button("Cancel"))
				{
					ImGui::CloseCurrentPopup();
					changeMeshPopupOpen = false;
				}
				ImGui::EndPopup();
			}
		}
		if (!e.hasComponent<goop::TransformComponent>() ||
			!e.hasComponent<goop::MeshComponent>() && ImGui::Button("Add Component"))
		{
			addComponentPopupOpen = true;
		}
		if (addComponentPopupOpen)
		{
			ImGui::OpenPopup("AddComponent");

			ImVec2 centerPos = ImVec2(viewport->WorkPos.x + viewport->WorkSize.x * 0.5f,
									  viewport->WorkPos.y + viewport->WorkSize.y * 0.5f);

			ImVec2 popupSize = ImVec2(200, 100); // Adjust the size if needed
			ImVec2 popupPos =
				ImVec2(centerPos.x - popupSize.x * 0.5f, centerPos.y - popupSize.y * 0.5f);
			ImGui::SetNextWindowPos(popupPos, ImGuiCond_Always);
			ImGui::SetNextWindowSize(popupSize, ImGuiCond_Always);
		}

		// AddComponent Popup
		if (ImGui::BeginPopup("AddComponent"))
		{
			ImGui::Text("Add Component");
			if (!e.hasComponent<goop::TransformComponent>() && ImGui::Button("Transform"))
			{
				e.addComponent<goop::TransformComponent>();
				ImGui::CloseCurrentPopup();
				addComponentPopupOpen = false;
			}
			if (!e.hasComponent<goop::MeshComponent>() && ImGui::Button("Mesh"))
			{
				e.addComponent<goop::MeshComponent>("res/viking_room.obj");
				goop::rm->loadMesh(e.getComponent<goop::MeshComponent>());
				ImGui::CloseCurrentPopup();
				addComponentPopupOpen = false;
			}
			if (ImGui::Button("Cancel"))
			{
				ImGui::CloseCurrentPopup();
				addComponentPopupOpen = false;
			}
			ImGui::EndPopup();
		}
	}
	ImGui::End();

	game->gui();
}
