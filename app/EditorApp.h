#define GOOP_APPTYPE_EDITOR

#include <goop/App.h>

#ifndef GOOP_APPTYPE_EDITOR
#include <imgui.h>
#endif

class EditorApp : public goop::App
{
  public:
	void init() final;
	void update(float dt) final;
	void gui() final;
	void render() final;

  private:
	bool shouldSpawnHouse = true;
	bool shouldSpawnCow = true;
	ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;
	ImGuiDockNodeFlags windowFlags =
		ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
};
