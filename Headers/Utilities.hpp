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
#include "Line.hpp"
#include "Logger.hpp"

static inline float NormalizeAxis(Sint16 value, float deadZone = 8000.0f)
{
	if (abs(value) < deadZone)
		return 0.0f;

	// Convert to [-1.0, 1.0] range
	float result = value / 32767.0f;

	// Optional: Scale to start at 0.0 right after dead zone
	float adjusted = (abs(result) - (deadZone / 32767.0f)) / (1.0f - (deadZone / 32767.0f));
	return (value > 0) ? adjusted : -adjusted;
};