#include "TestCustomComp.h"

void TestCustomComp::init()
{
}

void TestCustomComp::update(float dt)
{
	timer += dt;
	auto& tc = entity.getComponent<goop::TransformComponent>();
	tc.position.x = sin(timer);
}

