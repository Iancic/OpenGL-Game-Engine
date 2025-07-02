#pragma once
#include <Utilities.hpp>

class Input
{
public:

	Input();

	void PollEvents();
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

private:
	// Events and Input:
	SDL_GameController* controller;
	int mouseX, mouseY;
	const Uint8* keyState = SDL_GetKeyboardState(NULL);
	float controllerAxisRightX, controllerAxisRightY, controllerAxisLeftX, controllerAxisLeftY;
	bool controllerButtonPressed[SDL_CONTROLLER_BUTTON_MAX] = { false };
	bool controllerMoved = false;
};

