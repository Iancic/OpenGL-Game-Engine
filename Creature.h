#pragma once
#include "Segment.h"
#include "BaseWeapon.h"
#include "Blueprint.h"
#include "common.h"
#include "Line.h"
#include <vector>

class Creature
{
public:
	Creature(int difficulty, Camera* camera);
	~Creature() = default;


	int points = 0, maxPoints = 1000, DNA = 0, level = 1;

	enum DIRECTIONS 
	{
		NORTH,
		SOUTH,
		WEST,
		EAST
	};

	glm::vec2 lastLeft;
	glm::vec2 lastRight;

	std::vector<Segment*> segments;
	std::vector<BaseWeapon*> weapons;

	Blueprint* identity;

	Camera* activeCamera = nullptr;

	void Update(Transform target, float deltaTime);
	void Render();

	void Spine();

	void AddPoints(int value) { points += value; };

	void IncreaseLenght();
	void IncreaseSpeed();
	void IncreaseSlots();
	void IncreaseHealth();

protected:

private:
};

