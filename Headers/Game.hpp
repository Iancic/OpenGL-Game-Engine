#pragma once
#include "Program.hpp"

// Game Specific Header Files
#include "common.hpp"
#include "Creature.hpp"
#include "Camera.hpp"
#include "ResourceManager.hpp"
#include "FrameBuffer.hpp"
#include "UserInterface.hpp"
#include "IKChain.hpp"
#include "Logger.hpp"
#include "BoxCollider.hpp"
#include "CollisionSolver.hpp"
#include "Emitter.hpp"

class Game : public Program
{
public:
	Game(const char* title, int x, int y, int width, int height, bool fullscreen) : Program(title, x, y, width, height, fullscreen) {}
	~Game() {}

	void Init() override;
	void Shutdown() override;
	void Render() override;
	void HandleEvents() override;
	void HandleInput() override;
	void Update() override;

	static inline Camera* activeCamera = nullptr;

	bool isEditorOn = true;

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
	bool isPlayerMoving = false;
	int armLenght = 5;

	Creature* player = nullptr;
	std::vector<Creature*> creatures;
	void SpawnCreature(); // TODO implement prototype pattern
	Transform* origin = new Transform();

	ParticleProperties testProps;
	Emitter* testEmmiter = nullptr;

	// Events:
	SDL_GameController* controller;

	const Uint8* keyState = SDL_GetKeyboardState(NULL);
	bool keyPressedOnce[SDL_NUM_SCANCODES] = { false };
	bool controllerButtonPressed[SDL_CONTROLLER_BUTTON_MAX] = { false };
	float controllerAxisRightX, controllerAxisRightY;
};