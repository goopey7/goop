// Sam Collier 2023
#pragma once

namespace goop
{
class App
{
  public:
	virtual void init() = 0;
	virtual void update(float dt) = 0;
	virtual void gui() = 0;
	virtual void render() = 0;
};
extern App* createApp(int argc, char** argv);
} // namespace goop
