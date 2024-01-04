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
	tc.position.x = cosf(timer);
	tc.position.y = sinf(timer);
}

// Editor GUI - Shown in inspector view
void Shoutout::gui()
{
    //...
}
