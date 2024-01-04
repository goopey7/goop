// Sam Collier 2023

#include "Scene.h"
#include "components/CustomComponents.h"
#include "goop/Components.h"
#include "goop/Entity.h"
#include <glm/common.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <goop/Core.h>

using namespace goop;

Entity Scene::createEntity(const std::string& tag)
{
	Entity e = Entity(registry.create(), this);
	e.addComponent<TagComponent>(tag);
	return e;
}

std::optional<Entity> Scene::getEntity(const std::string& tag)
{
	auto view = registry.view<TagComponent>();
	for (auto entity : view)
	{
		if (view.get<TagComponent>(entity).tag == tag)
			return Entity(entity, this);
	}

	return std::nullopt;
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
				if (component["primitive"].is_null())
				{
					e.addComponent<goop::MeshComponent>(component["path"],
														component["texturePath"]);
				}
				else if (component["primitive"] == "box")
				{
					e.addComponent<goop::MeshComponent>(goop::Box(), component["texturePath"],
														component["path"]);
				}
			}
			else if (component["type"] == "rigidBody")
			{
				json& box = component["box"];
				glm::vec3 size = {box["x"], box["y"], box["z"]};
				auto& rb = e.addComponent<goop::RigidbodyComponent>(component["mass"], size);
				rb.entity = e;
			}
			else if (component["type"] == "camera")
			{
				glm::vec3 position = {component["position"]["x"], component["position"]["y"],
									  component["position"]["z"]};
				glm::vec3 rotation = {component["rotation"]["x"], component["rotation"]["y"],
									  component["rotation"]["z"]};
				e.addComponent<goop::CameraComponent>(position, rotation, component["active"]);
			}
			else if (customComponentFactoryMap.contains(component["type"]))
			{
				addCustomComponent(component["type"], e, this);
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
			auto& tc = e.getComponent<TransformComponent>();

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
			auto& mc = e.getComponent<MeshComponent>();
			meshJson["path"] = mc.path;
			meshJson["texturePath"] = mc.texturePath;
			if (std::holds_alternative<goop::Box>(mc.primitive))
			{
				meshJson["primitive"] = "box";
			}
			eJson["components"].push_back(meshJson);
		}
		if (e.hasComponent<RigidbodyComponent>())
		{
			json rbJson;
			rbJson["type"] = "rigidBody";
			auto& rbc = e.getComponent<RigidbodyComponent>();
			rbJson["mass"] = rbc.mass;
			rbJson["box"]["x"] = rbc.box[0];
			rbJson["box"]["y"] = rbc.box[1];
			rbJson["box"]["z"] = rbc.box[2];
			eJson["components"].push_back(rbJson);
		}
		if (e.hasComponent<CameraComponent>())
		{
			json camJson;
			camJson["type"] = "camera";
			auto& cc = e.getComponent<CameraComponent>();
			camJson["position"]["x"] = cc.position.x;
			camJson["position"]["y"] = cc.position.y;
			camJson["position"]["z"] = cc.position.z;
			camJson["rotation"]["x"] = cc.rotation.x;
			camJson["rotation"]["y"] = cc.rotation.y;
			camJson["rotation"]["z"] = cc.rotation.z;
			camJson["active"] = cc.active;
			eJson["components"].push_back(camJson);
		}

		saveCustomComponents(this, e, eJson["components"]);

		sceneJson["entities"].push_back(eJson);
	}

	return sceneJson;
}

void Scene::destroyEntity(Entity entity)
{
	if (entity.hasComponent<MeshComponent>())
	{
		goop::rm->unloadMesh(&entity.getComponent<MeshComponent>());
		goop::rm->unloadTexture(&entity.getComponent<MeshComponent>());
	}
	if (entity.hasComponent<RigidbodyComponent>())
	{
		sys::gPhysics->removeRigidBody(&entity.getComponent<RigidbodyComponent>());
	}
	registry.destroy((entt::entity)entity.getUID());
}

#ifdef GOOP_APPTYPE_EDITOR
void Scene::resetScene()
{
	if (sceneJson["entities"].is_null())
	{
		return;
	}

	for (json& entity : sceneJson["entities"])
	{
		std::string name = entity["name"];
		auto eOpt = getEntity(name);
		if (!eOpt.has_value())
		{
			continue;
		}

		goop::Entity e = eOpt.value();

		for (json& component : entity["components"])
		{
			std::string type = component["type"];
			if (type == "transform")
			{
				auto& tc = e.getComponent<goop::TransformComponent>();
				tc.position.x = component["position"]["x"];
				tc.position.y = component["position"]["y"];
				tc.position.z = component["position"]["z"];

				tc.rotation.x = component["rotation"]["x"];
				tc.rotation.y = component["rotation"]["y"];
				tc.rotation.z = component["rotation"]["z"];

				tc.scale.x = component["scale"]["x"];
				tc.scale.y = component["scale"]["y"];
				tc.scale.z = component["scale"]["z"];
			}
		}
	}
}
#endif
