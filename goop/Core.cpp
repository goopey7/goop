// Sam Collier 2023
#include "Core.h"

#include <chrono>
#include <imgui.h>
#include <rustlib.h>

using namespace goop;

const std::unique_ptr<goop::sys::ResourceManager> goop::rm =
	std::make_unique<goop::sys::ResourceManager>();

Core::Core(int argc, char** argv) : app(createApp(argc, argv))
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	auto& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	sys::gWindow->initialize();
	sys::gWindow->openWindow(1280, 720, "Goop", GOOP_WINDOW_DEFAULT);
	sys::gRenderer->initialize();
	rm->initialize();
	app->init();
}

void Core::run()
{
	say_hello("Sam");

	auto last = std::chrono::high_resolution_clock::now();
	float blastTime = 0.f;

	while (!sys::gWindow->shouldClose())
	{
		// get delta time with chrono
		auto now = std::chrono::high_resolution_clock::now();
		auto dt = std::chrono::duration<float>(now - last).count();
		sys::gWindow->pollEvents();

		app->update(dt);

		sys::gRenderer->beginFrame();
		ImGui::NewFrame();

#ifdef GOOP_APPTYPE_EDITOR
		ImVec2 viewportSize = app->getViewportSize();
		sys::gRenderer->render(viewportSize.x, viewportSize.y);
#endif
		app->gui();

#ifdef GOOP_APPTYPE_EDITOR
		ImGui::ShowDemoWindow();
#endif

		ImGui::Render();

		if (sys::gRenderer->isMeshQueueEmpty())
		{
			app->render();
		}

#ifndef GOOP_APPTYPE_EDITOR
		sys::gRenderer->render();
#endif

		sys::gRenderer->endFrame();

#ifdef GOOP_APPTYPE_EDITOR
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
#endif

		last = now;
	}

	sys::gRenderer->destroy();
	sys::gWindow->destroy();
	rm->destroy();
}
