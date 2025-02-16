#pragma once
#include "Creature.h"

class Player : public Creature
{
public:
	Player(int difficulty, Camera* camera);
	~Player();

	void Update(Transform target, float deltaTime) override;
	void HandleInput(float deltaTime, SDL_Event eventArg);

private:
	bool keyPressedOnce[SDL_NUM_SCANCODES] = { false };
	bool keyPressed[SDL_NUM_SCANCODES] = { false };

};