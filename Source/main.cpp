#pragma once
#include "Engine.hpp"
#include "Game.hpp"

int main(int argc, char* argv[])
{
    Program* game = new Game("Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, false);
    game->Init();

    Engine* engine = new Engine(game);
    engine->Run();

    delete game;
    delete engine;

    return 0;
}