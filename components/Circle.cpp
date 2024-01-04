#include "Circle.h"

void Circle::init()
{
    // Implement initialization
}

void Circle::update(float dt)
{
	timer += dt;
	auto& tc = getComponent<goop::TransformComponent>();
	tc.position.x = cosf(timer);
	tc.position.y = sinf(timer);
}

void Circle::gui()
{
}

