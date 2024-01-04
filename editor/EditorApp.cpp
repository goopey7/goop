#include "EditorApp.h"
#include <glm/gtx/matrix_decompose.hpp>
#include <goop/Input.h>
#include <imgui.h>
#include <iostream>

#include <goop/Core.h>
#include <goop/Entity.h>
#include <goop/sys/Renderer.h>
#include <goop/sys/Sfx.h>

#include <GLFW/glfw3.h>
#include <json.hpp>

#include "components/CustomComponents.h"

using json = nlohmann::json;

goop::App* goop::createEditor(int argc, char** argv, App* game, goop::Scene* scene)
{
	return new EditorApp(game, scene);
}

void EditorApp::init()
{
	game->init();

	// scan components directory
	for (const auto& entry : std::filesystem::directory_iterator("components"))
	{
		if (entry.path().extension() == ".cpp")
		{
			std::string name = entry.path().filename().string();
			name = name.substr(0, name.size() - 4);
			customComponentNames.push_back(name);
		}
	}
}

void EditorApp::update(float dt)
{
	if (shouldPlay)
	{
		game->update(dt);
	}
	else
	{
		goop::Camera* cam = scene->getCurrentCamera();

		if (goop::isLMBDown() || goop::isRMBDown())
		{
			goop::hideCursor(true);
			if (goop::isKeyDown(GLFW_KEY_W))
			{
				cam->translate(cam->getForward() * dt * 5.f);
			}
			if (goop::isKeyDown(GLFW_KEY_S))
			{
				cam->translate(-cam->getForward() * dt * 5.f);
			}
			if (goop::isKeyDown(GLFW_KEY_A))
			{
				cam->translate(-cam->getRight() * dt * 5.f);
			}
			if (goop::isKeyDown(GLFW_KEY_D))
			{
				cam->translate(cam->getRight() * dt * 5.f);
			}
			if (goop::isKeyDown(GLFW_KEY_E))
			{
				cam->translate(cam->getUp() * dt * 5.f);
			}
			if (goop::isKeyDown(GLFW_KEY_Q))
			{
				cam->translate(-cam->getUp() * dt * 5.f);
			}

			float dx = goop::getMouseDeltaX();
			float dy = goop::getMouseDeltaY();
			cam->rotate(glm::vec3(-dy, dx, 0.f) * dt * 5.f);
		}
		else
		{
			goop::hideCursor(false);
		}
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
		if (!shouldPlay)
		{
			goop::sys::gPhysics->destroy();
			scene->resetScene();
		}
		else
		{
			goop::sys::gPhysics->initialize();
			auto rbView = scene->view<goop::RigidbodyComponent>();
			auto tcView = scene->view<goop::TransformComponent>();
			for (auto entity : rbView)
			{
				goop::RigidbodyComponent* rbc = &rbView.get<goop::RigidbodyComponent>(entity);
				goop::TransformComponent* tc = &tcView.get<goop::TransformComponent>(entity);
				goop::sys::gPhysics->addRigidBody(rbc, tc);
			}
		}
	}
	if (!shouldPlay && ImGui::Button("Save"))
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
			auto& tc = e.getComponent<goop::TransformComponent>();

			ImGui::DragFloat3("Position", &tc.position[0], 0.1f);
			ImGui::DragFloat3("Rotation", &tc.rotation[0], 0.1f);
			ImGui::DragFloat3("Scale", &tc.scale[0], 0.1f);
		}
		if (e.hasComponent<goop::RigidbodyComponent>())
		{
			ImGui::Text("Rigidbody");
			auto& rbc = e.getComponent<goop::RigidbodyComponent>();
			ImGui::DragFloat("Mass", &rbc.mass, 0.1f);
			ImGui::DragFloat3("Size", &rbc.box[0], 0.1f);
		}
		if (e.hasComponent<goop::MeshComponent>())
		{
			ImGui::Text("Mesh");
			auto& mesh = e.getComponent<goop::MeshComponent>();
			ImGui::Text("Mesh: %s", mesh.path.c_str());
			ImGui::Text("Texture: %s", mesh.texturePath.c_str());
			if (ImGui::Button("Change Texture"))
			{
				changeTexturePopupOpen = true;
				ImVec2 centerPos = ImVec2(viewport->WorkPos.x + viewport->WorkSize.x * 0.5f,
										  viewport->WorkPos.y + viewport->WorkSize.y * 0.5f);
				ImGui::SetNextWindowPos(centerPos, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
			}
			if (changeTexturePopupOpen)
			{
				ImGui::OpenPopup("ChangeTexture");

				ImVec2 centerPos = ImVec2(viewport->WorkPos.x + viewport->WorkSize.x * 0.5f,
										  viewport->WorkPos.y + viewport->WorkSize.y * 0.5f);

				ImVec2 popupSize = ImVec2(200, 100); // Adjust the size if needed
				ImVec2 popupPos =
					ImVec2(centerPos.x - popupSize.x * 0.5f, centerPos.y - popupSize.y * 0.5f);
				ImGui::SetNextWindowPos(popupPos, ImGuiCond_Always);
				ImGui::SetNextWindowSize(popupSize, ImGuiCond_Always);
			}
			if (ImGui::BeginPopup("ChangeTexture"))
			{
				ImGui::Text("Change Texture");
				if (oldTexturePath.empty())
				{
					oldTexturePath = mesh.texturePath;
				}
				ImGui::InputText("Path", texturePath, 256);
				if (ImGui::Button("Change"))
				{
					mesh.texturePath = std::string(texturePath);
					goop::rm->loadTexture(mesh, oldTexturePath.c_str());
					oldTexturePath = "";
					std::memset(texturePath, 0, 256);
					ImGui::CloseCurrentPopup();
					changeTexturePopupOpen = false;
				}
				if (ImGui::Button("Cancel"))
				{
					ImGui::CloseCurrentPopup();
					changeTexturePopupOpen = false;
				}
				ImGui::EndPopup();
			}
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
				if (oldMeshPath.empty())
				{
					oldMeshPath = mesh.path;
				}
				ImGui::InputText("Path", meshPath, 256);
				if (ImGui::Button("Change"))
				{
					mesh.path = std::string(meshPath);
					if (oldMeshPath == "box")
					{
						mesh.primitive = std::monostate{};
					}
					goop::rm->loadMesh(mesh, oldMeshPath.c_str());
					oldMeshPath = "";
					std::memset(meshPath, 0, 256);
					ImGui::CloseCurrentPopup();
					changeMeshPopupOpen = false;
				}
				if (ImGui::Button("Box"))
				{
					mesh.primitive = goop::Box{};
					mesh.path = "box";
					goop::rm->loadMesh(mesh, oldMeshPath.c_str());
					oldMeshPath = "";
					std::memset(meshPath, 0, 256);
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
		guiCustomComponents(scene);
		if (ImGui::Button("Add Component"))
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
				e.addComponent<goop::MeshComponent>("res/viking_room.obj", "res/viking_room.png");
				goop::rm->loadMesh(e.getComponent<goop::MeshComponent>());
				goop::rm->loadTexture(e.getComponent<goop::MeshComponent>());
				ImGui::CloseCurrentPopup();
				addComponentPopupOpen = false;
			}
			if (!e.hasComponent<goop::RigidbodyComponent>() && ImGui::Button("Rigidbody"))
			{
				e.addComponent<goop::RigidbodyComponent>();
				if (goop::sys::gPhysics->isInitialized())
				{
					goop::sys::gPhysics->addRigidBody(&e.getComponent<goop::RigidbodyComponent>(),
													  &e.getComponent<goop::TransformComponent>());
				}
				ImGui::CloseCurrentPopup();
				addComponentPopupOpen = false;
			}
			for (const auto& name : customComponentNames)
			{
				if (ImGui::Button(name.c_str()))
				{
					if (customComponentFactoryMap.find(name) == customComponentFactoryMap.end())
					{
						throw std::runtime_error("Custom component " + name + " not found");
					}
					addCustomComponent(name, e, scene);
					ImGui::CloseCurrentPopup();
					addComponentPopupOpen = false;
				}
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
