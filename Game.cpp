#include "Game.h"
#include "Line.h"

Game::Game()
{
	ActiveCamera = new Camera(45.f, glm::vec3(0, 0, 0), SCREEN_WIDTH, SCREEN_HEIGHT);
	playerPtr = new Player(1, ActiveCamera);
	creaturePool.push_back(playerPtr);

	creaturePool.push_back(new Creature(1, ActiveCamera));
}

Game::~Game()
{

}

void Game::Init(const char* title, int x, int y, int width, int height, bool fullscreen)
{
	int flags = SDL_WINDOW_OPENGL;

	if (fullscreen) flags = SDL_WINDOW_FULLSCREEN;

	SDL_Init(SDL_INIT_EVERYTHING);

	if (SDL_NumJoysticks() >= 1) joystickPlayer1 = SDL_JoystickOpen(0);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	
	window = SDL_CreateWindow(title, x, y, width, height, flags);
	glContext = SDL_GL_CreateContext(window);

	gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);

	if (window) isRunning = true;

	UI->Init(window, glContext);
	
	Resources->loadResources(); // Make Sure it's before setting up OpenGl context

	// Everything that gets inialized with the use of Resources:
	RenderingBuffer = new FrameBuffer();
}

void Game::Update()
{
	Time();

	int mouseX, mouseY;
	SDL_GetMouseState(&mouseX, &mouseY);

	weaponsTarget.position.x = mouseX + ActiveCamera->transform->position.x;
	weaponsTarget.position.y = mouseY + ActiveCamera->transform->position.y;
	
	// Player & Camera Boundaries
	// We use the absolute value of the player's position to track movement across the screen boundaries.
	if (creaturePool[0]->segments[0]->transform.position.x > CAMERA_SHIFT_INCREMENT_WIDTH * counterRight + SCREEN_WIDTH / 2) // Right Boundary
	{
		counterRight++;
		counterLeft--;
		cameraTarget->position.x += CAMERA_SHIFT_INCREMENT_WIDTH;
	}

	else if (creaturePool[0]->segments[0]->transform.position.x < -(CAMERA_SHIFT_INCREMENT_WIDTH * counterLeft + SCREEN_WIDTH / 2)) // Left Boundary
	{
		counterLeft++;
		counterRight--;
		cameraTarget->position.x -= CAMERA_SHIFT_INCREMENT_WIDTH;
	}

	if (creaturePool[0]->segments[0]->transform.position.y > CAMERA_SHIFT_INCREMENT_HEIGHT * counterTop + SCREEN_HEIGHT / 2) // Top Boundary
	{
		counterTop++;
		counterBot--;
		cameraTarget->position.y += CAMERA_SHIFT_INCREMENT_HEIGHT;
	}

	else if (creaturePool[0]->segments[0]->transform.position.y < -(CAMERA_SHIFT_INCREMENT_HEIGHT * counterBot + SCREEN_HEIGHT / 2)) // Bot Boundary
	{
		counterBot++;
		counterTop--;
		cameraTarget->position.y -= CAMERA_SHIFT_INCREMENT_HEIGHT;
	}

	ActiveCamera->UpdateCamera(cameraTarget, 0.3f); // Update last value for lerp

	// Update Creatures 
	for (auto creature : creaturePool)
	{
		creature->Update(weaponsTarget, deltaTime);
	}
}

void Game::Render()
{
	RenderingBuffer->shader->use();

	glUniform1i(glGetUniformLocation(RenderingBuffer->shader->ID, "screenTexture"), 0);
	glBindFramebuffer(GL_FRAMEBUFFER, RenderingBuffer->FBO);

	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);   
	glClearColor(0.f, 0.f, 0.f, 1.0f);

	// -------------- Rendering --------------

	// Set The Offset Based On Camera
	galaxyOffset = { ActiveCamera->transform->position.x, ActiveCamera->transform->position.y };

	// Iterate through all visible sectors on screen
	
	// Planets smaller than player
	for (screenSector.x = sectorOffset; screenSector.x < nSectorsX - sectorOffset; screenSector.x++)
	{
		for (screenSector.y = sectorOffset; screenSector.y < nSectorsY - sectorOffset; screenSector.y++)
		{
			float starX = screenSector.x + galaxyOffset.x;
			float starY = screenSector.y + galaxyOffset.y;

			uint32_t starKey = (static_cast<uint32_t>(starX) & 0xFFFF) << 16 | (static_cast<uint32_t>(starY) & 0xFFFF);

			Star* star = nullptr;
			auto it = starCache.find(starKey);

			if (it != starCache.end()) 
			{
				star = it->second;  // Use the cached star
			}
			else 
			{
				star = new Star(starX, starY);  // Create a new star if it doesn't exist
				starCache[starKey] = star;      // Cache the new star
			}

			if (star->starExists && !starCache[starKey]->isDead)
			{
				float centerX = screenSector.x * SECTOR_WIDTH + SECTOR_WIDTH / 2.0f;
				float centerY = screenSector.y * SECTOR_HEIGHT + SECTOR_HEIGHT / 2.0f;

				// Center + star position offset
				Circle::getInstance()->DrawCircle(centerX + star->positionX, centerY + star->positionY, star->starDiameter, star->segments, star->starColour, timeElapsed / star->rotationSpeed, star->rotationDirection);

				float radius = star->starDiameter;

				float centerPlayerX = creaturePool[0]->segments[0]->transform.position.x;
				float centerPlayerY = creaturePool[0]->segments[0]->transform.position.y;
				float radiusPlayer = creaturePool[0]->segments[0]->segmentRadius;

				// Transform player position to screen coordinates for planet collision

				float adjustedPlayerX = static_cast<int>(centerPlayerX + SCREEN_WIDTH / 2 ) % SCREEN_WIDTH;
				float adjustedPlayerY = static_cast<int>(centerPlayerY + SCREEN_HEIGHT / 2 ) % SCREEN_HEIGHT;
				// For negative values in player
				if (adjustedPlayerX < 0)
					adjustedPlayerX = abs((abs(adjustedPlayerX) - SCREEN_WIDTH / 2) - SCREEN_WIDTH / 2);
				if (adjustedPlayerY < 0)
					adjustedPlayerY = abs((abs(adjustedPlayerY) - SCREEN_HEIGHT / 2) - SCREEN_HEIGHT / 2);

				float factor = 0.65f; // 0 (closer to the center of circle) -> 1.f (instant collision with circle no overlap)
				if ( (radius + radiusPlayer) * (radius + radiusPlayer) * factor >=
					((centerX + star->positionX) - adjustedPlayerX) * ((centerX + star->positionX) - adjustedPlayerX) +
					((centerY + star->positionY) - adjustedPlayerY) * ((centerY + star->positionY) - adjustedPlayerY) )
				{
					starCache[starKey]->isDead = true;
					creaturePool[0]->AddPoints(100);
					delete starCache[starKey];
				}
			}
		}
	}

	for (auto creature : creaturePool)
	{
		creature->Render();
	}

	UserInterfaceUpdate(creaturePool[0]);

	RenderingBuffer->DrawRenderBuffer(timeElapsed, true); // Render FBO
	SDL_GL_SwapWindow(window);
}

void Game::HandleEvents()
{
	SDL_Event event;
	SDL_PollEvent(&event);
	ImGui_ImplSDL2_ProcessEvent(&event);
	const Uint8* keyState = SDL_GetKeyboardState(NULL);

	playerPtr->HandleInput(deltaTime, event);

	if (event.type == SDL_QUIT)
		isRunning = false;
	if (keyState[SDL_SCANCODE_ESCAPE])
		isRunning = false;
}

void Game::Time()
{
	timeElapsed = SDL_GetTicks();

	// Cap the fps to specified number.
	if (!UNCAPPED)
	{
		// If we are too fast, waste some time until we reach the desired milisecond per frame.
		int timeToWait = MIL_PER_FRAME - (SDL_GetTicks() - milisecondsPreviousFrame);
		if (timeToWait > 0 && timeToWait <= MIL_PER_FRAME)
			SDL_Delay(timeToWait); // Yield execution.
	}

	// The difference in ticks since the last frame, converted to seconds
	deltaTime = (SDL_GetTicks() - milisecondsPreviousFrame) / 1000.f;

	milisecondsPreviousFrame = SDL_GetTicks();
}

void Game::UserInterfaceUpdate(Creature* creatureArg)
{
	UI->Start();
	UI->Style();

	if (creatureArg ->DNA > 0)
	{
		UI->UpgradeMenu(creatureArg);
		UI->InfoHUD(creatureArg);
	}

	UI->LevelBar(creatureArg);

	UI->Shutdown();
}

void Game::Quit()
{
	SDL_DestroyWindow(window);
	SDL_Quit();
}