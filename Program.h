#pragma once
#include <glad/glad.h> // Link OpenGL Functions
#include <SDL.h> // SDL: Windowing, Input, Audio
#include <glm-master/glm/glm.hpp> // GLM: Math
#include <imgui-master/imgui.h> // ImGui
#include <imgui-master/imgui_impl_sdl2.h>
#include <imgui-master/imgui_impl_sdlrenderer2.h>
#include <imgui-master/imgui_impl_opengl3.h>

// C++ Utilities
#include <iostream>
#include <map>
#include <unordered_map>

// Engine Utilities Re-usable Across Different Programs
#include "Line.h"

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
	virtual void Quit();
	virtual void Time();

	virtual bool Running() { return isRunning; };

protected:

	SDL_GLContext glContext;
	SDL_Window* window;

	bool isRunning;

	// Time
	int milisecondsPreviousFrame;
	double deltaTime;
	float timeElapsed;
};