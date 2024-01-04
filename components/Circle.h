#pragma once
#include <goop/Components.h>

class Circle : public goop::CustomComponent
{
public:
    Circle(goop::Entity e) : goop::CustomComponent(e) {}
    void init() final;
    void update(float dt) final;
	void gui() final;
private:
	float timer = 0.f;
};
