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
