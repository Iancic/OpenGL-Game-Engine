#pragma once

// Engine executes on programs
#include "Program.hpp"

class Engine
{
public:
	Engine(Program* programArg);
	~Engine() = default;

	void Run();
	
private:

	Program* activeProgram = nullptr;
};