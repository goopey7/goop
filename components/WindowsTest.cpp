#include "WindowsTest.h"
#include <iostream>

// Gets called when the game starts
void WindowsTest::init()
{
	/* do not remove this line */
	CustomComponent::init();
	std::cout << "WINDOWS INIT" << std::endl;
    //...
}

// Gets called every frame
void WindowsTest::update(float dt)
{
    //...
}

// Collision callbacks
void WindowsTest::onCollisionEnter(goop::Entity other)
{
	// std::cout << "Player started colliding with " << other.getComponent<goop::TagComponent>().tag << std::endl;
	//...
}

void WindowsTest::onCollisionExit(goop::Entity other)
{
	// std::cout << "Player stopped colliding with " << other.getComponent<goop::TagComponent>().tag << std::endl;
	//...
}

// Editor GUI - Shown in inspector view
// Refer to ImGui documentation for more info
void WindowsTest::gui()
{
	// ImGui::Text("Hello from %s", name.c_str());
    //...
}
