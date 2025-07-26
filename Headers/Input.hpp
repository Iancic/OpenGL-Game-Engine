#pragma once
#include <glad/glad.h> // Link OpenGL Functions
#include <SDL.h> // SDL: Windowing, Input, Audio
#include <glm-master/glm/glm.hpp> // GLM: Math
#include <imgui-master/imgui.h> // ImGui
#include <imgui-master/imgui_impl_sdl2.h>

class Input
{
public:

	Input();

	void PollEvents(SDL_Window* window, bool inDragRegion);
	void EndFrame();

	glm::vec2 GetLeftStick();
	glm::vec2 GetRightStick();

	glm::vec2 GetMousePosition();

	bool GetEvent(int key);
	bool GetKey(int key, int action);
	bool GetButton(int key, int action);

	void StartRumble();

	enum Action { DOWN, UP, HOLD };

	bool quitSDL = false;
	bool resizedWindow = false;

	std::unordered_map<int, bool> keyPressed;
	std::unordered_map<int, bool> keyReleased;
	std::unordered_map<int, bool> keyHeld;

	float NormalizeAxis(Sint16 value, float deadZone = 8000.0f)
	{
		if (abs(value) < deadZone)
			return 0.0f;

		// Convert to [-1.0, 1.0] range
		float result = value / 32767.0f;

		// Optional: Scale to start at 0.0 right after dead zone
		float adjusted = (abs(result) - (deadZone / 32767.0f)) / (1.0f - (deadZone / 32767.0f));
		return (value > 0) ? adjusted : -adjusted;
	};

private:
	// Events and Input:
	SDL_GameController* controller;
	int mouseX, mouseY;
	const Uint8* keyState = SDL_GetKeyboardState(NULL);
	float controllerAxisRightX, controllerAxisRightY, controllerAxisLeftX, controllerAxisLeftY;
	bool controllerButtonPressed[SDL_CONTROLLER_BUTTON_MAX] = { false };
	bool controllerMoved = false;

	bool dragging = false;
	int drag_offset_x = 0, drag_offset_y = 0;
};

