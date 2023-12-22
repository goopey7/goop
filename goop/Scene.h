// Sam Collier 2023
#pragma once

#include <entt.hpp>

namespace goop
{
class Entity;
class Scene
{
  public:
	Scene();
	~Scene();

	Entity createEntity(const std::string& tag = "Entity");

  private:
	entt::registry registry;
	friend class Entity;
};
} // namespace goop
