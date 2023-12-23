// Sam Collier 2023
#pragma once

#include <entt.hpp>
#include "Components.h"

namespace goop
{
class Entity;
class Scene
{
  public:
	Scene();
	~Scene();

	Entity createEntity(const std::string& tag = "Entity");

	template <typename T>
	auto view() { return registry.view<T>(); }

  private:
	entt::registry registry;
	friend class Entity;
};
} // namespace goop
