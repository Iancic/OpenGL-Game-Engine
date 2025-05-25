#include "Game.hpp"

// ----------------------
//     INITIALIZATION
// ----------------------

void Game::Init()
{
	controller = SDL_GameControllerOpen(0);
	Resources->loadResources();

	UI->Init(window, glContext);

	RenderingBuffer = new FrameBuffer();
	activeCamera = new Camera(45.f, glm::vec3(0, 0, 0), SCREEN_WIDTH, SCREEN_HEIGHT);
	player = new Creature(1, activeCamera);

	activeCamera->updateProjection(glm::vec2(player->segments[0]->transform.position.x, player->segments[0]->transform.position.y));
	Line::getInstance()->SetCamera(activeCamera); // Important To Setup For Debug Drawing

	testEmmiter = new Emitter();

	L = luaL_newstate();
	luaL_openlibs(L);

	// Register C++ function as callable from Lua
	lua_register(L, "GetRandomNumber", lua_GetRandomNumber);

	if (CheckLua(L, luaL_dofile(L, "Scripts/RandomNumber.lua")))
	{
		lua_getglobal(L, "ReturnRandomNumber"); // Push the function onto the stack

		if (lua_isfunction(L, -1))
		{
			if (CheckLua(L, lua_pcall(L, 0, 1, 0))) // Call it with 0 arguments, expecting 1 return value
			{
				if (lua_isnumber(L, -1))
				{
					float result = (float)lua_tonumber(L, -1);
					//std::cout << "[C++] Result from Lua: " << result << "\n";
				}
				lua_pop(L, 1); // Clean up result
			}
		}
		else
		{
			std::cerr << "[C++] Error: ReturnRandomNumber is not defined or not a function.\n";
			lua_pop(L, 1); // Clean the nil or wrong type from the stack
		}
	}

	TestSound();
}

// ----------------------
//       CORE LOOP
// ----------------------

void Game::Update()
{
	Time();

	/*
	SDL_GetMouseState(&mouseX, &mouseY);
	mouseX = mouseX - SCREEN_WIDTH / 2;
	mouseY = mouseY - SCREEN_HEIGHT / 2;

	// TODO: For now shooting is only manual state with hardcoded Transform
	// TODO: Make it follow nearest enemy
	Transform shootDirection;

	float aimRadius = 50.0f; // TODO: what should this be?
	glm::vec2 aimInput(controllerAxisRightX, -controllerAxisRightY);

	// If joystick is tilted
	if (glm::length(aimInput) > 0.1f)
	{
		aimInput = glm::normalize(aimInput);
		glm::vec2 center = glm::vec2(player->segments[player->creatureLenght / 2]->transform.position);

		glm::vec2 offset = aimInput * aimRadius;
		glm::vec2 target = center + offset;

		shootDirection.setPosition(target.x, target.y);
	}
	else
	{
		// Default to forward or center if no input
		shootDirection.setPosition(
			player->segments[player->creatureLenght / 2]->transform.position.x,
			player->segments[player->creatureLenght / 2]->transform.position.y
		);
	}

	player->Update(shootDirection, deltaTime);
	for (auto& creature : creatures)
	{
		creature->AI();
		creature->Update(*origin, deltaTime);
	}

	testEmmiter->Update(deltaTime);

	activeCamera->UpdateCamera(&player->segments[0]->transform, player->segments[0]->direction, deltaTime, isPlayerMoving);

	glm::vec2 focus = glm::vec2(player->segments[0]->transform.position.x, player->segments[0]->transform.position.y);
	activeCamera->updateProjection(focus);

	//CollisionSolver::getInstance()->SolveCollisions();
	*/
}

// ----------------------
//        RENDERING
// ----------------------

void Game::Render()
{
	// Bind framebuffer, viewport & clear buffers
	glBindFramebuffer(GL_FRAMEBUFFER, RenderingBuffer->FBO);
	RenderingBuffer->shader->use();
	glUniform1i(glGetUniformLocation(RenderingBuffer->shader->ID, "screenTexture"), 0); 
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glClearColor(0.0f, 0.1f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);

	/*
	// ----- Render Calls -----
	player->Render();

	for (auto& creature : creatures)
		creature->Render();

	testEmmiter->Render(activeCamera);

	// ----- Debug Render Calls -----
	if (UserInterface::getInstance()->renderDebugInfo)
	{
		player->RenderDebug();

		for (auto& creature : creatures)
			creature->RenderDebug();
	}
	*/

	// Draw framebuffer to screen
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	RenderingBuffer->DrawRenderBuffer(timeElapsed);

	// UI
	if (isEditorOn) UI->EngineEditor(player, RenderingBuffer, activeCamera, &scene);
	else UI->Game();

	// Present
	SDL_GL_SwapWindow(window);
}

// ----------------------
//     EVENTS & INPUT
// ----------------------

void Game::HandleEvents()
{
	SDL_Event event;
	while (SDL_PollEvent(&event)) // Loop through ALL events
	{
		ImGui_ImplSDL2_ProcessEvent(&event);

		switch (event.type)
		{
			// ----- Keyboard Events -----
			case SDL_QUIT:
				isRunning = false;
				break;

			case SDL_KEYDOWN:
				if (!event.key.repeat && !keyPressedOnce[event.key.keysym.scancode])
				{
					keyPressedOnce[event.key.keysym.scancode] = true;

					if (event.key.keysym.scancode == SDL_SCANCODE_P)
					{
						SpawnCreature();
					}

					if (event.key.keysym.scancode == SDL_SCANCODE_E)
					{
						testEmmiter->Emit(testProps);
					}

					if (event.key.keysym.scancode == SDL_SCANCODE_F)
					{
						player->Shoot();
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

			// ----- Controller Events -----
			case SDL_CONTROLLERBUTTONDOWN:
				if (!controllerButtonPressed[event.cbutton.button])
				{
					controllerButtonPressed[event.cbutton.button] = true;

					if (event.cbutton.button == SDL_CONTROLLER_BUTTON_A) Logger::Log("Pressed A");
					if (event.cbutton.button == SDL_CONTROLLER_BUTTON_B) Logger::Log("Pressed B");
					if (event.cbutton.button == SDL_CONTROLLER_BUTTON_X) { Logger::Log("Pressed X"); player->Shoot(); };
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
	isPlayerMoving = false;

	// Key Pressed 
	const Uint8* keyState = SDL_GetKeyboardState(NULL);

	// Controller Pressed
	const float controllerAxisLeftX = NormalizeAxis(SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX));
	const float controllerAxisLeftY = NormalizeAxis(SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY));
	controllerAxisRightX = NormalizeAxis(SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTX));
	controllerAxisRightY = NormalizeAxis(SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTY));

	// Player Keyboard Movement:
	// TODO: take into account diagonal movement
	// TODO: use command pattern
	if (keyState[SDL_SCANCODE_W]) { player->MoveNorth(); isPlayerMoving = true; };
	if (keyState[SDL_SCANCODE_S]) { player->MoveSouth(); isPlayerMoving = true; };
	if (keyState[SDL_SCANCODE_D]) { player->MoveEast(); isPlayerMoving = true; };
	if (keyState[SDL_SCANCODE_A]) { player->MoveWest(); isPlayerMoving = true; };

	// PLayer Controller Movement:
	bool controllerMoved = fabs(controllerAxisLeftX) > 0.1f || fabs(controllerAxisLeftY) > 0.1f;
	if (controllerMoved)
	{
		player->segments[0]->transform.position.x += player->movementSpeed * controllerAxisLeftX * deltaTime;
		player->segments[0]->transform.position.y -= player->movementSpeed * controllerAxisLeftY * deltaTime;
		isPlayerMoving = true;
	}
}

// ----------------------
//       SHUTDOWN
// ----------------------

void Game::Shutdown()
{
	lua_close(L);
}