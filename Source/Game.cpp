#include "Game.hpp"

void Game::Init()
{
	controller = SDL_GameControllerOpen(0);

	scenes.push_back(new Scene());
	activeScene = scenes[0];

	activeCamera = new Camera(45.f, glm::vec3(0, 0, 0), SCREEN_WIDTH, SCREEN_HEIGHT);

	activeCamera->updateProjection(glm::vec2(activeScene->origin->position.x, activeScene->origin->position.y));
	Line::getInstance()->SetCamera(activeCamera); // Important To Setup For Debug Drawing

	TestSound();
}

void Game::Update()
{
	Time();
	if (gameState == GameState::PLAYING)
		activeScene->Update();
}

void Game::LoadScene()
{
	// change active scene;
}

void Game::Render()
{
	activeScene->RenderingSystem(activeCamera);

	// Draw framebuffer to screen
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	RenderingBuffer->DrawRenderBuffer(timeElapsed);

	// Engine Editor UI
	if (isEditorOn) UI->EngineEditor(RenderingBuffer, activeCamera, activeScene);
}

void Game::Shutdown()
{

}

void Game::HandleEvents()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		ImGui_ImplSDL2_ProcessEvent(&event);

		switch (event.type)
		{
			case SDL_QUIT:
				isRunning = false;
				break;

			case SDL_KEYDOWN:
				if (!event.key.repeat && !keyPressedOnce[event.key.keysym.scancode])
				{
					keyPressedOnce[event.key.keysym.scancode] = true;

					if (event.key.keysym.scancode == SDL_SCANCODE_A)
					{
						activeScene->Start();
						gameState = GameState::PLAYING;
					}

					if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
					{
						isRunning = false; // Quit Game
					}
				}
				break;

			case SDL_KEYUP:
				keyPressedOnce[event.key.keysym.scancode] = false;
				break;

			case SDL_CONTROLLERBUTTONDOWN:
				if (!controllerButtonPressed[event.cbutton.button])
				{
					controllerButtonPressed[event.cbutton.button] = true;

					if (event.cbutton.button == SDL_CONTROLLER_BUTTON_A) Logger::Log("Pressed A");
					if (event.cbutton.button == SDL_CONTROLLER_BUTTON_B) Logger::Log("Pressed B");
					if (event.cbutton.button == SDL_CONTROLLER_BUTTON_X) Logger::Log("Pressed X");;
					if (event.cbutton.button == SDL_CONTROLLER_BUTTON_Y) Logger::Log("Pressed Y");

					if (SDL_GameControllerRumble(controller, 30000, 60000, 500) != 0)
					{
						SDL_Log("Failed to start rumble: %s", SDL_GetError());
					}
				}
				break;

			case SDL_CONTROLLERBUTTONUP:
				controllerButtonPressed[event.cbutton.button] = false;
				break;

			default:
				break;
		}
	}
}

void Game::HandleInput()
{
	// Key Pressed 
	const Uint8* keyState = SDL_GetKeyboardState(NULL);

	// Controller Pressed
	const float controllerAxisLeftX = NormalizeAxis(SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX));
	const float controllerAxisLeftY = NormalizeAxis(SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY));
	controllerAxisRightX = NormalizeAxis(SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTX));
	controllerAxisRightY = NormalizeAxis(SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTY));

	if (keyState[SDL_SCANCODE_W]) {  };
	if (keyState[SDL_SCANCODE_S]) {  };
	if (keyState[SDL_SCANCODE_D]) {  };
	if (keyState[SDL_SCANCODE_A]) {  };

	bool controllerMoved = fabs(controllerAxisLeftX) > 0.1f || fabs(controllerAxisLeftY) > 0.1f;
	if (controllerMoved) { }
}