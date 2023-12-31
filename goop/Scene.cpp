// Sam Collier 2023

#include "Scene.h"
#include "goop/Components.h"
#include "goop/Entity.h"
#include <glm/common.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <goop/Core.h>

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
				glm::vec3 position;
				glm::vec3 rotation;
				glm::vec3 scale;

				position.x = component["position"]["x"];
				position.y = component["position"]["y"];
				position.z = component["position"]["z"];

				rotation.x = component["rotation"]["x"];
				rotation.y = component["rotation"]["y"];
				rotation.z = component["rotation"]["z"];

				scale.x = component["scale"]["x"];
				scale.y = component["scale"]["y"];
				scale.z = component["scale"]["z"];

				e.addComponent<goop::TransformComponent>(position, rotation, scale);
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
			auto tc = e.getComponent<TransformComponent>();

			transformJson["position"]["x"] = tc.position.x;
			transformJson["position"]["y"] = tc.position.y;
			transformJson["position"]["z"] = tc.position.z;

			transformJson["rotation"]["x"] = tc.rotation.x;
			transformJson["rotation"]["y"] = tc.rotation.y;
			transformJson["rotation"]["z"] = tc.rotation.z;

			transformJson["scale"]["x"] = tc.scale.x;
			transformJson["scale"]["y"] = tc.scale.y;
			transformJson["scale"]["z"] = tc.scale.z;

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
