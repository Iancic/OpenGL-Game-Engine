#include "UserInterface.h"

UserInterface* UserInterface::ui_Instance = nullptr;

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
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	ImGui_ImplSDL2_InitForOpenGL(windowArg, glContextArg);
	ImGui_ImplOpenGL3_Init(glsl_version);
}

void UserInterface::Shutdown()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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

void UserInterface::InfoHUD(Creature* creatureArg)
{
	ImGui::SetNextWindowPos(ImVec2(100, 100));
	ImGui::SetNextWindowSize(ImVec2(400, 300), open);

	ImGui::Begin("Info", &open, windowflags);

	// Hitpoints
	ImGui::TextColored(ImVec4(255, 0, 0, 255), "Hitpoints");
	ImGui::SameLine(); ImGui::Text("%i", creatureArg->identity->hitpoints);

	// Movement Speed
	ImGui::TextColored(ImVec4(0, 255, 0, 255), "Movement Speed");
	ImGui::SameLine(); ImGui::Text("%i", creatureArg->identity->movementSpeed);

	// Creature Lenght
	ImGui::TextColored(ImVec4(255, 255, 0, 255), "Lenght");
	ImGui::SameLine(); ImGui::Text("%i", creatureArg->identity->creatureLenght);

	// Weapon Slots
	ImGui::TextColored(ImVec4(0, 255, 255, 255), "Weapon Slots");
	ImGui::SameLine(); ImGui::Text("%i", creatureArg->identity->weaponSlots);

	ImGui::End();
}

void UserInterface::UpgradeMenu(Creature* creatureArg)
{
	ImGui::SetNextWindowPos(ImVec2(100, SCREEN_HEIGHT - 200));
	ImGui::SetNextWindowSize(ImVec2(400, 300), open);

	ImGui::Begin("UpgradeWindow", &open, windowflags);

	// Current DNA Points
	ImGui::Text("DNA Available: %d", creatureArg->DNA);

	// Hitpoints
	if (ImGui::SmallButton("+##1"))
	{
		creatureArg->IncreaseHealth();
	}
	ImGui::SameLine(); ImGui::Text("Hitpoints");

	// Movement Speed
	if (ImGui::SmallButton("+##2"))
	{
		creatureArg->IncreaseSpeed();
	}
	ImGui::SameLine(); ImGui::Text("Speed");

	// Creature Lenght
	if (ImGui::SmallButton("+##3"))
	{
		creatureArg->IncreaseLenght();
	}
	ImGui::SameLine(); ImGui::Text("Lenght");

	// Weapon Slots
	if (ImGui::SmallButton("+##4"))
	{
		creatureArg->IncreaseSlots();
	}
	ImGui::SameLine(); ImGui::Text("Weapon Slots");

	ImGui::End();
}

void UserInterface::ClassPopUp()
{
	ImGui::SetNextWindowPos(ImVec2(SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 20));
	ImGui::SetNextWindowSize(ImVec2(300, 60), open);

	ImGui::Begin("ClassWindow", &open, windowflags);

	if (ImGui::Button("Tank"))
		std::cout << "I choose Tank";

	ImGui::SameLine();
	if (ImGui::Button("Spawner"))
		std::cout << "I choose Spawner";

	ImGui::SameLine();
	if (ImGui::Button("Devourer"))
		std::cout << "I choose Devourer";

	ImGui::End();
}

void UserInterface::LevelBar(Creature* creatureArg)
{
	ImGui::SetNextWindowPos(ImVec2(SCREEN_WIDTH / 2 - 210, SCREEN_HEIGHT - 100));
	ImGui::SetNextWindowSize(ImVec2(800, 300), open);

	ImGui::Begin("LevelWindow", &open, windowflags);
	ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

	// Current Level
	ImGui::Text("Creature Level: %i", creatureArg->level);
	float percentage = static_cast<float>(creatureArg->points) / static_cast<float>(creatureArg->maxPoints);
	ImGui::ProgressBar(percentage, ImVec2(400, 10), "");

	ImGui::PopStyleColor();

	ImGui::End();
}