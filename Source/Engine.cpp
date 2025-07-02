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
        activeProgram->InputSystem->PollEvents();
        activeProgram->HandleInput();
        activeProgram->Time();
        activeProgram->Update();
        activeProgram->InputSystem->EndFrame();
        activeProgram->PreRender();
        activeProgram->Render();
        activeProgram->PostRender();
    }

    activeProgram->Quit();
}