#include "UserInterface.hpp"
#include "Logger.hpp"

UserInterface* UserInterface::ui_Instance = nullptr;

UserInterface::UserInterface()
{
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
	{
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
	}

	FT_Face face;
	if (FT_New_Face(ft, "Assets/Fonts/MonaspaceNeonFrozen-Regular.ttf", 0, &face))
	{
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
	}

	FT_Set_Pixel_Sizes(face, 0, 48); // Setting the width to 0 lets the face dynamically calculate the width based on the given height.

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

	// first 128 ASCII characters
	for (unsigned char c = 0; c < 128; c++)
	{
		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) // Load Glyph 
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}

		// Generate Texture
		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Store Character for later use
		Character character = { texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		Characters.insert(std::pair<char, Character>(c, character));
	}

	// When we are done with True Type clear data
	FT_Done_Face(face);
	FT_Done_FreeType(ft);

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

void UserInterface::RenderText(const std::string& text, float x, float y, float scale, const glm::vec3& color)
{
	// Get and check shader
	auto shader = ResourceManager::getInstance()->textShader;
	if (!shader) return;

	shader->use();
	shader->setVec3("textColor", color);

	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(textVAO);
	glBindBuffer(GL_ARRAY_BUFFER, textVBO);

	for (const char& c : text)
	{
		// Ensure character exists in the map
		auto it = Characters.find(c);
		if (it == Characters.end()) continue;

		const Character& ch = it->second;

		float xpos = x + ch.Bearing.x * scale;
		float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;
		float w = ch.Size.x * scale;
		float h = ch.Size.y * scale;

		// Define quad vertices
		float vertices[6][4] = {
			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos,     ypos,       0.0f, 1.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },

			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },
			{ xpos + w, ypos + h,   1.0f, 0.0f }
		};

		// Bind texture and render glyph quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// Advance cursor (convert from 1/64th pixels to pixels)
		x += (ch.Advance >> 6) * scale;
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void UserInterface::EngineEditor(Creature* creatureArg, FrameBuffer* fbo, Camera* maincam, Scene* sceneRef)
{
	Start();
	Style();
	DockSpace();
	Gameplay();
	CreatureMenu(creatureArg);
	CameraMenu(creatureArg, fbo, maincam);

	if (showPostProccesed)
		GameViewport(fbo->postprocessedTexture);
	else
		GameViewport(fbo->framebufferTexture);
	Logger();
	ContentBrowser();
	Hierarchy(sceneRef);
	PropertiesPanel(sceneRef);
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
	ImGui::Begin("Viewport", nullptr, windowFlags);
	ImTextureID imguiTexture = static_cast<ImTextureID>(static_cast<uintptr_t>(fboID));
	ImGui::Image(imguiTexture, ImVec2(VIEWPORT_WIDTH, VIEWPORT_HEIGHT), ImVec2(0, 1), ImVec2(1, 0));
	ImGui::End();
}

void UserInterface::Style()
{
	ImGuiStyle& style = ImGui::GetStyle();
	ImVec4* colors = style.Colors;

	// Darker, more contrasted tones
	ImVec4 backgroundGray = ImVec4(0.10f, 0.10f, 0.10f, 1.00f); // almost black
	ImVec4 windowBgGray = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);    // darker than before
	ImVec4 baseGray = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);         // darker
	ImVec4 hoverGray = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);        // same hover
	ImVec4 activeGray = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);       // brighter when active

	// Text
	colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f); // brighter text for readability

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
	colors[ImGuiCol_MenuBarBg] = windowBgGray;

	// Checkmarks
	colors[ImGuiCol_CheckMark] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f); // visible light gray

	// Sliders
	colors[ImGuiCol_SliderGrab] = hoverGray;
	colors[ImGuiCol_SliderGrabActive] = activeGray;

	// Roundness
	style.WindowRounding = 3.0f;
	style.FrameRounding = 3.0f;
	style.ScrollbarRounding = 3.0f;
	style.GrabRounding = 3.0f;
	style.TabRounding = 3.0f;
}

void UserInterface::CreatureMenu(Creature* creatureArg)
{
	if (creatureMenu) {
		ImGui::Begin("Creature", &creatureMenu);

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

		ImGui::Spacing();
		ImGui::SliderFloat("Rotation Speed", &creatureArg->rotationSpeed, 0.f, 5.f);
		ImGui::SliderFloat("Dynamic Speed", &creatureArg->dynamicSpeed, 0.f, 200.f);
		ImGui::SliderFloat("Texture Scale", &creatureArg->zoomFactor, 0.f, 50.f);

		ImGui::SliderFloat2("Offset", &creatureArg->offset.x, 0.f, 50.f);

		ImGui::SliderFloat3("Secondary Color", &creatureArg->secondaryColor.x, 0.f, 1.f);
		ImGui::SliderFloat3("Primary Color", &creatureArg->primaryColor.x, 0.f, 1.f);

		ImGui::SliderFloat("Spot Size", &creatureArg->spotSize, -1.f, 1.f);

		ImGui::End();
	}
}

void UserInterface::CameraMenu(Creature* creatureArg, FrameBuffer* fbo, Camera* maincam)
{
	if (cameraMenu) {
		ImGui::Begin("Camera", &cameraMenu);

		if (ImGui::SliderFloat("Zoom", &maincam->zoom, 0.f, 5.f, "%.2f"))
		{
			glm::vec2 focus = glm::vec2(creatureArg->segments[0]->transform.position.x, creatureArg->segments[0]->transform.position.y);
			maincam->updateProjection(focus);
		}
		ImGui::SliderFloat("Camera Offset", &maincam->distanceFromPlayer, 0.f, 5.f);
		ImGui::SliderFloat("Follow Speed", &maincam->followSpeed, 0.f, 5.f);

		ImGui::Checkbox("Post Processing:", &showPostProccesed);

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
}

void UserInterface::Gameplay()
{
	if (gameMenu) {
		ImGui::Begin("Game", &gameMenu);

		ImGui::Checkbox("Show Colliders", &renderDebugInfo);

		ImGui::End();
	}
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

void UserInterface::ContentBrowser()
{
	ImGui::Begin("Content Browser");

	ImGui::End();
}

void UserInterface::Hierarchy(Scene* sceneRef)
{
	ImGui::Begin("Hierarchy");

	for (auto entity : sceneRef->registry.view<entt::entity>())
	{
		// Out of the entt id make one for IMGUI
		int id = static_cast<int>(entity);
		ImGui::PushID(id);

		// Get name of the Entity
		std::string label = "Entity" + std::to_string(id);
		if (sceneRef->registry.any_of<NameComponent>(entity)) 
		{
			auto& name = sceneRef->registry.get<NameComponent>(entity);
			label = name.name;
		}

		// TODO: wrap this inside a Tree Node for scene graph functionality
		bool clicked = ImGui::Selectable(label.c_str(), selectedHierarchyItem == entity);
		if (clicked) selectedHierarchyItem = entity;

		ImGui::PopID();
	}

	ImGui::End();
}

void UserInterface::PropertiesPanel(Scene* sceneRef)
{
	ImGui::Begin("Properties");

	if (selectedHierarchyItem != entt::null && sceneRef->registry.valid(selectedHierarchyItem))
	{
		// ALL POSSIBLE COMPONENTS
		if (ImGui::CollapsingHeader("Name")) {
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
				ImGui::Text("Test 2");
			}
		}

	}
	else 
		ImGui::Text("No entity selected.");

	ImGui::End();
}

void UserInterface::HeaderBar()
{
	if (ImGui::BeginMenuBar())
	{
		/*
		ImGui::Text("FPS");
		*/

		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Save"))
			{

			}
			if (ImGui::MenuItem("Export Build"))
			{

			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Preferences"))
			{
				
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("View"))
		{

			if (ImGui::MenuItem("Camera"))
			{
				cameraMenu = true;
			}
			if (ImGui::MenuItem("Game"))
			{
				gameMenu = true;
			}
			if (ImGui::MenuItem("Creature"))
			{
				creatureMenu = true;
			}

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}
}

void UserInterface::Game()
{
	RenderText("This is sample text", 100.0f, 100.0f, 1.0f, glm::vec3(1.f, 1.f, 1.f));
}
