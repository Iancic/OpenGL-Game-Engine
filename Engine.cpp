#include "Engine.h"
Engine::Engine(Program* programArg)
{
	activeProgram = programArg;
}

void Engine::Run()
{
    while (activeProgram->Running())
    {
        activeProgram->HandleEvents();
        activeProgram->Update();
        activeProgram->Render();
    }

    activeProgram->Quit();
}