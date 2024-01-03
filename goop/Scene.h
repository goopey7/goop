// Sam Collier 2023
#pragma once

#include "Components.h"
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

#ifdef GOOP_APPTYPE_EDITOR
	void resetScene();
#endif

  private:
	entt::registry registry;
	friend class Entity;

	nlohmann::json sceneJson;

	Camera* currentCamera = nullptr;
};
} // namespace goop
