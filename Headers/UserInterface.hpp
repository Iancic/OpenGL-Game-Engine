#pragma once
#include "Utilities.hpp"
#include "Character.hpp"
#include "Input.hpp"
#include "Scene.hpp"
#include "Icons.h"

class UserInterface
{
private:
	static UserInterface* ui_Instance;

	UserInterface();

public:
	// Prevent copying or assignment 
	UserInterface(const UserInterface&) = delete;
	UserInterface& operator=(const UserInterface&) = delete;

	enum class FILE_TYPE { SCRIPT, IMAGE };

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

	void EngineEditor(FrameBuffer* fbo);
	void Start();
	void Init(SDL_Window* windowArg, void* glContextArg);
	void Resize(SDL_Window* windowArg);
	void Shutdown(Scene* sceneRef);
	void DockSpace();
	void GameViewport(GLuint fboID, Camera* maincam, Scene* sceneRef);
	void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.f, float columnWidth = 80.f);
	void Style();
	void CreatureMenu(Creature* creatureArg);
	void CameraMenu(/*Creature* creatureArg, */ FrameBuffer* fbo, Camera* maincam);
	void Logger();
	void Hierarchy(Scene* sceneRef);
	void PropertiesPanel(Scene* sceneRef);
	void HeaderBar();

	bool IconButtonWithText(ImTextureID icon, const char* label, const ImVec2& iconSize, const ImVec2& padding = ImVec2(1, 1));

	void TransparentBackground();
	void EndTransparentBackground();

	enum class GameState { PLAYING, STOPPED };
	GameState gameState = GameState::STOPPED;

	// Trigger changes value on checkbox and prepares wheter fullscreen should be on runtime.
	bool isViewportMaximized = false, triggerMaximized = false; 

	Camera* cameraRef = nullptr;

	std::string OpenFileDialog(const FILE_TYPE typeArg);
	std::string fileName = "Empty";

	bool creatureMenu = false;

	entt::entity selectedHierarchyItem;
	std::vector<entt::entity> entitiesToDestroy;

	enum class FileType { SCRIPT, SCENE };
	void NewFile(const FileType type, const std::string name);

	bool openScenePopup = false, openScriptPopup = false; // these need to be created since pop up inside pop up does not work
	bool isInDragRegion = false;
	const float headerHeight = 30.0f;

	// For adding animations
	int animFrameWidth;
	int spriteCountHor, spriteCountVer;

	// Both of these are for the build window
	bool buildTab = false;
	int draggedIndex = -1;
	float selectableHeight = 25.0f;
	int draggedEntityIndex = -1, currentIndex = 0;

	void DrawEntityNode(entt::entity entity, Scene* sceneRef, entt::entity& selectedEntity, int& draggedEntityIndex, float selectableHeight);
	void DrawHierarchyPanel(Scene* sceneRef, entt::entity& selectedEntity);
};