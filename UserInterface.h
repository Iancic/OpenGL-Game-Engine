#pragma once
#include <imgui-master/imgui.h>
#include <imgui-master/imgui_impl_sdl2.h>
#include <imgui-master/imgui_impl_sdlrenderer2.h>
#include <imgui-master/imgui_impl_opengl3.h>
#include <string>

#include "common.h"
#include "Player.h"

class UserInterface
{
private:
	static UserInterface* ui_Instance;

	UserInterface() {};

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

	// -------- IMGUI --------

	ImGuiWindowFlags placeholderWindowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground;
	bool open = true;

	void ImGui(Player* creatureArg, GLuint fboID, Camera* maincam);
	void Start();
	void Init(SDL_Window* windowArg, void* glContextArg);
	void Shutdown();
	void DockSpace();
	void GameViewport(GLuint fboID);
	void Style();
	void CreatureMenu(Player* creatureArg);
	void CameraMenu(Player* creatureArg, Camera* maincam);
	void Logger();
	void HeaderBar();

	// -------- Game UI --------

	ImGuiWindowFlags editorWindowFlags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize;

	void Game();

	//

};