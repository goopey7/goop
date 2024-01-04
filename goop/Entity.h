// Sam Collier 2023
#pragma once
#include "Scene.h"
#include <memory>

namespace goop
{
class Entity
{
  public:
	Entity(entt::entity entity, Scene* scene) : entity(entity), scene(scene) {}

	template <typename T, typename... Args> T& addComponent(Args&&... args) const
	{
		return scene->registry.emplace<T>(entity, std::forward<Args>(args)...);
	}

	template <typename T> T& getComponent() const { return scene->registry.get<T>(entity); }

	template <typename T> bool hasComponent() const { return scene->registry.any_of<T>(entity); }

	template <typename T> void removeComponent() { scene->registry.remove<T>(entity); }

	uint32_t getUID() { return (uint32_t)entity; }
	entt::entity getEntity() const { return entity; }

	Scene* getScene() const { return scene; }

  private:
	entt::entity entity;

	// TODO entities shouldn't own the scene
	// weak_ptr?
	Scene* scene;
};
} // namespace goop
