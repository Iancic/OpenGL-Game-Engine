#include "Game.hpp"

Game* Game::game_Instance = nullptr;

void Game::Init()
{
	scenes.push_back(new Scene());
	activeScene = scenes[0];

	activeCamera = new Camera(45.f, glm::vec3(0, 0, 0), SCREEN_WIDTH, SCREEN_HEIGHT);

	activeCamera->updateProjection();
	Line::getInstance()->SetCamera(activeCamera); // Important To Setup For Debug Drawing
}

void Game::Update()
{
	if (UI->gameState == UserInterface::GameState::PLAYING) { activeScene->Update(*InputSystem, deltaTime); }
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
	if (UI->gameState == UserInterface::GameState::PLAYING) activeScene->Render();
}

void Game::Shutdown()
{

}