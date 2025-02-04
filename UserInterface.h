#pragma once
#include <imgui-master/imgui.h>
#include <imgui-master/imgui_impl_sdl2.h>
#include <imgui-master/imgui_impl_sdlrenderer2.h>
#include <imgui-master/imgui_impl_opengl3.h>
#include <string>

#include "common.h"
#include "Creature.h"

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

	ImGuiWindowFlags windowflags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground;
	bool open = true;

	void Start();
	void Init(SDL_Window* windowArg, void* glContextArg);
	void Shutdown();
	void Style();

	void InfoHUD(Creature* creatureArg);
	void UpgradeMenu(Creature* creatureArg);
	void ClassPopUp();
	void LevelBar(Creature* creatureArg);

};