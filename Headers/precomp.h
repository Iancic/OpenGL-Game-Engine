#pragma once
// NOTE: As a rule of thumb the following headers are:
// 1. EXTERNAL LIBRARIES: that will not be modified.
// 2. INTERNAL HEADERS: that are not modified often.

// Windows: Utilities
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <commdlg.h>

// YAML: Serialization
#define YAML_CPP_STATIC_DEFINE 
#include "yaml-cpp/yaml.h"

// OpenGL Functions:
#include <glad/glad.h> 

// SDL: Windowing, Input, Audio
#include <SDL.h> 

// GLM: Math
#include <glm-master/glm/glm.hpp>
#include <glm-master/glm/gtc/matrix_transform.hpp>
#include <glm-master/glm/gtc/type_ptr.hpp>

// ImGui:
#include <imgui-master/imgui.h> 
#include <imgui-master/imgui_impl_sdl2.h>
#include <imgui-master/imgui_impl_sdlrenderer2.h>
#include <imgui-master/imgui_impl_opengl3.h>
#include <imgui-master/imgui_internal.h>
#include "imgui-master/ImGuizmo.h"

// C++ Utilities:
#include <iostream>
#include <map>
#include <unordered_map>
#include <thread>
#include <cassert>
#include <string>
#include <filesystem>
#include <vector>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <ctime>
#include <sstream>

// FMOD
#include "fmod/include/fmod.hpp"
#include "fmod/include/fmod_errors.h"

// ENTT
#include "entt/entt.hpp"

// STB Image
#include <stb/stb_image.h>

// Sol: Bindings for LUA
#include <sol/sol.hpp>