#include "Player.h"

Player::Player(int difficulty, Camera* camera) 
	: Creature(difficulty, camera)
{
	identity = new Blueprint(difficulty);
	activeCamera = camera;

	// Create Segments
	for (int i = 0; i <= identity->creatureLenght; i++)
	{
		if (i == 0)
			segments.push_back(new Segment(NULL, identity->segmentShape[i], identity->creatureShape[i], identity->segmentSpacing));
		else
			segments.push_back(new Segment(segments[i - 1], identity->segmentShape[i], identity->creatureShape[i], identity->segmentSpacing));
	}

	for (int i = 0; i <= identity->creatureLenght; i += 2)
	{
		if (weaponsCreated < identity->weaponSlots)
		{
			weapons.push_back(new BaseWeapon(&segments[i]->transform, activeCamera));
			weaponsCreated++;
		}
	}

	// Spawn Position
	float positionX = 0;

	for (int i = identity->creatureLenght; i >= 0; i--)
	{
		segments[i]->transform.position.y = 50;
		segments[i]->transform.position.x = positionX;
		positionX += identity->segmentSpacing;
	}
}

Player::~Player()
{

}

void Player::Update(Transform target, float deltaTime)
{
	if (points >= maxPoints)
	{
		DNA++;
		level++;
		points = points - maxPoints; // Remainder of points
	}

	// Update each segment
	for (auto& element : segments)
	{
		element->Update();
	}

	// Update each weapons
	for (auto& element : weapons)
	{
		element->Update(target, deltaTime);
	}

	// Direction vectors
	for (int i = 0; i < identity->creatureLenght; i++)
	{
		segments[i]->direction = {
			segments[i + 1]->transform.position.x - segments[i]->transform.position.x,
			segments[i + 1]->transform.position.y - segments[i]->transform.position.y };
	}
}

void Player::HandleInput(float deltaTime, SDL_Event eventArg)
{
	printf("I reach here.");

	// Joysticks
	if (eventArg.type == SDL_JOYAXISMOTION)
	{
		if (eventArg.jaxis.which == 0) // Check which joystick moved
		{
			// Example of smooth joystick axis handling:
			float deadZone = 0.1f; // A threshold to avoid small, unintentional movements

			if (eventArg.jaxis.axis == 0) // X axis
			{
				float axisValue = eventArg.jaxis.value / 32767.0f; // Normalize between -1 and 1
				if (fabs(axisValue) > deadZone)
				{
					axisValue = (fabs(axisValue) - deadZone) / (1.0f - deadZone); // Smooth input response
					segments[0]->transform.position.x += axisValue * identity->movementSpeed * deltaTime;
				}
			}

			if (eventArg.jaxis.axis == 1) // Y axis
			{
				float axisValue = eventArg.jaxis.value / 32767.0f; // Normalize between -1 and 1
				if (fabs(axisValue) > deadZone)
				{
					axisValue = (fabs(axisValue) - deadZone) / (1.0f - deadZone); // Smooth input response
					segments[0]->transform.position.y += axisValue * identity->movementSpeed * deltaTime;
				}
			}

		}
	}

	// Mouse
	if (eventArg.type == SDL_MOUSEBUTTONDOWN)
	{
		if (SDL_BUTTON_LEFT)
		{

		}
	}

	const Uint8* keyState = SDL_GetKeyboardState(NULL);

	if (keyState[SDL_SCANCODE_W])
		segments[0]->transform.position.y -= identity->movementSpeed * deltaTime;
	if (keyState[SDL_SCANCODE_D])
		segments[0]->transform.position.x += identity->movementSpeed * deltaTime;
	if (keyState[SDL_SCANCODE_A])
		segments[0]->transform.position.x -= identity->movementSpeed * deltaTime;
	if (keyState[SDL_SCANCODE_S])
		segments[0]->transform.position.y += identity->movementSpeed * deltaTime;

	if (eventArg.type == SDL_KEYDOWN)
	{
		if (!keyPressedOnce[eventArg.key.keysym.scancode])
		{
			if (eventArg.key.keysym.scancode == SDL_SCANCODE_SPACE)
			{
				Shoot();
				//DNA += 100;
			}
		}
	}

	if (eventArg.type == SDL_KEYUP)
	{
		keyPressedOnce[eventArg.key.keysym.scancode] = false; // Reset on key release
	}
}
