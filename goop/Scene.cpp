// Sam Collier 2023

#include "Scene.h"
#include "goop/Components.h"
#include <goop/Core.h>
#include "goop/Entity.h"
#include <glm/common.hpp>
#include <glm/gtx/matrix_decompose.hpp>

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
	sceneJson = startScene;
	if (startScene["entities"].is_null())
	{
		return;
	}

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

nlohmann::json Scene::getScene() const { return sceneJson; }

nlohmann::json Scene::saveScene()
{
	sceneJson["entities"] = json::array();
	auto view = registry.view<TagComponent>();
	for (auto& entity : view)
	{
		auto e = goop::Entity(entity, this);
		json eJson;
		eJson["name"] = e.getComponent<TagComponent>().tag;
		eJson["components"] = json::array();
		if (e.hasComponent<TransformComponent>())
		{
			json transformJson;
			transformJson["type"] = "transform";
			glm::mat4 transform = e.getComponent<TransformComponent>().transform;
			glm::vec3 scale, skew, translation;
			glm::quat orientation;
			glm::vec4 perspective;
			glm::decompose(transform, scale, orientation, translation, skew, perspective);

			transformJson["position"]["x"] = translation.x;
			transformJson["position"]["y"] = translation.y;
			transformJson["position"]["z"] = translation.z;

			glm::vec3 euler = glm::eulerAngles(orientation);
			transformJson["rotation"]["x"] = glm::degrees(euler.x);
			transformJson["rotation"]["y"] = glm::degrees(euler.y);
			transformJson["rotation"]["z"] = glm::degrees(euler.z);

			transformJson["scale"]["x"] = scale.x;
			transformJson["scale"]["y"] = scale.y;
			transformJson["scale"]["z"] = scale.z;

			eJson["components"].push_back(transformJson);
		}
		if (e.hasComponent<MeshComponent>())
		{
			json meshJson;
			meshJson["type"] = "mesh";
			meshJson["path"] = e.getComponent<MeshComponent>().path;
			eJson["components"].push_back(meshJson);
		}

		sceneJson["entities"].push_back(eJson);
	}

	return sceneJson;
}

void Scene::destroyEntity(Entity entity)
{
	if (entity.hasComponent<MeshComponent>())
	{
		goop::rm->unloadMesh(entity.getComponent<MeshComponent>());
	}
	registry.destroy((entt::entity)entity.getUID());
}

