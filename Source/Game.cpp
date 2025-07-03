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
	if (UI->gameState == UserInterface::GameState::PLAYING) activeScene->Update();
}

void Game::HandleInput()
{
	if (InputSystem->GetEvent(SDL_QUIT)) isRunning = false;
	if (InputSystem->GetEvent(SDL_WINDOWEVENT_RESIZED)) UI->Resize(window);
	if (InputSystem->GetKey(SDL_SCANCODE_A, Input::Action::DOWN)) Logger::Log("Test message ", 123, 45.6f, true);
}

void Game::Render()
{
	activeScene->RenderingSystem(activeCamera);
}

void Game::Shutdown()
{

}