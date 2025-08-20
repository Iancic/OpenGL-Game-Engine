#pragma once
#include "Creature.hpp"

class Command
{
	virtual void execute(Creature& creature) = 0;
};

class MoveNorth : public Command
{
	virtual void execute(Creature& creature)
	{
		creature.MoveNorth();
	};
};

class MoveSouth : public Command
{
	virtual void execute(Creature& creature)
	{
		creature.MoveSouth();
	};
};

class MoveEast : public Command
{
	virtual void execute(Creature& creature)
	{
		creature.MoveEast();
	};
};

class MoveWest : public Command
{
	virtual void execute(Creature& creature)
	{
		creature.MoveWest();
	};
};

class FireCommand : public Command 
{
	virtual void execute(Creature& creature)
	{
		
	};
};