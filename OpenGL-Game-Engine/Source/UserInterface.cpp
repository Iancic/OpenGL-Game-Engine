#include "precomp.h"
#include "UserInterface.hpp"
#include "Logger.hpp"
#include "Game.hpp"
#include "Icons.h"

UserInterface* UserInterface::ui_Instance = nullptr;

UserInterface::UserInterface()
{
	glGenVertexArrays(1, &textVAO);
	glGenBuffers(1, &textVBO);
	glBindVertexArray(textVAO);
	glBindBuffer(GL_ARRAY_BUFFER, textVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void UserInterface::EngineEditor(FrameBuffer* fbo)
{
	cameraRef = Game::getInstance()->activeCamera;

	Start();
	Style();
	DockSpace();	

	if (showPostProccesed)
		GameViewport(fbo->postprocessedTexture, cameraRef, Game::getInstance()->activeScene);
	else
		GameViewport(fbo->framebufferTexture, cameraRef, Game::getInstance()->activeScene);

	CameraMenu(fbo, cameraRef);
	Logger();
	Hierarchy(Game::getInstance()->activeScene);
	PropertiesPanel(Game::getInstance()->activeScene);
	Shutdown(Game::getInstance()->activeScene);
}

void UserInterface::Start()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();
}

void UserInterface::Init(SDL_Window* windowArg, void* glContextArg)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGuiIO& io = ImGui::GetIO();
	int w, h;
	SDL_GetWindowSize(windowArg, &w, &h);
	io.DisplaySize = ImVec2((float)w, (float)h);
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_DockingEnable;;
	io.FontGlobalScale = 1.f;
	io.ConfigWindowsMoveFromTitleBarOnly = false;

	// Font and icons.
	io.Fonts->AddFontFromFileTTF("Assets/fonts/MonaspaceNeonFrozen-Regular.ttf", 16.0f);
	LoadIconFont(io, "Assets/fonts/icons.ttf", 16.0f); // merges icons

	// High-DPI aware display.
	int pxW = 0, pxH = 0;
	SDL_GetWindowSizeInPixels(windowArg, &pxW, &pxH);
	io.DisplaySize = ImVec2((float)pxW, (float)pxH);

	// OpenGL backend.
	const char* glsl_version = "#version 460 core";
	ImGui_ImplSDL3_InitForOpenGL(windowArg, glContextArg);
	ImGui_ImplOpenGL3_Init(glsl_version);
}

void UserInterface::Resize(SDL_Window* windowArg)
{
	ImGuiIO& io = ImGui::GetIO();
	int w, h;
	SDL_GetWindowSize(windowArg, &w, &h);
	io.DisplaySize = ImVec2((float)w, (float)h);
}

void UserInterface::Shutdown(Scene* sceneRef)
{
	for (auto entity : entitiesToDestroy)
	{
		sceneRef->DestroyEntity(entity);
	}
	entitiesToDestroy.clear();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UserInterface::DockSpace()
{
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowViewport(viewport->ID);

	window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

	ImGui::Begin("DockSpace_Window", nullptr, window_flags);
	ImGui::PopStyleVar(2);

	ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

	HeaderBar();

	ImGui::End();
}

void UserInterface::GameViewport(GLuint fboID, Camera* maincam, Scene* sceneRef)
{
	ImGui::Begin("Viewport", nullptr, windowFlags);

	// Because I have docking this is required to know to draw imguizmo rect on top
	ImVec2 windowPos = ImGui::GetWindowPos();
	ImVec2 cursorPos = ImGui::GetCursorPos();
	ImVec2 imagePos = ImVec2(windowPos.x + cursorPos.x, windowPos.y + cursorPos.y);
	ImVec2 imageSize = ImVec2(VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

	float buttonWidth = 40.0f;
	float buttonHeight = 25.0f;
	float spacing = ImGui::GetStyle().ItemSpacing.x;

	float totalWidth =   buttonWidth + spacing;
	float panelWidth = ImGui::GetContentRegionAvail().x;
	float startX = (panelWidth - totalWidth) * 0.5f;
	if (gameState == GameState::STOPPED)
	{

		ImGui::SetCursorPosX(startX);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(0, 255, 0, 255));
		if (ImGui::ImageButton("##Start", (ImTextureID)(uintptr_t)ResourceManager::playButton->ID, ImVec2(18, 18)))
		{
			gameState = GameState::PLAYING;
			sceneRef->Start();
			if (triggerMaximized)
				isViewportMaximized = true;
		}
		ImGui::PopStyleColor();
	}
	else
	{
		ImGui::SetCursorPosX(startX);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(255, 0, 0, 255));
		if (ImGui::ImageButton("##Shutdown", (ImTextureID)(uintptr_t)ResourceManager::stopButton->ID, ImVec2(18, 18)))
		{
			gameState = GameState::STOPPED;
			sceneRef->Shutdown();
			isViewportMaximized = false;
		}
		ImGui::PopStyleColor();
	}

	// Following is rendering the current scene name
	std::string activeScene = "Current Scene: " + Game::getInstance()->activeScene->sceneName;

	float textWidth = ImGui::CalcTextSize(activeScene.c_str()).x;

	// Push the next item to the right edge (minus some padding or the text width)
	float regionMaxX = ImGui::GetWindowContentRegionMax().x;
	float padding = 10.0f;

	ImGui::SameLine(regionMaxX - textWidth - padding);
	ImGui::TextColored(ImVec4(1, 1, 1, 1), activeScene.c_str());

	// Render the framebuffer in imgui image
	ImGui::Image((ImTextureID)(uintptr_t)fboID, imageSize, ImVec2(0, 1), ImVec2(1, 0));
	
	// Set imguzimor rect
	ImGuizmo::SetDrawlist();
	ImGuizmo::SetRect(imagePos.x, imagePos.y + buttonHeight, imageSize.x, imageSize.y);

	// TODO: view and projection do not seem to work from main camera
	glm::mat4 viewMatrix = glm::lookAt(glm::vec3(0, 0, 10), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	glm::mat4 projMatrix = maincam->GetProjectionMatrix();//glm::perspective(glm::radians(45.0f), imageSize.x / imageSize.y, 0.1f, 100.0f);

	if (selectedHierarchyItem != entt::null && sceneRef->registry.any_of<TransformComponent>(selectedHierarchyItem))
	{
		auto& transform = sceneRef->registry.get<TransformComponent>(selectedHierarchyItem);
		const auto& hierarchy = sceneRef->registry.get<HierarchyComponent>(selectedHierarchyItem);

		// Copy matrices for ImGuizmo
		float view[16], proj[16], model[16];
		memcpy(view, glm::value_ptr(viewMatrix), sizeof(view));
		memcpy(proj, glm::value_ptr(projMatrix), sizeof(proj));
		memcpy(model, glm::value_ptr(transform.WorldMatrix), sizeof(model));

		ImGuizmo::Manipulate(view, proj, ImGuizmo::TRANSLATE_X | ImGuizmo::TRANSLATE_Y, ImGuizmo::LOCAL, model);

		if (ImGuizmo::IsUsing())
		{
			glm::mat4 updatedWorld = glm::make_mat4(model);

			if (hierarchy.parentID != entt::null && sceneRef->registry.any_of<TransformComponent>(hierarchy.parentID))
			{
				auto& parentTransform = sceneRef->registry.get<TransformComponent>(hierarchy.parentID);
				glm::mat4 parentWorldInverse = glm::inverse(parentTransform.WorldMatrix);
				transform.LocalMatrix = parentWorldInverse * updatedWorld;

				/*
					The gizmo moves the child in world space.

					But your data stores local space.

					You must convert new world transform back into local space.

					This requires multiplying by the inverse of the parent's world transform.

					Visual recap
					Parent at(10, 0, 0) world space
					Child local at(5, 0, 0)
					Child world = Parent(10) + ChildLocal(5) = 15

					After gizmo move :
					Child world = 20
					New child local = 20 - Parent(10) = 10

					If you skip the inverse, you'd wrongly set:

					Child local = 20 (same as world)

					So world would become : 10 (parent)+20 (child) = 30 — wrong!
				*/
			}
			else
			{
				// No parent — local = world
				transform.LocalMatrix = updatedWorld;
			}
		}
	}
	
	ImGui::End();
}

void UserInterface::Style()
{
	ImGuiStyle& style = ImGui::GetStyle();
	ImVec4* colors = style.Colors;

	// Darker, more contrasted tones
	ImVec4 backgroundGray = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
	ImVec4 windowBgGray = ImVec4(0.1f, 0.1f, 0.1f, 1.00f);
	ImVec4 headerBlack = ImVec4(0.075f, 0.075f, 0.075f, 1.00f);
	ImVec4 baseGray = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	ImVec4 hoverGray = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
	ImVec4 activeGray = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);

	// Text
	colors[ImGuiCol_Text] = ImVec4(1.f, 1.f, 1.f, 1.0f);

	// Backgrounds
	colors[ImGuiCol_WindowBg] = windowBgGray;
	colors[ImGuiCol_ChildBg] = windowBgGray;
	colors[ImGuiCol_PopupBg] = windowBgGray;

	// Borders
	colors[ImGuiCol_Border] = baseGray;

	// Headers
	colors[ImGuiCol_Header] = baseGray;
	colors[ImGuiCol_HeaderHovered] = hoverGray;
	colors[ImGuiCol_HeaderActive] = activeGray;

	// Buttons
	colors[ImGuiCol_Button] = baseGray;
	colors[ImGuiCol_ButtonHovered] = hoverGray;
	colors[ImGuiCol_ButtonActive] = activeGray;

	// Frames
	colors[ImGuiCol_FrameBg] = baseGray;
	colors[ImGuiCol_FrameBgHovered] = hoverGray;
	colors[ImGuiCol_FrameBgActive] = activeGray;

	// Scrollbars
	colors[ImGuiCol_ScrollbarBg] = backgroundGray;
	colors[ImGuiCol_ScrollbarGrab] = baseGray;
	colors[ImGuiCol_ScrollbarGrabHovered] = hoverGray;
	colors[ImGuiCol_ScrollbarGrabActive] = activeGray;

	// Title bars
	colors[ImGuiCol_TitleBg] = windowBgGray;
	colors[ImGuiCol_TitleBgActive] = baseGray;
	colors[ImGuiCol_TitleBgCollapsed] = windowBgGray;

	// Resize grips
	colors[ImGuiCol_ResizeGrip] = baseGray;
	colors[ImGuiCol_ResizeGripHovered] = hoverGray;
	colors[ImGuiCol_ResizeGripActive] = activeGray;

	// Tabs
	colors[ImGuiCol_Tab] = baseGray;
	colors[ImGuiCol_TabHovered] = hoverGray;
	colors[ImGuiCol_TabActive] = activeGray;
	colors[ImGuiCol_TabUnfocused] = baseGray;
	colors[ImGuiCol_TabUnfocusedActive] = baseGray;

	// Menus
	colors[ImGuiCol_MenuBarBg] = headerBlack;

	// Checkmarks
	colors[ImGuiCol_CheckMark] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f); // visible light gray

	// Sliders
	colors[ImGuiCol_SliderGrab] = hoverGray;
	colors[ImGuiCol_SliderGrabActive] = activeGray;

	// Roundness
	style.WindowRounding = 1.0f;
	style.FrameRounding = 1.0f;
	style.ScrollbarRounding = 1.0f;
	style.GrabRounding = 1.0f;
	style.TabRounding = 1.0f;
}

void UserInterface::EmitterMenu(Emitter* emitterArg)
{
	//ImGui::Checkbox("Playing", &emitterArg->isEmitting);
	//ImGui::Spacing();

	ImGui::Text("Properties");
	ImGui::SliderFloat4("Color Start", &emitterArg->properties.colorBegin.x, 0.2f, 1.f);
	ImGui::SliderFloat4("Color End", &emitterArg->properties.colorEnd.x, 0.2f, 1.f);

	ImGui::Spacing();
	ImGui::SliderFloat("Velocity X", &emitterArg->properties.velocityVariationX, 1.f, 15.f);
	ImGui::SliderFloat("Velocity Y", &emitterArg->properties.velocityVariationY, 1.f, 15.f);

	ImGui::Spacing();
	ImGui::SliderFloat("Size Begin", &emitterArg->properties.sizeBegin, 1.f, 15.f);
	ImGui::SliderFloat("Size End", &emitterArg->properties.sizeEnd, 1.f, 15.f);
	ImGui::SliderFloat("Size Variation", &emitterArg->properties.sizeVariation, 1.f, 15.f);
	ImGui::SliderFloat("Lifetime", &emitterArg->properties.lifeTime, 0.1f, 10.f);

}

void UserInterface::CreatureMenu(Creature* creatureArg)
{
		ImGui::Checkbox("AI Controller", &creatureArg->AIcontrolled);
		ImGui::Spacing();

		ImGui::Text("Leg Options:");
		ImGui::Checkbox("Gait", &creatureArg->gait);
		ImGui::SliderFloat("Leg Width", &creatureArg->legWidth, 1.f, 5.f);
		ImGui::SliderFloat("Snap Distance", &creatureArg->distanceToSnap, -150.f, 200.f);
		ImGui::SliderFloat("Forward Step", &creatureArg->forwardOffset, -150.f, 200.f);
		ImGui::SliderFloat("Otward Step", &creatureArg->outwardBias, 0.f, 1.f);

		ImGui::Text("Leg Count ", &creatureArg->legCounter);
		ImGui::SameLine(); if (ImGui::Button("+")) creatureArg->AddLeg();  ImGui::SameLine(); if (ImGui::Button("-")) creatureArg->RemoveLeg();

		for (size_t i = 0; i < creatureArg->legs.size(); ++i) {
			ImGui::PushID(static_cast<int>(i)); // Avoid ImGui ID collisions
			std::string legLabel = "Leg " + std::to_string(i);
			if (ImGui::TreeNode(legLabel.c_str())) {
				for (size_t j = 0; j < creatureArg->legs[i]->segments.size(); j++) 
				{
					std::string segmentLabel = "Segment " + std::to_string(j);
					if (ImGui::TreeNode(segmentLabel.c_str())) 
					{
						ImGui::SliderFloat("Leg Length", &creatureArg->legs[i]->segments[j]->lenght, 1.0f, 30.0f);
						ImGui::TreePop();
					}
				}

				ImGui::Text("Chain Segments Count ", &creatureArg->legs[i]->chainLenght);
				ImGui::SameLine(); if (ImGui::Button("+")) creatureArg->legs[i]->AddSegment();  ImGui::SameLine(); if (ImGui::Button("-")) creatureArg->legs[i]->RemoveSegment();

				ImGui::TreePop();
			}
			ImGui::PopID();
		}


		ImGui::Spacing();

		ImGui::Text("Body Options:");
		ImGui::SliderInt("Creature Lenght", &creatureArg->desiredLenght, 2, 30);
		ImGui::SliderInt("Segment Spacing", &creatureArg->desiredSegmentSpacing, 5, 50);

		while (creatureArg->segmentSpacing < creatureArg->desiredSegmentSpacing)
		{
			creatureArg->AddSpacing();
		}

		while (creatureArg->segmentSpacing > creatureArg->desiredSegmentSpacing)
		{
			creatureArg->RemoveSpacing();
		}

		for (size_t i = 0; i < creatureArg->segments.size(); ++i)
		{
			ImGui::PushID(static_cast<int>(i)); // Avoid ImGui ID collisions

			std::string label = "Segment " + std::to_string(i);
			if (ImGui::TreeNode(label.c_str()))
			{
				ImGui::SliderInt("Thickness", &creatureArg->segments[i]->segmentRadius, 0, 20);
				ImGui::TreePop();
			}

			ImGui::PopID();
		}

		// Required Logic for Sliders to work
		while (creatureArg->creatureLenght < creatureArg->desiredLenght)
			creatureArg->IncreaseBodyLenght();

		while (creatureArg->creatureLenght > creatureArg->desiredLenght)
			creatureArg->DecreaseBodyLenght();

		ImGui::Spacing();
		ImGui::SliderFloat("Rotation Speed", &creatureArg->rotationSpeed, 0.f, 5.f);
		ImGui::SliderFloat("Dynamic Speed", &creatureArg->dynamicSpeed, 0.f, 200.f);
		ImGui::SliderFloat("Texture Scale", &creatureArg->zoomFactor, 0.f, 50.f);

		ImGui::SliderFloat2("Offset", &creatureArg->offset.x, 0.f, 50.f);

		ImGui::SliderFloat3("Secondary Color", &creatureArg->secondaryColor.x, 0.f, 1.f);
		ImGui::SliderFloat3("Primary Color", &creatureArg->primaryColor.x, 0.f, 1.f);

		ImGui::SliderFloat("Spot Size", &creatureArg->spotSize, -1.f, 1.f);
}

void UserInterface::CameraMenu(FrameBuffer* fbo, Camera* maincam)
{
	ImGui::Begin((const char*)(ICON_SLIDERS " Camera"));

	ImGui::Checkbox("Play Maximized", &triggerMaximized); ImGui::SameLine();
	ImGui::Checkbox("Post Processed", &showPostProccesed);

	if (ImGui::SliderFloat("Zoom", &maincam->zoom, 0.f, 5.f, "%.2f"))
	{
		//glm::vec2 focus = glm::vec2(creatureArg->segments[0]->transform.position.x, creatureArg->segments[0]->transform.position.y);
		maincam->updateProjection();
	}
	ImGui::SliderFloat("Camera Offset", &maincam->distanceFromPlayer, 0.f, 5.f);
	ImGui::SliderFloat("Follow Speed", &maincam->followSpeed, 0.f, 5.f);

	if (showPostProccesed)
	{
		ImGui::Checkbox("Show Scanline", &fbo->scanline);
		ImGui::SliderFloat("Chromatic Abberation", &fbo->abberationIntensity, 0.f, 5.f);
		ImGui::SliderFloat("Curve Left", &fbo->curveLeft, 0.f, 5.f);
		ImGui::SliderFloat("Curve Right", &fbo->curveRight, 0.f, 5.f);
		ImGui::SliderFloat("Vignette", &fbo->vignetteIntensity, 0.f, 5.f);
	}

	ImGui::End();
}

void UserInterface::Logger()
{
	ImGui::Begin((const char*)(ICON_INFO " Logs"));

	const auto& logs = Logger::GetMessages();

	for (const auto& entry : logs)
	{
		ImVec4 color;
		switch (entry.type)
		{
		case LogType::LOG_INFO:
			color = ImVec4(0.f, 1.0f, 0.f, 1.0f); // Green
			break;
		case LogType::LOG_ERROR:
			color = ImVec4(1.f, 0.f, 0.0f, 1.0f); // Red
			break;
		case LogType::LOG_WARNING:
			color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f); // Yellow
			break;
		default:
			color = ImVec4(1, 1, 1, 1); // White
			break;
		}
		
		ImGui::PushStyleColor(ImGuiCol_Text, color);
		ImGui::TextUnformatted(entry.message.c_str());
		ImGui::PopStyleColor();
	}

	ImGui::End();
}

void UserInterface::Hierarchy(Scene* sceneRef)
{
	DrawHierarchyPanel(sceneRef, selectedHierarchyItem);
}

void UserInterface::PropertiesPanel(Scene* sceneRef)
{
	//ImGui::Begin((const char*)(ICON_MENU " Properties"));
	ImGui::Begin("Properties");

	if (selectedHierarchyItem != entt::null && !sceneRef->registry.valid(selectedHierarchyItem)) {
		selectedHierarchyItem = entt::null;
		ImGui::Text("No entity selected.");
		ImGui::End();
		return;
	}

	float buttonWidth = 100.0f, buttonHeight = 22.f;
	if (selectedHierarchyItem != entt::null && sceneRef->registry.valid(selectedHierarchyItem))
	{
		// NOTE: Only for debugging the scenegraph functionality:
		if (ImGui::CollapsingHeader("Hierarchy"))
		{
			auto& hierarchyComponent = sceneRef->registry.get<HierarchyComponent>(selectedHierarchyItem);
			auto& orderComponent = sceneRef->registry.get<OrderComponent>(selectedHierarchyItem);

			if (hierarchyComponent.parentID == entt::null) ImGui::Text("Parent, of %i.", hierarchyComponent.childrenID.size());
			else ImGui::Text("Children.");
		}

		if (ImGui::CollapsingHeader("Name")) 
		{
			char buffer[64];
			auto& name = sceneRef->registry.get<NameComponent>(selectedHierarchyItem);
			strncpy_s(buffer, name.name.c_str(), sizeof(buffer));
			buffer[sizeof(buffer) - 1] = '\0';
			if (ImGui::InputText("Name ##", buffer, sizeof(buffer))) {
				name.name = buffer;
			}
		
			char buffer2[64];
			strncpy_s(buffer2, name.tag.c_str(), sizeof(buffer2));
			buffer2[sizeof(buffer2) - 1] = '\0';
			if (ImGui::InputText("Tag", buffer2, sizeof(buffer2))) {
				name.tag = buffer2;
			}
		}

		if (sceneRef->registry.any_of<TransformComponent>(selectedHierarchyItem)) 
		{
			auto& transform = sceneRef->registry.get<TransformComponent>(selectedHierarchyItem);

			if (ImGui::CollapsingHeader("Transform"))
			{	
				DrawVec3ControlFromMatrix("Translation", transform.LocalMatrix, "Translation", 0.0f);
				DrawVec3ControlFromMatrix("Rotation", transform.LocalMatrix, "Rotation", 0.0f);
				DrawVec3ControlFromMatrix("Scale", transform.LocalMatrix, "Scale", 1.0f);

				ImGui::Spacing();
				float panelWidth = ImGui::GetContentRegionAvail().x;

				ImGui::PushID(1);
				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
				ImGui::SetCursorPosX((panelWidth - buttonWidth) * 0.5f);
				if (ImGui::Button("Delete", ImVec2(buttonWidth, buttonHeight)))
				{
					sceneRef->registry.remove<TransformComponent>(selectedHierarchyItem);
				}
				ImGui::PopStyleColor();
				ImGui::Spacing();
				ImGui::PopID();
			}
		}

		if (sceneRef->registry.any_of<SpriteComponent>(selectedHierarchyItem))
		{
			auto& sprite = sceneRef->registry.get<SpriteComponent>(selectedHierarchyItem);

			if (ImGui::CollapsingHeader("Sprite"))
			{
				if (ImGui::Button("Browse"))
				{
					std::string path = OpenFileDialog(FILE_TYPE::IMAGE);
					if (!path.empty())
					{
						fileName = std::filesystem::path(path).filename().string();
						sprite.texturePath = path;
						sprite.texture = Texture2D();
						int width, height, nrChannels;
						unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
						sprite.texture.Generate(width, height, data, false);
						stbi_image_free(data);
					}
				}
				ImGui::SameLine(); ImGui::Text(fileName.c_str());

				ImGui::Spacing();
				float panelWidth = ImGui::GetContentRegionAvail().x;

				ImGui::PushID(2);
				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
				ImGui::SetCursorPosX((panelWidth - buttonWidth) * 0.5f);
				if (ImGui::Button("Delete", ImVec2(buttonWidth, buttonHeight)))
				{
					sceneRef->registry.remove<SpriteComponent>(selectedHierarchyItem);
				}
				ImGui::PopStyleColor();
				ImGui::Spacing();
				ImGui::PopID();
			}
		}

		if (sceneRef->registry.any_of<ScriptComponent>(selectedHierarchyItem))
		{
			auto& script = sceneRef->registry.get<ScriptComponent>(selectedHierarchyItem);


			if (ImGui::CollapsingHeader("Script"))
			{

				if (ImGui::Button("Browse"))
				{
					std::string path = OpenFileDialog(FILE_TYPE::SCRIPT);
					if (!path.empty())
					{
						script.scriptPath = std::filesystem::path(path).filename().string();
						fileName = std::filesystem::path(path).filename().string();
					}
				}
				ImGui::SameLine(); ImGui::Text(fileName.c_str());

				ImGui::Spacing();
				float panelWidth = ImGui::GetContentRegionAvail().x;

				ImGui::PushID(3);
				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
				ImGui::SetCursorPosX((panelWidth - buttonWidth) * 0.5f);
				if (ImGui::Button("Delete", ImVec2(buttonWidth, buttonHeight)))
				{
					sceneRef->registry.remove<ScriptComponent>(selectedHierarchyItem);
				}
				ImGui::PopStyleColor();
				ImGui::Spacing();
				ImGui::PopID();
			}
		}

		if (sceneRef->registry.any_of<Creature>(selectedHierarchyItem))
		{
			auto& creature = sceneRef->registry.get<Creature>(selectedHierarchyItem);

			if (ImGui::CollapsingHeader("Creature"))
			{
				CreatureMenu(&creature);

				ImGui::Spacing();
				float panelWidth = ImGui::GetContentRegionAvail().x;

				ImGui::PushID(4);
				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
				ImGui::SetCursorPosX((panelWidth - buttonWidth) * 0.5f);
				if (ImGui::Button("Delete", ImVec2(buttonWidth, buttonHeight)))
				{
					sceneRef->registry.remove<Creature>(selectedHierarchyItem);
				}
				ImGui::PopStyleColor();
				ImGui::Spacing();
				ImGui::PopID();
			}
		}

		if (sceneRef->registry.any_of<Emitter>(selectedHierarchyItem))
		{
			auto& emitter = sceneRef->registry.get<Emitter>(selectedHierarchyItem);

			if (ImGui::CollapsingHeader("Emitter"))
			{
				EmitterMenu(&emitter);
				
				ImGui::Spacing();
				float panelWidth = ImGui::GetContentRegionAvail().x;

				ImGui::PushID(5);
				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
				ImGui::SetCursorPosX((panelWidth - buttonWidth) * 0.5f);
				if (ImGui::Button("Delete", ImVec2(buttonWidth, buttonHeight)))
				{
					sceneRef->registry.remove<Emitter>(selectedHierarchyItem);
				}
				ImGui::PopStyleColor();
				ImGui::Spacing();
				ImGui::PopID();
			}
		}

		if (sceneRef->registry.any_of<AnimationComponent>(selectedHierarchyItem))
		{
			auto& animation = sceneRef->registry.get<AnimationComponent>(selectedHierarchyItem);

			if (ImGui::CollapsingHeader("Animation"))
			{
				ImGui::Spacing();

				float inputWidth = 25.f;
				float spacing = ImGui::GetStyle().ItemSpacing.x;

				// First row: "Frame Count: Horizontal [input] Vertical [input]"
				// Measure all widths
				float label1Width = ImGui::CalcTextSize("Frame Count:  Horizontal").x;
				float input1Width = inputWidth;
				float label2Width = ImGui::CalcTextSize("Vertical").x;
				float input2Width = inputWidth;

				// Total width of group
				float groupWidth = label1Width + spacing + input1Width + spacing + label2Width + spacing + input2Width;

				// Center the group
				float panelWidth = ImGui::GetContentRegionAvail().x;
				ImGui::SetCursorPosX((panelWidth - groupWidth) * 0.5f);

				// Draw the group
				ImGui::Text("Frames Horizontally");
				ImGui::SameLine();
				ImGui::SetNextItemWidth(input1Width);
				ImGui::InputInt("##1", &spriteCountHor, 0, 36);

				ImGui::SameLine();
				ImGui::Text("Vertically");
				ImGui::SameLine();
				ImGui::SetNextItemWidth(input2Width);
				ImGui::InputInt("##2", &spriteCountVer, 0, 36);

				ImGui::Spacing();

				float buttonWidth = 140.0f, buttonHeight = 25.f;
				ImGui::SetCursorPosX((panelWidth - buttonWidth) * 0.5f);

				if (ImGui::Button("Add Animation", ImVec2{buttonWidth, buttonHeight})) // Button to open popup and create the animation.
				{
					std::string path = OpenFileDialog(FILE_TYPE::IMAGE);
					if (!path.empty())
					{
						// Extracting name from chosen file.
						std::string animFileName;
						animFileName = std::filesystem::path(path).filename().string();

						// Creating a new animation.
						Animation newAnim(path, animFileName, true, true, spriteCountHor, spriteCountVer, 10.f);

						// Adding the animation to the map inside the component with it's corresponding name.
						animation.animations.insert({ animFileName, newAnim });
						animation.currentAnimation = animFileName;
					}
				}

				ImGui::Spacing();

				if (animation.animations.size() != 0 ) ImGui::Text("Loaded Animations");

				ImGui::Indent(15.f);

				for (auto iterator = animation.animations.begin(); iterator != animation.animations.end();)
				{
					std::string animationName = iterator->first;
					if (ImGui::CollapsingHeader(animationName.c_str()))
					{
						//ImGui::Text("Duration"); ImGui::SameLine(); ImGui::SetNextItemWidth(inputWidth + 25); ImGui::InputFloat("##5", &iterator->second.duration, 0.0f, 0.0f, "%.1f", ImGuiInputTextFlags_None);
						ImGui::Text("Loop"); ImGui::SameLine(); ImGui::Checkbox("##4", &iterator->second.loop); 
						//ImGui::Text("On Start"); ImGui::SameLine(); ImGui::Checkbox("##6", &iterator->second.onStart);

						ImGui::Text("Playing"); ImGui::SameLine(); ImGui::Checkbox("##Playing", &iterator->second.playing);
						ImGui::InputFloat("Speed", &iterator->second.speed, 0.0f, 0.0f, "%.1f", ImGuiInputTextFlags_None);

						ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
						ImGui::SetCursorPosX((panelWidth - buttonWidth) * 0.5f);
						if (ImGui::Button("Delete Animation", ImVec2(buttonWidth, buttonHeight)))
						{
							iterator = animation.animations.erase(iterator); // erase and move to the next element
							ImGui::PopStyleColor();
							continue; // skip incrementing the iterator
						}
						ImGui::PopStyleColor();
					}

					++iterator; // increment only if not erased
				}

				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
				ImGui::SetCursorPosX((panelWidth - buttonWidth) * 0.5f);
				if (ImGui::Button("Delete Component", ImVec2(buttonWidth, buttonHeight)))
				{
					sceneRef->registry.remove<AnimationComponent>(selectedHierarchyItem);
				}
				ImGui::PopStyleColor();
				ImGui::Spacing();

				ImGui::Unindent();
			}
		}

		ImGui::Dummy(ImVec2{ 0, 14 });

		float popUpWidth = 200, popUpHeight = 100;
		float panelWidth = ImGui::GetContentRegionAvail().x;
		buttonWidth = 140.0f, buttonHeight = 25.f;
		ImGui::SetCursorPosX((panelWidth - buttonWidth) * 0.5f);
		if(ImGui::Button("New Component", ImVec2(buttonWidth, buttonHeight)))
		{
			ImVec2 pos = ImGui::GetCursorScreenPos();
			ImGui::SetNextWindowPos(ImVec2(pos.x + (panelWidth - popUpWidth) * 0.5f, pos.y));
			ImGui::OpenPopup("AddComponentPopup");
		}

		ImGui::SetNextWindowSize(ImVec2(popUpWidth, popUpHeight));
		ImGui::Spacing();
		if (ImGui::BeginPopup("AddComponentPopup")) 
		{
			Entity entityWrapper(selectedHierarchyItem, sceneRef->registry);

			if (!sceneRef->registry.any_of<TransformComponent>(selectedHierarchyItem))
			{
				if (ImGui::MenuItem("Transform Component"))
				{
					entityWrapper.AddComponent<TransformComponent>();
				}
			}
			if (!sceneRef->registry.any_of<SpriteComponent>(selectedHierarchyItem))
			{
				if (ImGui::MenuItem("Sprite Component"))
				{
					entityWrapper.AddComponent<SpriteComponent>();
				}
			}
			if (!sceneRef->registry.any_of<ScriptComponent>(selectedHierarchyItem))
			{
				if (ImGui::MenuItem("Script Component"))
				{
					entityWrapper.AddComponent<ScriptComponent>();
				}
			}
			if (!sceneRef->registry.any_of<AnimationComponent>(selectedHierarchyItem))
			{
				if (ImGui::MenuItem("Animation Component"))
				{
					entityWrapper.AddComponent<AnimationComponent>();
				}
			}
			if (!sceneRef->registry.any_of<Creature>(selectedHierarchyItem))
			{
				if (ImGui::MenuItem("Creature Component"))
				{
					entityWrapper.AddComponent<Creature>(1, cameraRef);
				}
			}
			if (!sceneRef->registry.any_of<Emitter>(selectedHierarchyItem))
			{
				if (ImGui::MenuItem("Emitter Component"))
				{
					entityWrapper.AddComponent<Emitter>();
				}
			}
			ImGui::EndPopup();
		}

		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
		ImGui::SetCursorPosX((panelWidth - buttonWidth) * 0.5f);
		if (ImGui::Button("Delete Entity", ImVec2(buttonWidth, buttonHeight)))
		{
			sceneRef->DestroyEntity(selectedHierarchyItem);
		}
		ImGui::PopStyleColor();
		ImGui::Spacing();
	}
	else 
		ImGui::Text("No entity selected.");

	ImGui::End();
}

void UserInterface::HeaderBar()
{
	// This is used to check wether cursor is over header so we can move the window.
	const float headerHeight = 30.0f;
	ImVec2 headerMin = ImGui::GetWindowPos();
	ImVec2 headerMax = ImVec2(headerMin.x + ImGui::GetWindowWidth(), headerMin.y + headerHeight);

	ImVec2 mousePos = ImGui::GetMousePos();
	isInDragRegion = mousePos.x >= headerMin.x && mousePos.x <= headerMax.x &&
		mousePos.y >= headerMin.y && mousePos.y <= headerMax.y &&
		!ImGui::IsAnyItemHovered();  

	if (ImGui::BeginMenuBar())
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f });

		if (ImGui::Button((const char*)(ICON_COG " Build")) && !buildTab) { buildTab = true; }

		if (buildTab)
		{
			TransparentBackground();

			ImGui::SetNextWindowSize(ImVec2(300, 500), ImGuiCond_Always); // Full screen
			ImGui::SetNextWindowPos(ImVec2(SCREEN_WIDTH / 2 - 300 / 2, SCREEN_HEIGHT / 2 - 500 / 2), ImGuiCond_Always); // Top-left corner
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0); // Remove borders
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0)); // Remove padding

			ImGui::Begin("Build Window", NULL,
				ImGuiWindowFlags_NoDecoration | // No title bar
				ImGuiWindowFlags_NoResize |     // Non-resizable
				ImGuiWindowFlags_NoMove);

			if (ImGui::ImageButton("##Close", (ImTextureID)(uintptr_t)ResourceManager::closeButton->ID, ImVec2(18, 18)))
			{
				buildTab = false;
			}
			ImGui::Dummy(ImVec2(0, 15));

			ImGui::Text("Current Scene in Build:");

			for (int i = 0; i < Game::getInstance()->scenes.size(); i++) 
			{
				std::string sceneName = Game::getInstance()->scenes[i]->sceneName;

				int selectableHeight = 18;
				bool foo;

				ImGui::PushID(i);
				ImGui::Text("%d", i); ImGui::SameLine(); 
				ImGui::Checkbox("##", &Game::getInstance()->scenes[i]->inBuild); ImGui::SameLine();
				ImGui::Selectable(sceneName.c_str(), false, 0, ImVec2(0, selectableHeight));

				if (ImGui::IsItemActive() && !ImGui::IsItemHovered())
				{
					if (draggedIndex = -1) draggedIndex = i;

					float dragDistance = ImGui::GetMouseDragDelta(0).y;

					if (std::abs(dragDistance) > selectableHeight)
					{
						int direction = dragDistance > 0 ? 1 : -1; // Was it moved beyond the first neighouring selectable?
						int newIndex = draggedIndex + direction;

						// If the new index is valid then swap the elements and reset the dragging distance.
						if (newIndex > -1 && newIndex < Game::getInstance()->scenes.size())
						{
							std::swap(Game::getInstance()->scenes[draggedIndex], Game::getInstance()->scenes[newIndex]);
							draggedIndex = newIndex;

							ImGui::ResetMouseDragDelta();
						}
					}
				}
				ImGui::Separator();
				ImGui::PopID();
			}

			ImGui::Dummy(ImVec2(0, 15));

			ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(40, 40, 40, 255));
			if (ImGui::Button("Build")) { };
			ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(40, 40, 40, 255));
			if (ImGui::Button("Build and Run")) { };
			ImGui::PopStyleColor(2);

			ImGui::PopStyleVar(2);
			ImGui::End();
			EndTransparentBackground();
		}
		if (ImGui::Button((const char*)(ICON_WRENCH " Scenes")))
			ImGui::OpenPopup("SceneHeaderPopup");

		ImGui::SetNextWindowSize(ImVec2(85, 60));
		ImVec2 pos = ImGui::GetCursorScreenPos();
		ImGui::SetNextWindowPos(ImVec2(pos.x - 65, pos.y + 25));

		if (ImGui::BeginPopup("SceneHeaderPopup"))
		{
			if (ImGui::MenuItem("Save")) { Game::getInstance()->activeScene->Serialize(); }
			if (ImGui::MenuItem("Load")) { Game::getInstance()->activeScene->Deserialize(); }
			ImGui::EndPopup();
		}

		if (ImGui::Button((const char*)(ICON_FOLDER " Add")))
		{
			ImGui::OpenPopup("CreatePopup");
			ImGui::SetNextWindowSize(ImVec2(125, 60));
			pos = ImGui::GetCursorScreenPos();
			ImGui::SetNextWindowPos(ImVec2(pos.x - 65, pos.y + 25));
		}
		if (ImGui::BeginPopup("CreatePopup"))
		{
			if (ImGui::MenuItem("New Script"))
			{
				openScriptPopup = true;
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::MenuItem("New Scene ")) 
			{
				openScenePopup = true;
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		if (openScriptPopup)
		{
			ImVec2 pos = ImGui::GetCursorScreenPos();
			ImGui::SetNextWindowPos(ImVec2(pos.x - 65, pos.y + 25));
			ImGui::OpenPopup("ScriptPopup");
			openScriptPopup = false;
		}

		if (ImGui::BeginPopup("ScriptPopup"))
		{
			static char buffer[64];
			buffer[sizeof(buffer) - 1] = '\0';

			ImGui::Text("Script Name"); ImGui::SameLine();
			ImGui::SetNextItemWidth(125);
			ImGui::InputText("##", buffer, sizeof(buffer));

			ImGui::SameLine();
			ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(40, 40, 40, 255));
			if (ImGui::Button("Add"))
			{
				NewFile(FileType::SCRIPT, buffer);
				ImGui::CloseCurrentPopup();
			}
			ImGui::PopStyleColor();

			if (!ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) &&
				!ImGui::IsAnyItemActive() &&
				ImGui::IsMouseDown(ImGuiMouseButton_Left))
			{
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		if (openScenePopup)
		{
			ImVec2 pos = ImGui::GetCursorScreenPos();
			ImGui::SetNextWindowPos(ImVec2(pos.x - 65, pos.y + 25));
			ImGui::OpenPopup("ScenePopup");
			openScenePopup = false;
		}

		if (ImGui::BeginPopup("ScenePopup"))
		{
			static char buffer[64];
			buffer[sizeof(buffer) - 1] = '\0';

			ImGui::Text("Scene Name"); ImGui::SameLine();
			ImGui::SetNextItemWidth(125);
			ImGui::InputText("##", buffer, sizeof(buffer));

			ImGui::SameLine();
			ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(40, 40, 40, 255));
			if (ImGui::Button("Add"))
			{
				NewFile(FileType::SCENE, buffer);
				ImGui::CloseCurrentPopup();
			}
			ImGui::PopStyleColor();

			if (!ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) &&
				!ImGui::IsAnyItemActive() &&
				ImGui::IsMouseDown(ImGuiMouseButton_Left))
			{
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
		ImGui::PopStyleColor();
		ImGui::EndMenuBar();
	}
}

bool UserInterface::IconButtonWithText(ImTextureID icon, const char* label, const ImVec2& iconSize, const ImVec2& padding)
{
	ImGui::PushID(label);
	float additionalLenght = 5.f;
	// Calculate total size (icon + spacing + text)
	ImVec2 textSize = ImGui::CalcTextSize(label);
	ImVec2 buttonSize(iconSize.x + padding.x + textSize.x + additionalLenght,
		(iconSize.y > textSize.y ? iconSize.y : textSize.y) + padding.y * 2);

	bool pressed = ImGui::Button("", buttonSize); // single unified clickable area

	// Draw icon
	ImVec2 cursorPos = ImGui::GetItemRectMin();
	ImGui::SetCursorScreenPos(ImVec2(cursorPos.x + padding.x, cursorPos.y + (buttonSize.y - iconSize.y) * 0.5f));
	ImGui::Image(icon, iconSize);

	// Draw text next to icon
	ImGui::SetCursorScreenPos(ImVec2(cursorPos.x + iconSize.x + padding.x * 2, cursorPos.y + (buttonSize.y - textSize.y) * 0.5f));
	ImGui::TextUnformatted(label);

	ImGui::PopID();
	return pressed;
}


void UserInterface::TransparentBackground()
{
	ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = ImVec4(0.2f, 0.2f, 0.2f, 0.8f); // Transparent background

	ImGui::SetNextWindowSize(ImVec2(SCREEN_WIDTH, SCREEN_HEIGHT), ImGuiCond_Always); // Full screen
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always); // Top-left corner
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0); // Remove borders
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0)); // Remove padding

	ImGui::Begin("Transparent Window", NULL,
		ImGuiWindowFlags_NoDecoration | // No title bar
		ImGuiWindowFlags_NoResize |     // Non-resizable
		ImGuiWindowFlags_NoMove);
}

void UserInterface::EndTransparentBackground()
{
	ImGui::End();
	ImGui::PopStyleVar(2);
}

std::string UserInterface::OpenFileDialog(const FILE_TYPE typeArg)
{
	char filename[MAX_PATH] = "";

	OPENFILENAMEA ofn = { 0 };
	ofn.lStructSize = sizeof(OPENFILENAMEA);
	ofn.hwndOwner = nullptr;
	ofn.lpstrFile = filename;
	ofn.nMaxFile = sizeof(filename);
	if (typeArg == FILE_TYPE::SCRIPT)
		ofn.lpstrFilter = "Script Files\0*.lua;\0All Files\0*.*\0";
	else
		ofn.lpstrFilter = "Image Files\0*.png;*.jpg;*.jpeg\0All Files\0*.*\0";
	ofn.nFilterIndex = 1;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (GetOpenFileNameA(&ofn))
		return std::string(filename);
	return ""; // Cancelled or failed
}

void UserInterface::NewFile(const FileType type, const std::string name)
{
	std::string extension, typeLocation, fullPath;
	switch (type)
	{
		case FileType::SCENE:
			typeLocation = "Assets/Scenes/";
			extension = ".yaml";
			break;
		case FileType::SCRIPT:
			typeLocation = "Assets/Scripts/";
			extension = ".lua";
			break;
	}

	fullPath = typeLocation + name + extension;

	if (std::filesystem::exists(fullPath)) 
	{
		Logger::Error("File already exists.");
		return;
	}

	ofstream file(fullPath);
	file.close();

	if (extension == ".lua")
	{
		ofstream file(fullPath);

		const char* basicLoops = R""""(function Start()
	print("Start")
end

function Update(dt)
	print("Update")
end

function Shutdown()
	print("Shutdown")
end
		)"""";
		file << basicLoops;
	}

	file.close();
	Logger::Log("Created file: " + fullPath);
}

void UserInterface::DrawEntityNode(entt::entity entity, Scene* sceneRef, entt::entity& selectedEntity, int& draggedEntityIndex, float selectableHeight)
{
	auto& registry = sceneRef->registry;

	// Compose label
	std::string label;
	if (registry.any_of<NameComponent>(entity))
		label = registry.get<NameComponent>(entity).name;
	else
		label = "Entity" + std::to_string((int)entity);

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
	if (selectedEntity == entity)
		flags |= ImGuiTreeNodeFlags_Selected;

	auto& hc = registry.get_or_emplace<HierarchyComponent>(entity);
	bool hasChildren = !hc.childrenID.empty();

	bool opened = false;
	if (hasChildren)
	{
		opened = ImGui::TreeNodeEx((void*)(intptr_t)entity, flags, "%s", label.c_str());
	}
	else
	{
		flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
		ImGui::TreeNodeEx((void*)(intptr_t)entity, flags, "%s", label.c_str());
	}

	// Select on click
	if (ImGui::IsItemClicked())
		selectedEntity = entity;

	// Drag source
	if (ImGui::BeginDragDropSource())
	{
		ImGui::SetDragDropPayload("DND_ENTITY", &entity, sizeof(entt::entity));
		ImGui::Text("%s", label.c_str());
		ImGui::EndDragDropSource();
	}

	// Drop target on this node
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_ENTITY"))
		{
			entt::entity dropped = *(const entt::entity*)payload->Data;

			if (dropped != entity)
			{
				auto& droppedHC = registry.get_or_emplace<HierarchyComponent>(dropped);

				// Check if dropping onto its current parent => DE-PARENT
				if (droppedHC.parentID == entity)
				{
					// Remove dropped from parent's children list
					auto& parentHC = registry.get<HierarchyComponent>(entity);
					parentHC.childrenID.erase(
						std::remove(parentHC.childrenID.begin(), parentHC.childrenID.end(), dropped),
						parentHC.childrenID.end());

					droppedHC.parentID = entt::null; // Set to root
				}
				else
				{
					// Normal reparenting
					if (droppedHC.parentID != entt::null)
					{
						auto& oldParent = registry.get<HierarchyComponent>(droppedHC.parentID);
						oldParent.childrenID.erase(
							std::remove(oldParent.childrenID.begin(), oldParent.childrenID.end(), dropped),
							oldParent.childrenID.end());
					}

					droppedHC.parentID = entity;

					// Add to new parent's children list if not already present
					if (std::find(hc.childrenID.begin(), hc.childrenID.end(), dropped) == hc.childrenID.end())
					{
						hc.childrenID.push_back(dropped);
					}
				}
			}
		}
		ImGui::EndDragDropTarget();
	}

	// Drag reorder of children (only if node opened)
	if (opened)
	{
		// Sort children by order
		std::sort(hc.childrenID.begin(), hc.childrenID.end(),
			[&](entt::entity a, entt::entity b) {
				return registry.get<OrderComponent>(a).order < registry.get<OrderComponent>(b).order;
			});

		int draggedChildIndex = -1;
		for (int i = 0; i < (int)hc.childrenID.size(); i++)
		{
			auto child = hc.childrenID[i];

			// Handle sibling reordering drag (you can adapt this logic if needed)
			if (ImGui::IsItemActive() && !ImGui::IsItemHovered())
			{
				if (draggedChildIndex == -1)
					draggedChildIndex = i;

				float dragDistance = ImGui::GetMouseDragDelta(0).y;

				if (std::abs(dragDistance) > selectableHeight)
				{
					int direction = dragDistance > 0 ? 1 : -1;
					int newIndex = draggedChildIndex + direction;

					if (newIndex >= 0 && newIndex < (int)hc.childrenID.size())
					{
						auto& orderA = registry.get<OrderComponent>(hc.childrenID[draggedChildIndex]);
						auto& orderB = registry.get<OrderComponent>(hc.childrenID[newIndex]);
						std::swap(orderA.order, orderB.order);

						draggedChildIndex = newIndex;
						ImGui::ResetMouseDragDelta();
					}
				}
			}

			DrawEntityNode(child, sceneRef, selectedEntity, draggedChildIndex, selectableHeight);
		}

		ImGui::TreePop();
	}
}

void UserInterface::DrawHierarchyPanel(Scene* sceneRef, entt::entity& selectedEntity)
{
	//ImGui::Begin((const char*)(ICON_LIST_NESTED " Hierarchy"));
	ImGui::Begin("Hierarchy");

	int rootDraggedIndex = -1;
	float selectableHeight = ImGui::GetTextLineHeightWithSpacing();

	auto& registry = sceneRef->registry;

	// Gather root entities (parentID == null)
	auto view = registry.view<HierarchyComponent>();
	std::vector<entt::entity> roots;
	for (auto entity : view) {
		if (registry.get<HierarchyComponent>(entity).parentID == entt::null) {
			roots.push_back(entity);
		}
	}

	// Sort roots by OrderComponent
	std::sort(roots.begin(), roots.end(),
		[&](entt::entity a, entt::entity b) {
			return registry.get<OrderComponent>(a).order < registry.get<OrderComponent>(b).order;
		});

	// Draw root nodes with drag-drop reorder and drop targets for reparenting
	for (int i = 0; i < (int)roots.size(); i++) {
		entt::entity root = roots[i];

		// Compose label
		std::string label;
		if (registry.any_of<NameComponent>(root))
			label = registry.get<NameComponent>(root).name;
		else
			label = "Entity" + std::to_string((int)root);

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
		if (selectedEntity == root)
			flags |= ImGuiTreeNodeFlags_Selected;

		auto& hc = registry.get_or_emplace<HierarchyComponent>(root);
		bool hasChildren = !hc.childrenID.empty();

		bool opened = false;
		if (hasChildren)
			opened = ImGui::TreeNodeEx((void*)(intptr_t)root, flags, "%s", label.c_str());
		else {
			flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
			ImGui::TreeNodeEx((void*)(intptr_t)root, flags, "%s", label.c_str());
		}

		if (ImGui::IsItemClicked())
			selectedEntity = root;

		// Drag source for root
		if (ImGui::BeginDragDropSource()) {
			ImGui::SetDragDropPayload("DND_ENTITY", &root, sizeof(entt::entity));
			ImGui::Text("%s", label.c_str());
			ImGui::EndDragDropSource();
		}

		// Drop target on root node for reparenting or de-parenting
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_ENTITY")) {
				entt::entity dropped = *(const entt::entity*)payload->Data;
				if (dropped != root) {
					auto& droppedHC = registry.get_or_emplace<HierarchyComponent>(dropped);

					// If dropped onto its current parent -> de-parent
					if (droppedHC.parentID == root) {
						auto& parentHC = registry.get<HierarchyComponent>(root);
						parentHC.childrenID.erase(
							std::remove(parentHC.childrenID.begin(), parentHC.childrenID.end(), dropped),
							parentHC.childrenID.end());
						droppedHC.parentID = entt::null; // Make root
					}
					else {
						// Remove from old parent if any
						if (droppedHC.parentID != entt::null) {
							auto& oldParent = registry.get<HierarchyComponent>(droppedHC.parentID);
							oldParent.childrenID.erase(
								std::remove(oldParent.childrenID.begin(), oldParent.childrenID.end(), dropped),
								oldParent.childrenID.end());
						}

						// Reparent to this root
						droppedHC.parentID = root;

						if (std::find(hc.childrenID.begin(), hc.childrenID.end(), dropped) == hc.childrenID.end())
							hc.childrenID.push_back(dropped);
					}
				}
			}
			ImGui::EndDragDropTarget();
		}

		// Drag reorder roots
		if (ImGui::IsItemActive() && !ImGui::IsItemHovered()) {
			if (rootDraggedIndex == -1)
				rootDraggedIndex = i;

			float dragDistance = ImGui::GetMouseDragDelta(0).y;
			if (std::abs(dragDistance) > selectableHeight) {
				int direction = dragDistance > 0 ? 1 : -1;
				int newIndex = rootDraggedIndex + direction;

				if (newIndex >= 0 && newIndex < (int)roots.size()) {
					auto& orderA = registry.get<OrderComponent>(roots[rootDraggedIndex]);
					auto& orderB = registry.get<OrderComponent>(roots[newIndex]);

					std::swap(orderA.order, orderB.order);
					std::swap(roots[rootDraggedIndex], roots[newIndex]);

					rootDraggedIndex = newIndex;
					ImGui::ResetMouseDragDelta();
				}
			}
		}

		// Recursively draw children nodes
		if (opened) {
			// Sort children before drawing
			std::sort(hc.childrenID.begin(), hc.childrenID.end(),
				[&](entt::entity a, entt::entity b) {
					return registry.get<OrderComponent>(a).order < registry.get<OrderComponent>(b).order;
				});

			int draggedChildIndex = -1;
			for (auto& child : hc.childrenID)
				DrawEntityNode(child, sceneRef, selectedEntity, draggedChildIndex, selectableHeight);

			ImGui::TreePop();
		}
	}

	ImGui::Dummy(ImVec2{ 0, 14 });

	float panelWidth = ImGui::GetContentRegionAvail().x;
	float buttonWidth = 100.0f, buttonHeight = 25.f;
	ImGui::SetCursorPosX((panelWidth - buttonWidth) * 0.5f);

	if (ImGui::Button("New Entity", ImVec2(buttonWidth, buttonHeight)))
		sceneRef->AddEntity("Entity", "Default");

	ImGui::End();
}
