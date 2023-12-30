#define GOOP_APPTYPE_EDITOR

#include <goop/App.h>

#ifndef GOOP_APPTYPE_EDITOR
#include <imgui.h>
#endif

class EditorApp : public goop::App
{
  public:
	EditorApp(goop::App* game, goop::Scene* scene) : game(game), App(scene) {}
	void init() final;
	void update(float dt) final;
	void gui() final;

  private:
	ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;
	ImGuiDockNodeFlags windowFlags =
		ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	App* game;
	bool shouldPlay = false;
};
