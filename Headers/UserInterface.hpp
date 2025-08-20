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
    bool showPostProccesed = false;
    bool renderDebugInfo = true;
    bool creatureMenu = false;

    bool openScenePopup = false, openScriptPopup = false; // these need to be created since pop up inside pop up does not work
    bool isInDragRegion = false;

    // Trigger changes value on checkbox and prepares wheter fullscreen should be on runtime.
    bool isViewportMaximized = false, triggerMaximized = false;

    // Both of these are for the build window
    bool buildTab = false;
    int draggedIndex = -1;
    float selectableHeight = 25.0f;
    int draggedEntityIndex = -1, currentIndex = 0;

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse;

	void EngineEditor(FrameBuffer* fbo);
	void Start();
	void Init(SDL_Window* windowArg, void* glContextArg);
	void Resize(SDL_Window* windowArg);
	void Shutdown(Scene* sceneRef);
	void DockSpace();
	void GameViewport(GLuint fboID, Camera* maincam, Scene* sceneRef);
	void Style();
    void EmitterMenu(Emitter* emitterArg);
	void CreatureMenu(Creature* creatureArg);
	void CameraMenu(FrameBuffer* fbo, Camera* maincam);
	void Logger();
	void Hierarchy(Scene* sceneRef);
	void PropertiesPanel(Scene* sceneRef);
	void HeaderBar();

	bool IconButtonWithText(ImTextureID icon, const char* label, const ImVec2& iconSize, const ImVec2& padding = ImVec2(1, 1));

	void TransparentBackground();
	void EndTransparentBackground();

	enum class GameState { PLAYING, STOPPED };
	GameState gameState = GameState::STOPPED;

	Camera* cameraRef = nullptr;

	std::string OpenFileDialog(const FILE_TYPE typeArg);
	std::string fileName = "Empty";


	entt::entity selectedHierarchyItem;
	std::vector<entt::entity> entitiesToDestroy;

	enum class FileType { SCRIPT, SCENE };
	void NewFile(const FileType type, const std::string name);

	const float headerHeight = 30.0f;

	// For adding animations
	int animFrameWidth;
	int spriteCountHor, spriteCountVer;


	void DrawEntityNode(entt::entity entity, Scene* sceneRef, entt::entity& selectedEntity, int& draggedEntityIndex, float selectableHeight);
	void DrawHierarchyPanel(Scene* sceneRef, entt::entity& selectedEntity);

    void DrawVec3ControlFromMatrix(const std::string& label, glm::mat4& matrix, const std::string& type, float resetValue = 0.0f, float columnWidth = 100.0f)
    {
        glm::vec3 value;

        // Initial extraction
        if (type == "Translation")
        {
            value = glm::vec3(matrix[3]);
        }
        else if (type == "Scale")
        {
            value.x = glm::length(glm::vec3(matrix[0]));
            value.y = glm::length(glm::vec3(matrix[1]));
            value.z = glm::length(glm::vec3(matrix[2]));
        }
        else if (type == "Rotation")
        {
            glm::vec3 translation, scale, rotationEuler;
            glm::vec4 perspective;
            glm::quat rotationQuat;
            glm::vec3 skew;

            glm::decompose(matrix, scale, rotationQuat, translation, skew, perspective);
            value = glm::degrees(glm::eulerAngles(rotationQuat)); // Convert to degrees for ImGui
        }

        // UI Drawing
        ImGui::PushID((label + type).c_str());
        ImGui::BeginTable("Vec3Control", 4, ImGuiTableFlags_SizingFixedFit);
        ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, columnWidth);
        ImGui::TableSetupColumn("X");
        ImGui::TableSetupColumn("Y");
        ImGui::TableSetupColumn("Z");

        ImGui::TableNextRow();

        // Reset Button
        ImGui::TableSetColumnIndex(0);
        if (ImGui::Button((label + "##Reset" + type).c_str()))
            value = glm::vec3(resetValue);

        // Editable fields
        bool changed = false;

        ImGui::TableSetColumnIndex(1);
        ImGui::SetNextItemWidth(70);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4{ 0.8f, 0.1f, 0.15f, 0.3f });
        changed |= ImGui::DragFloat(("##X" + type).c_str(), &value.x, 0.1f);
        ImGui::PopStyleColor();

        ImGui::TableSetColumnIndex(2);
        ImGui::SetNextItemWidth(70);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4{ 0.2f, 0.7f, 0.2f, 0.3f });
        changed |= ImGui::DragFloat(("##Y" + type).c_str(), &value.y, 0.1f);
        ImGui::PopStyleColor();

        ImGui::TableSetColumnIndex(3);
        ImGui::SetNextItemWidth(70);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4{ 0.1f, 0.25f, 0.8f, 0.3f });
        changed |= ImGui::DragFloat(("##Z" + type).c_str(), &value.z, 0.1f);
        ImGui::PopStyleColor();

        ImGui::EndTable();
        ImGui::PopID();

        // Recompose only if user changed values
        if (!changed) return;

        if (type == "Translation")
        {
            matrix[3] = glm::vec4(value, 1.0f);
        }
        else if (type == "Scale")
        {
            glm::vec3 x = glm::normalize(glm::vec3(matrix[0]));
            glm::vec3 y = glm::normalize(glm::vec3(matrix[1]));
            glm::vec3 z = glm::normalize(glm::vec3(matrix[2]));

            matrix[0] = glm::vec4(x * value.x, 0.0f);
            matrix[1] = glm::vec4(y * value.y, 0.0f);
            matrix[2] = glm::vec4(z * value.z, 0.0f);
        }
        else if (type == "Rotation")
        {
            glm::vec3 translation, scale, skew;
            glm::vec4 perspective;
            glm::quat dummyRot;

            glm::decompose(matrix, scale, dummyRot, translation, skew, perspective);

            glm::quat rotationQuat = glm::quat(glm::radians(value));
            glm::mat4 rotationMat = glm::toMat4(rotationQuat);
            glm::mat4 translationMat = glm::translate(glm::mat4(1.0f), translation);
            glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), scale);

            matrix = translationMat * rotationMat * scaleMat;
        }
    }


};