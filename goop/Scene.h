// Sam Collier 2023
#pragma once

#include "goop/Camera.h"
#include <entt.hpp>

#include <json.hpp>
#include <queue>
using json = nlohmann::json;

namespace goop
{
class Entity;
class Scene
{
  public:
	Entity createEntity(const std::string& tag = "Entity");
	void destroyEntity(Entity entity);
	void loadScene();
	nlohmann::json getScene() const;
	nlohmann::json saveScene();

	template <typename T> auto view() { return registry.view<T>(); }

	std::optional<Entity> getEntity(const std::string& tag);

	bool hasEntity(entt::entity id) const { return registry.valid(id); }

	void setCurrentCamera(Camera* camera) { currentCamera = camera; }
	Camera* getCurrentCamera() const { return currentCamera; }

	void addSpawnedEntity(entt::entity entity) { spawnedEntities.push_back(entity); }

	void resetScene();
	void clearScene();
	void nextScene()
	{
		if (sceneQueue.empty())
		{
			return;
		}
		clearScene();
		sceneQueue.pop();
		loadScene();
	}
	void queueScene(nlohmann::json scene)
	{
		sceneQueue.push(scene);
		loadedScenes.emplace(scene);
	}
	void clearQueue()
	{
		json currentScene = sceneQueue.front();
		sceneQueue = std::queue<nlohmann::json>();
		sceneQueue.push(currentScene);
	}
	const std::set<json>& getLoadedScenes() const { return loadedScenes; }

  private:
	entt::registry registry;
	friend class Entity;

	Camera* currentCamera = nullptr;

	std::vector<entt::entity> spawnedEntities;

	std::queue<nlohmann::json> sceneQueue;
	std::set<json> loadedScenes;
};
} // namespace goop
