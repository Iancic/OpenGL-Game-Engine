#pragma once
#include "precomp.h"

class Input
{
public:

	Input();

	void PollEvents(SDL_Window* window, bool inDragRegion);
	void EndFrame();

	glm::vec2 GetLeftStick() { return { controllerAxisLeftX,  controllerAxisLeftY }; }
	glm::vec2 GetRightStick() { return { controllerAxisRightX, controllerAxisRightY }; }

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

	float NormalizeAxis(Sint16 value, float deadZone = 8000.0f);

private:
	SDL_Gamepad* gamepad = nullptr;
	int mouseX = 0, mouseY = 0;
	float controllerAxisRightX = 0.f, controllerAxisRightY = 0.f, controllerAxisLeftX = 0.f, controllerAxisLeftY = 0.f;

	bool controllerButtonPressed[SDL_GAMEPAD_BUTTON_COUNT] = { false };
    bool controllerButtonReleased[SDL_GAMEPAD_BUTTON_COUNT] = { false };
    bool controllerButtonHeld[SDL_GAMEPAD_BUTTON_COUNT] = { false };

	bool controllerMoved = false;
};

