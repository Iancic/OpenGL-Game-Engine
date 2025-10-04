#pragma once
#include "precomp.h"
#include "Engine.hpp"
#include "Game.hpp"

int main(int argc, char* argv[])
{
    Program* game = Game::getInstance("Untitled Editor");
    Engine* engine = new Engine(game);
    engine->Run();

    delete game;
    delete engine;

    return 0;
}