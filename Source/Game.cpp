#include "Game.hpp"

Game* Game::game_Instance = nullptr;

void Game::Init()
{
	// TESTING: build tab with multiple scenes
	scenes.push_back(new Scene());
	activeScene = scenes[0];
	scenes.push_back(new Scene());
	scenes.push_back(new Scene());

	scenes[0]->sceneName = "Main Menu";
	scenes[1]->sceneName = "Game";
	scenes[2]->sceneName = "Exit";

	activeCamera = new Camera(45.f, glm::vec3(0, 0, 0), SCREEN_WIDTH, SCREEN_HEIGHT);

	activeCamera->updateProjection();
	Line::getInstance()->SetCamera(activeCamera); // Important To Setup For Debug Drawing
}

void Game::Update()
{
	Time();

	if (UI->gameState == UserInterface::GameState::PLAYING) activeScene->Update(*InputSystem, deltaTime);

	activeCamera->updateProjection();
}

void Game::HandleInput()
{
	// Window Quit Button
	if (InputSystem->GetEvent(SDL_QUIT)) isRunning = false;

	// Quit Viewport From Fullscreen
	if (InputSystem->GetKey(SDL_SCANCODE_ESCAPE, Input::Action::DOWN) && UI->isViewportMaximized)
	{
		UI->gameState = UserInterface::GameState::STOPPED;
		activeScene->Shutdown();
		UI->isViewportMaximized = false;
	}

	// Window Resize
	if (InputSystem->GetEvent(SDL_WINDOWEVENT_RESIZED)) UI->Resize(window);
}

void Game::Render()
{
	activeScene->Render(deltaTime, activeCamera);
}

void Game::Shutdown()
{

}