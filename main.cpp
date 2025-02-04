#pragma once
#include "Game.h"
#include "Engine.h"

int main(int argc, char* argv[])
{
    Game* game = new Game();
    game->Init("Cosmoboros", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, false);

    Engine* engine = new Engine(game);
    engine->Run();

    return 0;
}