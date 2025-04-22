#include "Star.h"
#include <iostream>

Star::Star(float x, float y, int difficulty)
{
	nLehmer = (static_cast<uint32_t>(x) & 0xFFFF) << 16 | (static_cast<uint32_t>(y) & 0xFFFF); // More bits bigger universe

	starExists = (rndInt(0, 100) == 1); 
	if (!starExists) return;

	starDiameter = rndDouble(10.f + difficulty, 15.f + difficulty * 2);
	starColour = starColours[rndInt(0, 3)];

	positionX = rndDouble(-15.f, 15.f);
	positionY = rndDouble(-15.f, 15.f);

	segments = rndInt(5, 6);
	//segments = 6;

	rotationSpeed = rndInt(400, 800); // The higher the value the slower the rotation.

	if (rotationSpeed % 2 == 0)
	{
		rotationDirection = 1;
	}
	else
		rotationDirection = -1;
}

double Star::rndDouble(double min, double max) // Random double in this range
{
	return ((double)Lehmer32() / (double)(0x7FFFFFFF)) * (max - min) + min;
}

int Star::rndInt(int min, int max) // Random int in this ranger
{
	return (Lehmer32() % (max - min)) + min;
}