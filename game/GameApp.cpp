#include "GameApp.h"
#include "goop/Components.h"
#include "goop/Entity.h"
#include <imgui.h>
#include <iostream>

#include <goop/Core.h>
#include <goop/sys/Renderer.h>
#include <goop/sys/Sfx.h>

goop::App* goop::createGame(int argc, char** argv, Scene* scene) { return new GameApp(scene); }

void GameApp::init()
{
	std::cout << "Hello from GameApp!" << std::endl;

	goop::rm->loadSfx("res/blast.mp3");
	// goop::rm->playSfx(0);

	// TODO dynamically load and unload meshes and handle instances etc
	scene->createEntity("Viking Room").addComponent<goop::MeshComponent>("res/viking_room.obj");
	scene->createEntity("Viking Room1").addComponent<goop::MeshComponent>("res/viking_room.obj");
	glm::mat4& transform =
		scene->getEntity("Viking Room1").getComponent<goop::TransformComponent>().transform;
	transform = glm::translate(transform, glm::vec3(0.0f, -2.0f, -2.0f));
	scene->createEntity("Cow").addComponent<goop::MeshComponent>("res/cow.obj");

	/* TODO -------------
	   - store EVERYTHING contiguously - decide on a data structure - (week 7)
	   - dynamically generate a header file for resource enums

	   - End user should not worry about loading / unloading resources
			- should be abstracted once ECS is in -- but sfx??
   */
}

void GameApp::update(float dt)
{
	timeElapsed += dt;
	glm::mat4& transform =
		scene->getEntity("Viking Room").getComponent<goop::TransformComponent>().transform;

	transform = glm::rotate(glm::mat4(1.f), timeElapsed, glm::vec3(0.0f, 1.0f, 0.0f));
}
