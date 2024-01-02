#define GOOP_APPTYPE_EDITOR

#include <goop/App.h>
#include <goop/Entity.h>

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
		ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	App* game;
	bool shouldPlay = false;
	bool addEntityPopupOpen = false;
	bool addComponentPopupOpen = false;
	bool changeMeshPopupOpen = false;
	bool changeTexturePopupOpen = false;
	char entityName[256] = "";
	std::string oldMeshPath = "";
	std::string oldTexturePath = "";
	char meshPath[256] = "";
	char texturePath[256] = "";
	std::optional<goop::Entity> selectedEntity = std::nullopt;
};
