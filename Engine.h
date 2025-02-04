#pragma once

#include <glad/glad.h> // Link OpenGL Functions
#include <SDL.h> // SDL: Windowing, Input, Audio
#include <glm-master/glm/glm.hpp> // GLM: Math

#include <iostream>
#include <map>
#include <unordered_map>

#include "Program.h"

#include "Line.h"
#include "Creature.h"
#include "Camera.h"
#include "ResourceManager.h"
#include "FrameBuffer.h"
#include "Star.h"

class Engine
{
public:
	Engine(Program* programArg);
	~Engine() = default;

	void Run();
	
private:

	Program* activeProgram = nullptr;


};