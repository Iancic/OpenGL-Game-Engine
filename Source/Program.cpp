#include "Program.hpp"

Program::Program(const char* title)
{
	FMOD::System* system = nullptr;
	FMOD_RESULT result;

	// Create FMOD system
	result = FMOD::System_Create(&system);
	if (result != FMOD_OK) {
		std::cerr << "FMOD error! " << FMOD_ErrorString(result) << std::endl;
	}

	// Initialize FMOD system
	result = system->init(512, FMOD_INIT_NORMAL, nullptr);
	if (result != FMOD_OK) {
		std::cerr << "FMOD init error! " << FMOD_ErrorString(result) << std::endl;
	}

	SDL_Init(SDL_INIT_EVERYTHING | SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER);

	for (int i = 0; i < SDL_NumJoysticks(); ++i)
	{
		if (SDL_IsGameController(i))
		{
			SDL_GameController* controller = SDL_GameControllerOpen(i);
			if (controller)
			{
				Logger::Log(SDL_GameControllerName(controller));
			}
			else
			{
				Logger::Log("Could not open gamecontroller");
			}
		}
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	int flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
	window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, flags);
	glContext = SDL_GL_CreateContext(window);

	gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);

	if (window) isRunning = true;

	InputSystem = new Input();
	Resources->loadResources();
	UI = UserInterface::getInstance();
	RenderingBuffer = new FrameBuffer();
	UI->Init(window, glContext);
	Init();
}

Program::~Program()
{
	Shutdown();
}

void Program::PreRender()
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
}

void Program::PostRender()
{
	// Draw framebuffer to screen
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	RenderingBuffer->DrawRenderBuffer(timeElapsed);

	// Engine Editor UI
	if (isEditorOn && !UI->isViewportMaximized) UI->EngineEditor(RenderingBuffer);

	// SDL Draw
	SDL_GL_SwapWindow(window);
}

void Program::Quit()
{
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void Program::Time()
{
	Uint32 currentTicks = SDL_GetTicks();

	timeElapsed = currentTicks;

	// Cap the FPS if not uncapped
	if (!UNCAPPED)
	{
		int timeToWait = MIL_PER_FRAME - (currentTicks - milisecondsPreviousFrame);
		if (timeToWait > 0)
			SDL_Delay(timeToWait);

		// Update the ticks after delay
		currentTicks = SDL_GetTicks();
	}

	deltaTime = (currentTicks - milisecondsPreviousFrame) / 1000.f;
	milisecondsPreviousFrame = currentTicks;
}

int Program::GetFPS()
{
	return (deltaTime > 0.0) ? static_cast<int>(1.0 / deltaTime) : 0;
}

int Program::GetMS()
{
	return static_cast<int>(deltaTime * 1000.0);;
}