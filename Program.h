#pragma once
#include <glad/glad.h>
#include <SDL.h>
#include <glm-master/glm/glm.hpp>
#include <imgui-master/imgui.h>
#include <imgui-master/imgui_impl_sdl2.h>
#include <imgui-master/imgui_impl_sdlrenderer2.h>
#include <imgui-master/imgui_impl_opengl3.h>
#include <iostream>
#include <map>
#include <unordered_map>

#include "Line.h"

#include "Camera.h"
#include "ResourceManager.h"
#include "FrameBuffer.h"


class Program
{
public:

	bool isRunning;

	virtual void Init(const char* title, int x, int y, int width, int height, bool fullscreen) = 0;
	virtual void Update() = 0;
	virtual void Render() = 0;
	virtual void HandleEvents() = 0;
	virtual void Quit() = 0;

	virtual bool Running() { return isRunning; };

private:
};