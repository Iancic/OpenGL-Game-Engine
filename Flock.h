#pragma once
#include "Program.h"

#include "common.h"
#include "UserInterface.h"
#include "Star.h"
#include "Boid.h"

class Flock : public Program
{
public:
	Flock();
	~Flock();

	void Init(const char* title, int x, int y, int width, int height, bool fullscreen);
	void Update();
	void Render();
	void HandleEvents();
	void Quit();

	bool Running() { return isRunning; };

private:

	enum class GameState
	{
		GAME_ACTIVE,
		GAME_PAUSED,
	};

	ResourceManager* Resources = ResourceManager::getInstance();

	FrameBuffer* RenderingBuffer = nullptr;

	UserInterface* UI = UserInterface::getInstance();

	Transform* cameraTarget = new Transform(); // This is for camera
	Transform target{}; // This is for boids

	SDL_GLContext glContext;

	Camera* ActiveCamera;

	std::vector<Boid*> boidPool;

	const Uint8* keyState;
	bool keyPressed[SDL_NUM_SCANCODES] = { false };

	int milisecondsPreviousFrame;
	double deltaTime;
	float timeElapsed;

	const float CAMERA_SHIFT_INCREMENT_WIDTH = SCREEN_WIDTH;
	const float CAMERA_SHIFT_INCREMENT_HEIGHT = SCREEN_HEIGHT;
	int counterLeft = 0, counterRight = 0, counterTop = 0, counterBot = 0;

	int sectorOffset = 1;
	int nSectorsX = SCREEN_WIDTH / SECTOR_WIDTH;
	int nSectorsY = SCREEN_HEIGHT / SECTOR_HEIGHT;

	unordered_map< uint32_t, Star*> starCache;

	SDL_Joystick* joystickPlayer1 = nullptr;
	const int deadZone = 1500;
	int xDirection = 0;
	int yDirection = 0;

	bool isRunning;

	SDL_Window* window;

	void Time();

	void UserInterface(Creature* creatureArg);
};