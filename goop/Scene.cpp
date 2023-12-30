// Sam Collier 2023

#include "Scene.h"
#include "goop/Components.h"
#include "goop/Entity.h"
#include <glm/ext/matrix_transform.hpp>

using namespace goop;

Scene::Scene() {}

Scene::~Scene() {}

Entity Scene::createEntity(const std::string& tag)
{
	Entity e = Entity(registry.create(), this);
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

void Scene::loadScene(nlohmann::json& startScene)
{
	for (json& entity : startScene["entities"])
	{
		goop::Entity e = createEntity(entity["name"]);
		for (json& component : entity["components"])
		{
			if (component["type"] == "transform")
			{
				glm::mat4 transform = glm::mat4(1.0f);
				transform = glm::translate(transform, glm::vec3(component["position"]["x"],
																component["position"]["y"],
																component["position"]["z"]));
				transform = glm::rotate(transform, glm::vec1(component["rotation"]["x"]).x,
										glm::vec3(1.f, 0.f, 0.f));
				transform = glm::rotate(transform, glm::vec1(component["rotation"]["y"]).x,
										glm::vec3(0.f, 1.f, 0.f));
				transform = glm::rotate(transform, glm::vec1(component["rotation"]["z"]).x,
										glm::vec3(0.f, 0.f, 1.f));
				transform = glm::scale(transform,
									   glm::vec3(component["scale"]["x"], component["scale"]["y"],
												 component["scale"]["z"]));
				e.addComponent<goop::TransformComponent>(transform);
			}
			else if (component["type"] == "mesh")
			{
				e.addComponent<goop::MeshComponent>(component["path"]);
			}
		}
	}
}
