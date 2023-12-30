#include "GameApp.h"
#include "goop/Components.h"
#include "goop/Entity.h"
#include <imgui.h>
#include <iostream>

#include <goop/Core.h>
#include <goop/sys/Renderer.h>
#include <goop/sys/Sfx.h>
#include <json.hpp>
using json = nlohmann::json;

goop::App* goop::createGame(int argc, char** argv, Scene* scene) { return new GameApp(scene); }

void GameApp::init()
{
	std::cout << "Hello from GameApp!" << std::endl;

	// goop::rm->loadSfx("res/blast.mp3");
	//  goop::rm->playSfx(0);

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
		json lvl = {{
			"scenes",
			{
				{"name", "lvl_1"},
			},
		}};

		std::ofstream file(cfg["game_file"]);
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

	/* TODO -------------
	   - store EVERYTHING contiguously - decide on a data structure - (week 7)
	   - dynamically generate a header file for resource enums

	   - End user should not worry about loading / unloading resources
			- should be abstracted once ECS is in -- but sfx??
   */
}

void GameApp::update(float dt) {}
