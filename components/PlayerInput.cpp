#include "PlayerInput.h"
#include "goop/Core.h"
#include <GLFW/glfw3.h>
#include <goop/Input.h>
#include <goop/Physics.h>
#include <imgui.h>
#include <iostream>

// Gets called when the game starts
void PlayerInput::init() { CustomComponent::init(); }

// Gets called every frame
void PlayerInput::update(float dt)
{
	goop::grabCursor(true);
	float mouseDX = goop::getMouseDeltaX();
	float mouseDY = goop::getMouseDeltaY();
	auto& cam = getComponent<goop::CameraComponent>();
	cam.rotation.x -= mouseDY * dt * 5.f;
	cam.rotation.y += mouseDX * dt * 5.f;

	if (goop::isKeyPressed(ImGuiKey_Space))
	{
		auto& rb = getComponent<goop::RigidbodyComponent>();
		goop::applyImpulse(rb, glm::vec3(0, 5.f, 0));
	}

	if (goop::isKeyDown(ImGuiKey_W))
	{
		velocity.z = -1.f;
	}
	if (goop::isKeyDown(ImGuiKey_S))
	{
		velocity.z = 1.f;
	}
	if (!goop::isKeyDown(ImGuiKey_W) && !goop::isKeyDown(ImGuiKey_S))
	{
		velocity.z = 0.f;
	}

	if (goop::isKeyDown(ImGuiKey_A))
	{
		velocity.x = -1.f;
	}
	if (goop::isKeyDown(ImGuiKey_D))
	{
		velocity.x = 1.f;
	}
	if (!goop::isKeyDown(ImGuiKey_A) && !goop::isKeyDown(ImGuiKey_D))
	{
		velocity.x = 0.f;
	}

	if (glm::length(velocity) > 0)
	{
		velocity = glm::normalize(velocity);
	}
	goop::setVelocity(entity, velocity * walkSpeed, true);

	if (goop::isKeyPressed(ImGuiKey_F))
	{
		auto e = spawnEntity();
		auto& tc = e.addComponent<goop::TransformComponent>();
		tc.position = e.getScene()->getCurrentCamera()->getPosition() +
					  e.getScene()->getCurrentCamera()->getForward() * 2.f;
		tc.scale = glm::vec3(0.5f);
		auto& rb = e.addComponent<goop::RigidbodyComponent>(1.f, glm::vec3(1.f));
		rb.box = tc.scale;
		goop::sys::gPhysics->addRigidBody(&e.getComponent<goop::RigidbodyComponent>(),
										  &e.getComponent<goop::TransformComponent>());
		auto& mc = e.addComponent<goop::MeshComponent>(goop::Box(), "res/texture.jpg", "box");
		goop::rm->loadMesh(&mc);
		goop::rm->loadTexture(&mc);
	}
}

// Collision callbacks
void PlayerInput::onCollisionEnter(goop::Entity other)
{
	std::cout << "Collision enter with entity " << other.getUID() << std::endl;
}

void PlayerInput::onCollisionExit(goop::Entity other)
{
	//...
}

// Editor GUI - Shown in inspector view
// Refer to ImGui documentation for more info
void PlayerInput::gui()
{
	//...
}
