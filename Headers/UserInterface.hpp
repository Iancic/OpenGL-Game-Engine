#pragma once
#include <imgui-master/imgui.h>
#include <imgui-master/imgui_impl_sdl2.h>
#include <imgui-master/imgui_impl_sdlrenderer2.h>
#include <imgui-master/imgui_impl_opengl3.h>
#include <string>
#include <map>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "Common.hpp"
#include "Creature.hpp"
#include "FrameBuffer.hpp"
#include "Character.hpp"

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

	// -------- IMGUI --------

	ImGuiWindowFlags placeholderWindowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground;
	bool open = true;

	bool showPostProccesed = false;
	bool renderDebugInfo = true;

	void EngineEditor(Creature* creatureArg, FrameBuffer* fbo, Camera* maincam);
	void Start();
	void Init(SDL_Window* windowArg, void* glContextArg);
	void Shutdown();
	void DockSpace();
	void GameViewport(GLuint fboID);
	void Style();
	void CreatureMenu(Creature* creatureArg);
	void CameraMenu(Creature* creatureArg, FrameBuffer* fbo, Camera* maincam);
	void Gameplay();
	void Logger();
	void HeaderBar();

	// -------- Game UI --------

	ImGuiWindowFlags editorWindowFlags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize;

	std::map<char, Character> Characters;

	void RenderText(const std::string& text, float x, float y, float scale, const glm::vec3& color);

	void Game();

	//

};