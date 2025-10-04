#include "precomp.h"
#include "Program.hpp"

Program::Program(const char* title)
{
	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS | SDL_INIT_JOYSTICK | SDL_INIT_GAMEPAD))
	{
		Logger::Error(SDL_GetError());
	}

	// OpenGL Attributes.
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	// STBI image.
	stbi_set_flip_vertically_on_load(true);

	// Create SDL window.
	const uint32_t flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY;
	window = SDL_CreateWindow(title, SCREEN_WIDTH, SCREEN_HEIGHT, flags);

	// Center window.
	SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

	// OpenGL Context.
	glContext = SDL_GL_CreateContext(window);
	if (!glContext) 
	{
		Logger::Log(SDL_GetError());
		SDL_DestroyWindow(window);
		SDL_Quit();
		return;
	}
	SDL_GL_MakeCurrent(window, glContext);
	SDL_GL_SetSwapInterval(1);

	// Load GL with GLAD
	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) 
	{
		Logger::Log("Failed to load OpenGL via GLAD");
		SDL_GL_DestroyContext(glContext);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return;
	}

	// Start the program.
	isRunning = true;

	// Initialize necessary systems.
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
	isRunning = false;
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void Program::Time()
{
	Uint64 currentTicks = SDL_GetTicks();

	timeElapsed = currentTicks;

	/*
	// Cap the FPS if not uncapped
	if (!UNCAPPED)
	{
		int timeToWait = MIL_PER_FRAME - (currentTicks - milisecondsPreviousFrame);
		if (timeToWait > 0.f)
			SDL_Delay(timeToWait);

		// Update the ticks after delay
		currentTicks = SDL_GetTicks();
	}
	*/

	deltaTime = static_cast<float>(currentTicks - milisecondsPreviousFrame) / 1000.0f;
	milisecondsPreviousFrame = currentTicks;
}

int Program::GetFPS()
{
	return (deltaTime > 0.0) ? static_cast<int>(1.0f / deltaTime) : 0.f;
}

int Program::GetMS()
{
	return static_cast<int>(deltaTime * 1000.0f);
}