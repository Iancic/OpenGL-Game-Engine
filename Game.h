#pragma once
#include "Program.h"

// Game Specific Header Files
#include "common.h"
#include "Player.h"
#include "Camera.h"
#include "ResourceManager.h"
#include "FrameBuffer.h"
#include "UserInterface.h"
#include "IKChain.h"
#include "Logger.h"

class Game : public Program
{
public:
	Game(const char* title, int x, int y, int width, int height, bool fullscreen) : Program(title, x, y, width, height, fullscreen) {}
	~Game() {}

	void Init() override;
	void Shutdown() override;
	void Render() override;
	void HandleEvents() override;
	void Update() override;

	static inline Camera* activeCamera = nullptr;

private:

	UserInterface* UI = UserInterface::getInstance();
	ResourceManager* Resources = ResourceManager::getInstance();
	FrameBuffer* RenderingBuffer = nullptr;

	enum class GameState
	{
		GAME_ACTIVE,
		GAME_PAUSED,
	};

	int mouseX, mouseY;
	bool inputPressed = false;
	int armLenght = 5;
	
	Player* playerPtr = nullptr;
	Transform* origin = new Transform();

	// Input
	const Uint8* keyState = SDL_GetKeyboardState(NULL);
	bool keyPressedOnce[SDL_NUM_SCANCODES] = { false };
	bool keyPressed[SDL_NUM_SCANCODES] = { false };
};