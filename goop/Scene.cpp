// Sam Collier 2023

#include "Scene.h"
#include "goop/Components.h"
#include "goop/Entity.h"

using namespace goop;

Scene::Scene() {}

Scene::~Scene() {}

Entity Scene::createEntity(const std::string& tag)
{
	Entity e = Entity(registry.create(), this);
	e.addComponent<TransformComponent>();
	e.addComponent<TagComponent>(tag);
	return e;
}

Entity Scene::getEntity(const std::string& tag)
{
	auto view = registry.view<TagComponent>();
	for (auto entity : view)
	{
		if (view.get<TagComponent>(entity).tag == tag)
			return Entity(entity, this);
	}

	return Entity(entt::null, this);
}
