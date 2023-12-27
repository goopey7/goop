#include <goop/App.h>
#include <goop/Scene.h>

#ifndef GOOP_APPTYPE_EDITOR
#include <imgui.h>
#endif

class GameApp : public goop::App
{
  public:
	GameApp(goop::Scene* scene = nullptr) : App(scene) {}
	void init() final;
	void update(float dt) final;
	void gui() final {}

  private:
	bool shouldSpawnHouse = true;
	bool shouldSpawnCow = true;
	float timeElapsed = 0.f;
};
