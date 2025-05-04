#include "Engine.hpp"

Engine::Engine(Program* programArg)
{
	activeProgram = programArg;
}

void Engine::Run()
{
    while (activeProgram->Running())
    {
        activeProgram->HandleEvents();
        activeProgram->HandleInput();
        activeProgram->Update();
        activeProgram->Render();
    }

    activeProgram->Quit();
}