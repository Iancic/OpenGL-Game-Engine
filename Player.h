#pragma once
#include "Segment.h"
#include "BaseWeapon.h"
#include "common.h"
#include "Line.h"
#include "IKChain.h"
#include "SpriteRenderer.h"
#include "Vertex.h"
#include <vector>

class Player
{
public:
	Player(int difficulty, Camera* camera);
	~Player();

	enum DIRECTIONS
	{
		NORTH,
		SOUTH,
		WEST,
		EAST
	};

	// Creature Options:
	float movementSpeed = 225.f;
	int hitpoints = 100;
	int regen = 10;
	int weaponSlots = 1;

	int creatureLenght = 5;
	int segmentSpacing = 10;

	std::vector<int> creatureShape;
	std::vector<int> segmentShape;

	bool hasSpine = true, hasGeometry = false, hasWeapons = true;
	
	// Leg Options:
	float outwardBias = 0.5f;
	float sideOffset = 30.f;
	float forwardOffset = 30.0f;
	std::vector<glm::vec2> footTargets;
	std::vector<IKChain*> legs;
	int legCounter = 2;
	float distanceToSnap = 10.f;
	glm::vec2 lastSnap{ 0 };

	int points = 0, maxPoints = 1000, level = 1; // points > maxPoints -> Level Up
	int DNA = 0; // Currency 1
	int dust = 0; // Currency 2

	glm::vec2 lastLeft;
	glm::vec2 lastRight;

	std::vector<Segment*> segments;
	std::vector<BaseWeapon*> weapons;
	int weaponsCreated = 0;

	int maxRadius;

	GLuint spineVAO, spineVBO;
	std::vector<Vertex>spineVertices;

	Camera* activeCamera = nullptr;

	SpriteRenderer* spriteRenderer = nullptr; // Used to draw all sprites

	void RenderOutline();
	void RenderSprites();
	void Update(Transform target, float deltaTime);
	bool HandleInput(float deltaTime, SDL_Event eventArg);

	void Spine();
	void Shoot();

	void AddPoints(int valueArg);
	void LevelUp();

	int desiredLenght;
	void IncreaseBodyLenght();
	void DecreaseBodyLenght();

	void IncreaseSpeed(float valueArg);
	void DecreaseSpeed(float valueArg);

	void IncreaseWeaponSlots();
	void DecreaseWeaponSlots();

	void IncreaseHealth(int valueArg);
	void DecreaseHealth(int valueArg);

	void IncreaseSizeUniformly();
	void DecreaseSizeUniformly();

	void IncreaseSizeSequentially();
	void DecreaseSizeSequentially();

	void AddLeg();
	void RemoveLeg();

	int desiredSegmentSpacing;
	void AddSpacing();
	void RemoveSpacing();

private:
	bool keyPressedOnce[SDL_NUM_SCANCODES] = { false };
	bool keyPressed[SDL_NUM_SCANCODES] = { false };

};