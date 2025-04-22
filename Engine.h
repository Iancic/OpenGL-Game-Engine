#pragma once

// Engine executes on programs
#include "Program.h"

class Engine
{
public:
	Engine(Program* programArg);
	~Engine() = default;

	void Run();
	
private:

	Program* activeProgram = nullptr;
};