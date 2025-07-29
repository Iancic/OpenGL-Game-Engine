#pragma once
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <commdlg.h>
#include "Entity.hpp"
#include "Camera.hpp"
#define YAML_CPP_STATIC_DEFINE 
#include "yaml-cpp/yaml.h"
#include "Input.hpp"

class Scene
{
public:
	Scene() = default;
	~Scene() = default;

	// Cycle
	void Start();
	void Render(float deltaTime, Camera* activeCamera);
	void Update(Input& inputSystem, float deltaTime);
	void Shutdown();

	// Lua Utilities
	void BindTypes(sol::state& lua);
	void BindEntity(sol::state& lua);

	// Scene Utilities
	void Serialize();
	void Deserialize();

	// Entity Utilities
	void AddEntity(const std::string& name, const std::string& tag);
	void DestroyEntity(entt::entity handle);
	
	// Systems
	void System_Sprite(Camera* activeCamera);
	void System_Animation(Camera* activeCamera);

	std::string OpenFileDialog();

	bool initialized = false, inBuild = true;

	entt::registry registry;
	std::vector<Entity> entities;

	std::string sceneName = "Not Loaded", scenePath;
private:
	
};