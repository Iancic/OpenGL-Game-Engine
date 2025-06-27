#pragma once
#include "Program.hpp"
#include "Utilities.hpp"
#include "Scene.hpp"

class Game : public Program
{
public:
	Game(const char* title) : Program(title) {}
	~Game() {}

	void Init() override;
	void Shutdown() override;
	void Render() override;
	void HandleEvents() override;
	void HandleInput() override;
	void Update() override;

	static inline Camera* activeCamera = nullptr;

	void LoadScene();

	enum class GameState
	{
		PAUSED,
		PLAYING,
		STOPPED
	};
	GameState gameState = GameState::PAUSED;

private:

	std::vector<Scene*> scenes;
	Scene* activeScene = nullptr;
	
	// Events and Input:
	SDL_GameController* controller;
	int mouseX, mouseY;
	const Uint8* keyState = SDL_GetKeyboardState(NULL);
	float controllerAxisRightX, controllerAxisRightY;
	bool keyPressedOnce[SDL_NUM_SCANCODES] = { false };
	bool controllerButtonPressed[SDL_CONTROLLER_BUTTON_MAX] = { false };
};