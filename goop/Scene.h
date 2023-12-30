// Sam Collier 2023
#pragma once

#include <entt.hpp>
#include "Components.h"

#include <json.hpp>
using json = nlohmann::json;

namespace goop
{
class Entity;
class Scene
{
  public:
	Scene();
	~Scene();

	Entity createEntity(const std::string& tag = "Entity");
	void loadScene(nlohmann::json& scene);
	nlohmann::json getScene() const;
	nlohmann::json saveScene();

	template <typename T>
	auto view() { return registry.view<T>(); }

	Entity getEntity(const std::string& tag);

  private:
	entt::registry registry;
	friend class Entity;

	nlohmann::json sceneJson;
};
} // namespace goop
