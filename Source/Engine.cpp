#include "Engine.hpp"

Engine::Engine(Program* programArg)
{
	activeProgram = programArg;
}

void Engine::Run()
{
    activeProgram->Init();

    while (activeProgram->Running())
    {
        activeProgram->HandleEvents();
        activeProgram->HandleInput();
        activeProgram->Update();
        activeProgram->PreRender();
        activeProgram->Render();
        activeProgram->PostRender();
    }

    activeProgram->Quit();
}