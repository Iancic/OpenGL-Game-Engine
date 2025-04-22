#include "Game.h"
#include "Line.h"

void Game::Init()
{
	UI->Init(window, glContext);
	Resources->loadResources();
	RenderingBuffer = new FrameBuffer();

	activeCamera = new Camera(45.f, glm::vec3(0, 0, 0), SCREEN_WIDTH, SCREEN_HEIGHT);
	playerPtr = new Player(1, activeCamera);

	activeCamera->updateProjection(glm::vec2(playerPtr->segments[0]->transform.position.x, playerPtr->segments[0]->transform.position.y));
	Line::getInstance()->activeCamera = activeCamera; // Very important
}

void Game::Shutdown()
{

}

void Game::Update()
{
	Time();
	
	SDL_GetMouseState(&mouseX, &mouseY);

	mouseX = mouseX - SCREEN_WIDTH / 2;
	mouseY = mouseY - SCREEN_HEIGHT / 2;

	playerPtr->Update(*origin, deltaTime);

	activeCamera->UpdateCamera(&playerPtr->segments[0]->transform, playerPtr->segments[0]->direction, deltaTime, inputPressed);

	glm::vec2 focus = glm::vec2(playerPtr->segments[0]->transform.position.x, playerPtr->segments[0]->transform.position.y);
	activeCamera->updateProjection(focus);

}

void Game::Render()
{
	// Bind framebuffer, Viewport & clear buffers
	glBindFramebuffer(GL_FRAMEBUFFER, RenderingBuffer->FBO);
	RenderingBuffer->shader->use();
	glUniform1i(glGetUniformLocation(RenderingBuffer->shader->ID, "screenTexture"), 0); 
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);

	// Render Calls
	//playerPtr->RenderSprites();
	playerPtr->RenderOutline();

	// Draw framebuffer to screen
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	RenderingBuffer->DrawRenderBuffer(timeElapsed, true);

	// UI rendering
	UI->Game();
	UI->ImGui(playerPtr, RenderingBuffer->framebufferTexture, activeCamera);

	// Present
	SDL_GL_SwapWindow(window);
}

void Game::HandleEvents()
{
	SDL_Event event;
	SDL_PollEvent(&event);
	ImGui_ImplSDL2_ProcessEvent(&event);
	const Uint8* keyState = SDL_GetKeyboardState(NULL);

	// Player Movement:
	inputPressed = playerPtr->HandleInput(deltaTime, event);

	// Quit The Game:
	if (event.type == SDL_QUIT)
		isRunning = false;
	if (keyState[SDL_SCANCODE_ESCAPE])
		isRunning = false;
}