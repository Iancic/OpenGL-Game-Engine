#pragma once
#include <vector>
#include <cstdlib>
#include <ctime>

class Blueprint
{
public:
	Blueprint(int difficulty);
	~Blueprint();

	float movementSpeed = 225.f;
	int hitpoints = 100;
	int regen = 10;
	int weaponSlots = 0;

	int creatureLenght;
	int segmentSpacing;

	std::vector<int> creatureShape;
	std::vector<int> segmentShape;

	bool hasSpine = true, hasGeometry = false, hasWeapons = true;

};