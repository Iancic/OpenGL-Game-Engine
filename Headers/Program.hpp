#pragma once
#include "Utilities.hpp"

class Program
{
public:
	Program(const char* title);
	virtual ~Program();

	virtual void Init() {};
	virtual void Shutdown() {};
	virtual void Update() = 0;
	virtual void PreRender();
	virtual void Render() = 0;
	virtual void PostRender();
	virtual void HandleEvents() = 0;
	virtual void HandleInput() = 0;
	virtual void Quit();
	virtual void Time();

	virtual bool Running() { return isRunning; };

	int GetFPS();
	int GetMS();

protected:

	bool isRunning;
	bool isEditorOn = true;

	
	ResourceManager* Resources = ResourceManager::getInstance();
	UserInterface* UI = nullptr;

	FrameBuffer* RenderingBuffer = nullptr;
	SDL_GLContext glContext;
	SDL_Window* window;

	int milisecondsPreviousFrame;
	double deltaTime;
	float timeElapsed;
};