#include "Shoutout.h"
#include <iostream>

// Gets called when the game starts
void Shoutout::init()
{
	std::cout << "Shoutout::init()" << std::endl;
}

// Gets called every frame
void Shoutout::update(float dt)
{
	timer += dt;
	auto& tc = getComponent<goop::TransformComponent>();
	tc.position.x = sinf(timer);
	tc.position.z = cosf(timer);
}

// Editor GUI - Shown in inspector view
// Refer to ImGui documentation for more info
void Shoutout::gui()
{
	ImGui::Text("Hello from %s", name.c_str());
    //...
}
