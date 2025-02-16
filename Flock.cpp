#include "Flock.h"

Flock::Flock()
{
	ActiveCamera = new Camera(45.f, glm::vec3(0, 0, 0), SCREEN_WIDTH, SCREEN_HEIGHT);
	boidPool.push_back(new Boid());
	cameraTarget->setPosition(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
}

Flock::~Flock()
{
}

void Flock::Init(const char* title, int x, int y, int width, int height, bool fullscreen)
{
	int flags = SDL_WINDOW_OPENGL;

	if (fullscreen)
	{
		flags = SDL_WINDOW_FULLSCREEN;
	}

	// Initialize SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		std::cerr << "SDL Initialization Error: " << SDL_GetError() << std::endl;
		return;
	}

	// Joy Stick
	if (SDL_INIT_JOYSTICK < 0)
	{
		std::cout << "SDL Controller Not Initialized. " << std::endl;
	}

	if (SDL_NumJoysticks() < 1)
	{
		std::cout << "" << std::endl;
	}
	else
	{
		joystickPlayer1 = SDL_JoystickOpen(0);
		std::cout << "Joystick ID: " << SDL_JoystickInstanceID(joystickPlayer1) << std::endl;
	}


	// Set OpenGL version before creating the context
	if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3) != 0 || SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3) != 0)
	{
		std::cerr << "Error setting OpenGL version: " << SDL_GetError() << std::endl;
		return;
	}

	window = SDL_CreateWindow(title, x, y, width, height, flags);

	// Create OpenGL context for the window
	glContext = SDL_GL_CreateContext(window);

	// Initialize OpenGL loader (GLAD)
	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
	{
		std::cerr << "Failed to initialize OpenGL loader!" << std::endl;
		return;
	}
	if (window)
	{
		isRunning = true;
	}

	// Initialize ImGui
	UI->Init(window, glContext);

	Resources->loadResources(); // Make Sure it's before setting up OpenGl context

	// Everything that gets inialized with the use of Resources:
	RenderingBuffer = new FrameBuffer();
}

void Flock::Update()
{
	Time();

	ActiveCamera->UpdateCamera(cameraTarget, 0.3f); // Update last value for lerp

	for (auto boid : boidPool)
	{
		boid->Update(boidPool);
	}

	int mouseX, mouseY;
	SDL_GetMouseState(&mouseX, &mouseY);
}

void Flock::Render()
{
	RenderingBuffer->shader->use();

	glUniform1i(glGetUniformLocation(RenderingBuffer->shader->ID, "screenTexture"), 0);
	glBindFramebuffer(GL_FRAMEBUFFER, RenderingBuffer->FBO);

	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	glClearColor(0.f, 0.f, 0.f, 1.0f);

	// -------------- Rendering --------------

	for (auto boid : boidPool)
	{
		boid->Render(ActiveCamera);
	}


	// ---------------------------------------

	RenderingBuffer->DrawRenderBuffer(timeElapsed, false); // Render FBO
	SDL_GL_SwapWindow(window);  // Swap buffers
}

void Flock::HandleEvents()
{
	SDL_Event event;
	SDL_PollEvent(&event);
	ImGui_ImplSDL2_ProcessEvent(&event);

	keyState = SDL_GetKeyboardState(NULL);

	// Mouse
	if (event.type == SDL_MOUSEBUTTONDOWN)
	{
		if (SDL_BUTTON_LEFT)
		{

		}
	}

	// Keyboard
	if (event.type == SDL_QUIT)
	{
		isRunning = false;
	}

	if (event.type == SDL_KEYDOWN)
	{
		keyPressed[event.key.keysym.scancode] = true;
	}

	if (event.type == SDL_KEYUP)
		keyPressed[event.key.keysym.scancode] = false;

	if (keyPressed[SDL_SCANCODE_SPACE])
	{
	}

	if (keyPressed[SDL_SCANCODE_ESCAPE])
		isRunning = false; // Quit Game
}

void Flock::Quit()
{
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void Flock::Time()
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

void Flock::UserInterface(Creature* creatureArg)
{
	// TO DO: Add Proper UI
}