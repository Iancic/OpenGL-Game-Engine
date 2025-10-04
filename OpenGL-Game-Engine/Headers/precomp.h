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
#include <SDL3/SDL.h>

// GLM: Math
#define GLM_ENABLE_EXPERIMENTAL
#include <glm-master/glm/glm.hpp>
#include <glm-master/glm/gtc/type_ptr.hpp>
#include <glm-master/glm/gtc/matrix_transform.hpp>
#include <glm-master/glm/gtx/matrix_decompose.hpp>
#include <glm-master/glm/gtx/quaternion.hpp>

// ImGui:
#include <imgui-master/imgui.h> 
#include <imgui-master/backends/imgui_impl_sdl3.h>
#include <imgui-master/backends/imgui_impl_opengl3.h>
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

// ENTT
#include "entt/entt.hpp"

// STB Image
#include <stb/stb_image.h>

// Sol: Bindings for LUA
#include <sol/sol.hpp>

// box2D: physics
#include <box2D/include/box2d/box2d.h>