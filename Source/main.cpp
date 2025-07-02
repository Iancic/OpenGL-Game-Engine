#pragma once
#include "Engine.hpp"
#include "Game.hpp"

int main(int argc, char* argv[])
{
    Program* game = Game::getInstance("Engine");
    Engine* engine = new Engine(game);
    engine->Run();

    delete game;
    delete engine;

    return 0;
}