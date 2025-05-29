#pragma once
#include "Utilities.hpp"

class Program
{
public:
	Program(const char* title, int x, int y, int width, int height, bool fullscreen);
	virtual ~Program();

	virtual void Init() {};
	virtual void Shutdown() {};
	virtual void Update() = 0;
	virtual void Render() = 0;
	virtual void HandleEvents() = 0;
	virtual void HandleInput() = 0;
	virtual void Quit();
	virtual void Time();

	virtual bool Running() { return isRunning; };

	int GetFPS();
	int GetMS();

protected:

	SDL_GLContext glContext;
	SDL_Window* window;

	bool isRunning;

	// Time
	int milisecondsPreviousFrame;
	double deltaTime;
	float timeElapsed;
};