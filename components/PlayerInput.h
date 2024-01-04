#pragma once
#include <goop/Components.h>

class PlayerInput : public goop::CustomComponent
{
public:
    PlayerInput(goop::Entity e) : goop::CustomComponent(e) {name = "PlayerInput";}
    void init();
    void update(float dt);
    void gui();

private:
	void onCollisionEnter(goop::Entity other) final;
	void onCollisionExit(goop::Entity other) final;

	glm::vec3 velocity;
	float walkSpeed = 2.f;
};
