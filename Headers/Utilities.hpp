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
#include <thread>

// Engine Utilities Re-usable Across Different Programs
#include "Line.hpp"
#include "Logger.hpp"

#include "fmod/include/fmod.hpp"
#include "fmod/include/fmod_errors.h"

#include "entt/entt.hpp"

#include "common.hpp"
#include "Creature.hpp"
#include "Camera.hpp"
#include "ResourceManager.hpp"
#include "FrameBuffer.hpp"
#include "UserInterface.hpp"
#include "IKChain.hpp"
#include "Logger.hpp"
#include "BoxCollider.hpp"
#include "CollisionSolver.hpp"
#include "Emitter.hpp"
#include "LuaUtilities.hpp"

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

// Just To Test if FMOD works
static inline void TestSound()
{
    FMOD::System* system = nullptr;
    FMOD::Sound* sound = nullptr;
    FMOD::Channel* channel = nullptr;
    FMOD_RESULT result;

    // Create FMOD system
    result = FMOD::System_Create(&system);
    if (result != FMOD_OK || !system) {
        std::cerr << "System creation failed: " << FMOD_ErrorString(result) << std::endl;
        return;
    }

    // Initialize
    result = system->init(512, FMOD_INIT_NORMAL, nullptr);
    if (result != FMOD_OK) {
        //std::cerr << "FMOD init failed: " << FMOD_ErrorString(result) << std::endl;
        return;
    }

    // Load sound
    result = system->createSound("Assets/Audio/test.wav", FMOD_DEFAULT, nullptr, &sound);
    if (result != FMOD_OK || !sound) {
        std::cerr << "Sound load failed: " << FMOD_ErrorString(result) << std::endl;
        system->release();
        return;
    }

    // Play sound
    result = system->playSound(sound, nullptr, false, &channel);
    if (result != FMOD_OK || !channel) {
        std::cerr << "Sound play failed: " << FMOD_ErrorString(result) << std::endl;
        sound->release();
        system->release();
        return;
    }

    //std::cout << "Playing sound..." << std::endl;

    // Wait for playback to finish
    bool isPlaying = true;
    while (isPlaying) {
        system->update();
        channel->isPlaying(&isPlaying);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    //std::cout << "Playback finished." << std::endl;

    // Clean up using FMOD's release methods
    sound->release();
    system->close();
    system->release();
}