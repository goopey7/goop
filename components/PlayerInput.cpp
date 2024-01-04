#include "PlayerInput.h"
#include <goop/Input.h>
#include <imgui.h>
#include <GLFW/glfw3.h>
#include <goop/Physics.h>
#include <iostream>

// Gets called when the game starts
void PlayerInput::init()
{
    //...
}

// Gets called every frame
void PlayerInput::update(float dt)
{
	goop::grabCursor(true);
	float mouseDX = goop::getMouseDeltaX();
	float mouseDY = goop::getMouseDeltaY();
	auto& cam = getComponent<goop::CameraComponent>();
	cam.rotation.x -= mouseDY * dt * 5.f;
	cam.rotation.y += mouseDX * dt * 5.f;

	// jump
	if (goop::isKeyPressed(ImGuiKey_Space))
	{
		auto& rb = getComponent<goop::RigidbodyComponent>();
		goop::applyImpulse(rb, glm::vec3(0, 5.f, 0));
		std::cout<<"pressed space"<<std::endl;
	}
	if (goop::isKeyReleased(ImGuiKey_Space))
	{
		std::cout<<"released space"<<std::endl;
	}
}

// Editor GUI - Shown in inspector view
// Refer to ImGui documentation for more info
void PlayerInput::gui()
{
	// ImGui::Text("Hello from %s", name.c_str());
    //...
}
