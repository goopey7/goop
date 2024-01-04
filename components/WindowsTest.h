#pragma once
#include <goop/Components.h>
#include <imgui.h>

class WindowsTest : public goop::CustomComponent
{
public:
    WindowsTest(goop::Entity e) : goop::CustomComponent(e) {name = "WindowsTest";}
    void init();
    void update(float dt);
    void gui();
private:
	void onCollisionEnter(goop::Entity other) final;
	void onCollisionExit(goop::Entity other) final;
};
