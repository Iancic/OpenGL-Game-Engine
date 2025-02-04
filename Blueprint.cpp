#include "Blueprint.h"

Blueprint::Blueprint(int difficulty)
{
	// Blueprint is created based on difficulty
	// Higher difficulty bigger monsters
	srand(time(NULL));

	// Creature Lenght
	int baseLenght = 15;
	int extraLenght = rand() % 3; // Can be 0 extra segments, 1 extra segment or 2 extra segments
	creatureLenght = (difficulty / 10) + baseLenght + extraLenght;

	// Creature Width
	segmentSpacing = 10;
	int baseCreatureWidth = 10;
	
	int shape = rand() % 2; // Exponential or Normal
	//int shape = 1;

	if (shape) // Normal
	{
		for (int i = 0; i <= creatureLenght; i++)
		{
			creatureShape.push_back(baseCreatureWidth);
		}
	}
	else // Exponential 
	{
		for (int i = 0; i <= creatureLenght; i++)
		{
			creatureShape.push_back(i + baseCreatureWidth);
		}
	}

	// Creature Segment Shape
	int mixed = rand() % 2; // Does it have mixed or uniform shape?

	mixed = 0;

	if (mixed && shape) // Random Shapes For Creature
	{
		for (int i = 0; i <= creatureLenght; i++)
		{
			int shapeType = rand() % 3 + 3;
			segmentShape.push_back(shapeType);
		}
	}
	else // One Shape For The Whole Creature
	{
		//int shapeType = rand() % 3 + 3;
		int shapeType = 360;

		for (int i = 0; i <= creatureLenght; i++)
		{
			segmentShape.push_back(shapeType);
		}
	}
}

Blueprint::~Blueprint()
{

}
