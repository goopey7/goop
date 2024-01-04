// Sam Collier 2023
#pragma once
#include "imgui.h"
#include <goop/sys/Window.h>

namespace goop
{
static bool isKeyDown(ImGuiKey key) { return ImGui::IsKeyDown(key); }

static bool wasKeyDown[ImGuiKey_COUNT];

static bool isKeyPressed(ImGuiKey key)
{
	bool down = ImGui::IsKeyDown(key);
	bool pressed = down && !wasKeyDown[key];
	wasKeyDown[key] = down;
	return pressed;
}

static bool isKeyReleased(ImGuiKey key) { return ImGui::IsKeyReleased(key); }

static bool isLMBDown() { return goop::sys::gWindow->isLMBDown(); }

static bool isRMBDown() { return goop::sys::gWindow->isRMBDown(); }

static float getMouseX() { return goop::sys::gWindow->getMouseX(); }

static float getMouseY() { return goop::sys::gWindow->getMouseY(); }

static float getMouseDeltaX() { return goop::sys::gWindow->getMouseDeltaX(); }

static float getMouseDeltaY() { return goop::sys::gWindow->getMouseDeltaY(); }

static void hideCursor(bool hide) { goop::sys::gWindow->hideCursor(hide); }

static void grabCursor(bool grab) { goop::sys::gWindow->grabCursor(grab); }
} // namespace goop
