#pragma once
#include <goop/Components.h>
#include <goop/Entity.h>

class TestCustomComp : public goop::CustomComponent
{
  public:
	TestCustomComp(goop::Entity e) : goop::CustomComponent(e) {}
	void init() final;
	void update(float dt) final;

  private:
	float timer = 0.0f;
};
