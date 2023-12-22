#include <goop/App.h>

#ifndef GOOP_APPTYPE_EDITOR
#include <imgui.h>
#endif

class GameApp : public goop::App
{
  public:
	void init() final;
	void update(float dt) final;
	void gui() final;
	void render() final;

  private:
	bool shouldSpawnHouse = true;
	bool shouldSpawnCow = true;
};
