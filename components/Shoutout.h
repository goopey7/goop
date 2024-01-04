#pragma once
#include <goop/Components.h>

class Shoutout : public goop::CustomComponent
{
  public:
	Shoutout(goop::Entity e) : goop::CustomComponent(e) { name = "Shoutout"; }
	void init();
	void update(float dt);
	void gui();

  private:
	float timer;
};
