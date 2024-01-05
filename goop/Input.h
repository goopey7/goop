// Sam Collier 2023
#pragma once
#include "imgui.h"
#include <goop/sys/Window.h>

namespace goop
{
inline bool isKeyDown(ImGuiKey key) { return ImGui::IsKeyDown(key); }

inline bool wasKeyDown[ImGuiKey_COUNT];

inline bool isKeyPressed(ImGuiKey key)
{
	bool down = ImGui::IsKeyDown(key);
	bool pressed = down && !wasKeyDown[key];
	wasKeyDown[key] = down;
	return pressed;
}

inline bool isKeyReleased(ImGuiKey key) { return ImGui::IsKeyReleased(key); }

inline bool isLMBDown() { return goop::sys::gWindow->isLMBDown(); }

inline bool isRMBDown() { return goop::sys::gWindow->isRMBDown(); }

inline float getMouseX() { return goop::sys::gWindow->getMouseX(); }

inline float getMouseY() { return goop::sys::gWindow->getMouseY(); }

inline float getMouseDeltaX() { return goop::sys::gWindow->getMouseDeltaX(); }

inline float getMouseDeltaY() { return goop::sys::gWindow->getMouseDeltaY(); }

inline void hideCursor(bool hide) { goop::sys::gWindow->hideCursor(hide); }

inline void grabCursor(bool grab) { goop::sys::gWindow->grabCursor(grab); }
} // namespace goop
