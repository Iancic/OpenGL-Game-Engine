#include "Creature.h"
#include <iostream>
#include <math.h>
#include <cmath>

Creature::Creature(int difficulty, Camera* camera)
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

void Creature::Update(Transform target, float deltaTime)
{
	
}

void Creature::Render()
{
	// Render each segment geometry
	if (identity->hasGeometry)
	{
		for (auto& element : segments)
		{
			element->Render(activeCamera);
		}
	}

	// Render each weapon
	if (identity->hasWeapons)
	{
		for (auto& element : weapons)
		{
			element->Render(activeCamera);
		}
	}

	// Render Spine
	if (identity->hasSpine)
	{
		Spine();
	}
}

void Creature::Spine()
{
	for (int i = 0; i < identity->creatureLenght - 1; i++) 
	{
		// Get the start and end points for the current and next segment
		glm::vec2 startPos = segments[i]->transform.position - activeCamera->transform->position;
		glm::vec2 endPos = segments[i + 1]->transform.position - activeCamera->transform->position;

		glm::vec2 startDir = segments[i]->direction;
		glm::vec2 endDir = segments[i + 1]->direction;

		// Start Left & Right Positions:
		glm::vec2 leftPointStart = glm::vec2(startDir.y, -startDir.x); // Rotate counter-clockwise by 90 degrees
		glm::vec2 rightPointStart = glm::vec2(-startDir.y, startDir.x); // Rotate clockwise by 90 degrees

		// Make sure these are normalized (even if the direction vector isn't)
		leftPointStart = glm::normalize(leftPointStart);
		rightPointStart = glm::normalize(rightPointStart);

		// End Left & Right Positions:
		glm::vec2 leftPointEnd = glm::vec2(endDir.y, -endDir.x);
		glm::vec2 rightPointEnd = glm::vec2(-endDir.y, endDir.x);

		// Normalize again if needed
		leftPointEnd = glm::normalize(leftPointEnd);
		rightPointEnd = glm::normalize(rightPointEnd);

		// Calculate the final positions using the segment radius
		glm::vec2 leftA = startPos + leftPointStart * glm::vec2(segments[i]->segmentRadius);
		glm::vec2 rightA = startPos + rightPointStart * glm::vec2(segments[i]->segmentRadius);

		glm::vec2 leftB = endPos + leftPointEnd * glm::vec2(segments[i]->segmentRadius);
		glm::vec2 rightB = endPos + rightPointEnd * glm::vec2(segments[i]->segmentRadius);

		if (i == 0)
		{
			Circle::getInstance()->DrawSemiCircle(leftA.x, leftA.y, rightA.x, rightA.y, segments[i]->segmentRadius, M_PI / 2, glm::vec4 (255, 255, 255, 255));
		}
		else if (i == identity->creatureLenght - 2)
		{
			Circle::getInstance()->DrawSemiCircle(leftB.x, leftB.y, rightB.x, rightB.y, segments[i + 1]->segmentRadius, -M_PI / 2, glm::vec4(255, 255, 255, 255));
		}

		// Draw the lines (this will depend on your specific SDL function)
		Line::getInstance()->Render(glm::vec3(leftA,0.f), glm::vec3(leftB, 0.f), glm::vec4(255, 255, 255, 255));
		Line::getInstance()->Render(glm::vec3(rightA, 0.f), glm::vec3(rightB, 0.f), glm::vec4(255, 255, 255, 255));
	}
}

void Creature::IncreaseLenght()
{
	if (DNA >= 1)
	{
		DNA--;
		identity->creatureLenght += 1;
		segments.push_back(new Segment(segments[identity->creatureLenght - 1], 
										segments[identity->creatureLenght - 1]->segmentShape, 
										segments[identity->creatureLenght - 1]->segmentRadius,
										10));
	}
}

void Creature::IncreaseSpeed()
{
	if (DNA >= 1)
	{
		DNA--;
		identity->movementSpeed += 12.5f;
	}
}

void Creature::IncreaseSlots()
{
	if (DNA >= 1)
	{
		DNA--;
		identity->weaponSlots += 1;

		for (int i = weaponsCreated; i <= identity->creatureLenght; i+=2)
		{
			if (weaponsCreated < identity->weaponSlots)
			{
					weapons.push_back(new BaseWeapon(&segments[i]->transform, activeCamera));
					weaponsCreated++;
			}
		}
	}
}

void Creature::IncreaseHealth()
{
	if (DNA >= 1)
	{
		DNA--;
		identity->hitpoints += 50;
	}
}

void Creature::Shoot()
{
	for (auto& element : weapons)
	{
		element->Shoot();
	}
}