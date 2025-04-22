#include "UserInterface.h"
#include "Logger.h"

UserInterface* UserInterface::ui_Instance = nullptr;

void UserInterface::ImGui(Player* creatureArg, GLuint fboID, Camera* maincam)
{
	Start();
	Style();
	DockSpace();
	CreatureMenu(creatureArg);
	CameraMenu(creatureArg, maincam);
	GameViewport(fboID);
	Logger();
	Shutdown();
}

void UserInterface::Start()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui::NewFrame();
}

void UserInterface::Init(SDL_Window* windowArg, void* glContextArg)
{
	IMGUI_CHECKVERSION();
	const char* glsl_version = "#version 100";
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2(SCREEN_WIDTH, SCREEN_HEIGHT);
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_DockingEnable;;
	io.FontGlobalScale = 1.f;
	io.Fonts->AddFontFromFileTTF("Assets/fonts/MonaspaceNeonFrozen-Regular.ttf", 16.0f);
	io.Fonts->Build();
	ImGui_ImplSDL2_InitForOpenGL(windowArg, glContextArg);
	ImGui_ImplOpenGL3_Init(glsl_version);
}

void UserInterface::Shutdown()
{
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

void UserInterface::GameViewport(GLuint fboID)
{
	ImGui::Begin("Game View");
	ImTextureID imguiTexture = static_cast<ImTextureID>(static_cast<uintptr_t>(fboID));
	ImGui::Image(imguiTexture, ImVec2(VIEWPORT_WIDTH, VIEWPORT_HEIGHT), ImVec2(0, 1), ImVec2(1, 0));
	ImGui::End();
}

void UserInterface::Style()
{
	ImGuiStyle& style = ImGui::GetStyle();
	ImVec4* colors = style.Colors;

	colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.05f, 0.05f, 0.05f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);

	colors[ImGuiCol_Button] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(1.f, 1.f, 1.f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);

	colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);

	colors[ImGuiCol_Border] = ImVec4(0.20f, 0.20f, 0.20f, 0.50f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);

	colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.05f, 0.05f, 0.05f, 1.00f);

	colors[ImGuiCol_ResizeGrip] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
}

void UserInterface::CreatureMenu(Player* creatureArg)
{
	ImGui::Begin("Creature", &open);



	ImGui::Text("Leg Options:");
	ImGui::SliderFloat("Snap Distance", &creatureArg->distanceToSnap, -150.f, 200.f);
	ImGui::SliderFloat("Forward Step", &creatureArg->forwardOffset, -150.f, 200.f);
	ImGui::SliderFloat("Otward Step", &creatureArg->outwardBias, 0.f, 1.f);

	ImGui::Text("Leg Count ", &creatureArg->legCounter); 
	ImGui::SameLine(); if (ImGui::Button("+")) creatureArg->AddLeg();  ImGui::SameLine(); if (ImGui::Button("-")) creatureArg->RemoveLeg();

	for (size_t i = 0; i < creatureArg->legs.size(); ++i) {
		ImGui::PushID(static_cast<int>(i)); // Avoid ImGui ID collisions
		std::string legLabel = "Leg " + std::to_string(i);
		if (ImGui::TreeNode(legLabel.c_str())) {
			for (size_t j = 0; j < creatureArg->legs[i]->segments.size(); j++) {
				std::string segmentLabel = "Segment " + std::to_string(j);
				if (ImGui::TreeNode(segmentLabel.c_str())) {
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
			ImGui::SliderInt("Thickness", &creatureArg->segments[i]->segmentRadius, 0.0f, 20.0f);
			ImGui::TreePop();
		}

		ImGui::PopID();
	}

	// Required Logic for Sliders to work
	while (creatureArg->creatureLenght < creatureArg->desiredLenght)
		creatureArg->IncreaseBodyLenght();

	while (creatureArg->creatureLenght > creatureArg->desiredLenght)
		creatureArg->DecreaseBodyLenght();

	ImGui::End();
	
}

void UserInterface::CameraMenu(Player* creatureArg, Camera* maincam)
{
	ImGui::Begin("Camera", &open);

	if (ImGui::SliderFloat("Zoom", &maincam->zoom, 0.f, 5.f, "%.2f"))
	{
		glm::vec2 focus = glm::vec2(creatureArg->segments[0]->transform.position.x, creatureArg->segments[0]->transform.position.y);
		maincam->updateProjection(focus);
	}
	ImGui::SliderFloat("Camera Offset", &maincam->distanceFromPlayer, 0.f, 5.f);
	ImGui::SliderFloat("Follow Speed", &maincam->followSpeed, 0.f, 5.f);

	ImGui::End();
}

void UserInterface::Logger()
{
	ImGui::Begin("Logs");

	const auto& logs = Logger::GetMessages(); // Or use Logger::messages if it's public

	for (const auto& entry : logs)
	{
		ImVec4 color;
		switch (entry.type)
		{
		case LogType::LOG_INFO:
			color = ImVec4(0.6f, 1.0f, 0.6f, 1.0f); // Light green
			break;
		case LogType::LOG_ERROR:
			color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f); // Light red
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

void UserInterface::HeaderBar()
{
	if (ImGui::BeginMenuBar())
	{
		/*
		ImGui::Text("FPS");
		*/

		if (ImGui::BeginMenu("Options"))
		{
			
			if (ImGui::MenuItem("Randomize Creature"))
			{
				// Close app or set a flag
			}
			if (ImGui::MenuItem("Save Preset"))
			{
				// Close app or set a flag
			}
			if (ImGui::MenuItem("Load Preset"))
			{
				// Close app or set a flag
			}
			
			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}
}

void UserInterface::Game()
{
	
}
