// Sam Collier 2023
#pragma once

#include "goop/Camera.h"
#include <entt.hpp>

#include <json.hpp>
using json = nlohmann::json;

namespace goop
{
class Entity;
class Scene
{
  public:
	Entity createEntity(const std::string& tag = "Entity");
	void destroyEntity(Entity entity);
	void loadScene(nlohmann::json& scene);
	nlohmann::json getScene() const;
	nlohmann::json saveScene();

	template <typename T> auto view() { return registry.view<T>(); }

	std::optional<Entity> getEntity(const std::string& tag);

	bool hasEntity(entt::entity id) const { return registry.valid(id); }

	void setCurrentCamera(Camera* camera) { currentCamera = camera; }
	Camera* getCurrentCamera() const { return currentCamera; }

	void addSpawnedEntity(entt::entity entity) { spawnedEntities.push_back(entity); }

#ifdef GOOP_APPTYPE_EDITOR
	void resetScene();
#endif

  private:
	entt::registry registry;
	friend class Entity;

	nlohmann::json sceneJson;

	Camera* currentCamera = nullptr;

	std::vector<entt::entity> spawnedEntities;
};
} // namespace goop
