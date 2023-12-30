// Sam Collier 2023
#pragma once

#include "goop/Scene.h"
#include <fstream>
#ifdef GOOP_APPTYPE_EDITOR
#include <imgui.h>
#endif

#include <json.hpp>

namespace goop
{
class App
{
  public:
	App(Scene* scene) : scene(scene) {}
	virtual void init() = 0;
	virtual void update(float dt) = 0;
	virtual void gui() = 0;
	std::optional<nlohmann::json> loadJson(const std::string& path) const
	{
		std::ifstream file(path);
		if (!file.is_open())
		{
			return std::nullopt;
		}

		nlohmann::json j;
		try
		{
			file >> j;
		}
		catch (const std::exception& e)
		{
			return std::nullopt;
		}

		return j;
	}

#ifdef GOOP_APPTYPE_EDITOR
	ImVec2 getViewportSize() const { return viewportSize; }
#endif

  protected:
	goop::Scene* scene;
#ifdef GOOP_APPTYPE_EDITOR
	ImVec2 viewportSize;
#endif
};
#ifdef GOOP_APPTYPE_EDITOR
extern App* createEditor(int argc, char** argv, App* game, Scene* scene);
#endif
extern App* createGame(int argc, char** argv, Scene* scene);
} // namespace goop
