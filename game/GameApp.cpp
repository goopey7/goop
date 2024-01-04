#include "GameApp.h"
#include "goop/Components.h"
#include "goop/Entity.h"
#include <imgui.h>
#include <iostream>

#include <goop/Core.h>
#include <goop/sys/Renderer.h>
#include <goop/sys/Sfx.h>
#include <json.hpp>
#include "components/CustomComponents.h"
using json = nlohmann::json;

goop::App* goop::createGame(int argc, char** argv, Scene* scene) { return new GameApp(scene); }

void GameApp::init()
{
	std::cout << "Hello from GameApp!" << std::endl;

	auto cfgOpt = loadJson("cfg.json");
	if (!cfgOpt.has_value())
	{
		std::cout << "Failed to load cfg.json" << std::endl;
		throw std::runtime_error("Failed to load cfg.json");
	}
	json cfg = cfgOpt.value();

	auto lvlOpt = loadJson(cfg["game_file"]);
	if (!lvlOpt.has_value())
	{
		std::cout << "Failed to load " << cfg["game_file"] << std::endl;
		throw std::runtime_error("Failed to load " + cfg["game_file"].get<std::string>());
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

	/* TODO -------------
	   - store EVERYTHING contiguously - decide on a data structure - (week 7)
	   - dynamically generate a header file for resource enums

	   - End user should not worry about loading / unloading resources
			- should be abstracted once ECS is in -- but sfx??
   */

	goop::Camera* cam = new goop::Camera();
	cam->setPosition(glm::vec3(0.f, 0.f, 5.f));
	scene->setCurrentCamera(cam);

#ifndef GOOP_APPTYPE_EDITOR
	initCustomComponents(scene);
	goop::sys::gWindow->grabCursor(true);
#endif
}

void GameApp::update(float dt)
{
	goop::sys::gPhysics->simulate(dt);
	updateCustomComponents(scene, dt);

	auto view = scene->view<goop::CameraComponent>();
	for (auto entity : view)
	{
		auto e = goop::Entity(entity, scene);
		auto& cc = e.getComponent<goop::CameraComponent>();
		if (cc.active)
		{
			auto& tc = e.getComponent<goop::TransformComponent>();
			goop::Camera* cam = scene->getCurrentCamera();
			cam->setPosition(tc.position + cc.position);
			cam->setRotation(cc.rotation + cc.rotation);
		}
	}
}
