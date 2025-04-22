#include "Program.h"

Program::Program(const char* title, int x, int y, int width, int height, bool fullscreen)
{
	int flags = SDL_WINDOW_OPENGL;

	if (fullscreen) flags = SDL_WINDOW_FULLSCREEN;

	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	window = SDL_CreateWindow(title, x, y, width, height, flags);
	glContext = SDL_GL_CreateContext(window);

	gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);

	if (window) isRunning = true;

	Init();
}

Program::~Program()
{
	Shutdown();
}

void Program::Quit()
{
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void Program::Time()
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
