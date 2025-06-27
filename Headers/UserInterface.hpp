#pragma once

#include <imgui-master/imgui.h>
#include <imgui-master/imgui_impl_sdl2.h>
#include <imgui-master/imgui_impl_sdlrenderer2.h>
#include <imgui-master/imgui_impl_opengl3.h>
#include <imgui-master/imgui_internal.h>
#include <string>
#include <map>

#include "Common.hpp"
#include "Creature.hpp"
#include "FrameBuffer.hpp"
#include "Character.hpp"

#include "Scene.hpp"

#include "imgui-master/ImGuizmo.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <commdlg.h>

class UserInterface
{
private:
	static UserInterface* ui_Instance;

	UserInterface();

public:
	// Prevent copying or assignment 
	UserInterface(const UserInterface&) = delete;
	UserInterface& operator=(const UserInterface&) = delete;

	// Get the instance
	static UserInterface* getInstance()
	{
		if (ui_Instance == nullptr)
		{
			ui_Instance = new UserInterface();
		}

		return ui_Instance;
	}

	unsigned int textVAO, textVBO;

	ImGuiWindowFlags placeholderWindowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground;
	bool open = true;

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse;

	bool showPostProccesed = false;
	bool renderDebugInfo = true;

	void EngineEditor(FrameBuffer* fbo, Camera* maincam, Scene* sceneRef);
	void Start();
	void Init(SDL_Window* windowArg, void* glContextArg);
	void Shutdown(Scene* sceneRef);
	void DockSpace();
	void GameViewport(GLuint fboID, Camera* maincam, Scene* sceneRef);
	void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.f, float columnWidth = 80.f);
	void Style();
	void CreatureMenu(Creature* creatureArg);
	void CameraMenu(Creature* creatureArg, FrameBuffer* fbo, Camera* maincam);
	void Logger();
	void Hierarchy(Scene* sceneRef);
	void DrawEntity();
	void PropertiesPanel(Scene* sceneRef);
	void HeaderBar();

	// For Sprite Component
	std::string OpenFileDialog();
	std::string fileName = "Empty";

	bool creatureMenu = false;

	entt::entity selectedHierarchyItem;
	std::vector<entt::entity> entitiesToDestroy;

};