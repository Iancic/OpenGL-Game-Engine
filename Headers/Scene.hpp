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
	Scene();
	~Scene() = default;

	void Start();
	void Render(float deltaTime);
	void Update(Input& inputSystem, float deltaTime);
	void Shutdown();

	void BindTypes(sol::state& lua);
	void BindEntity(sol::state& lua);

	void Serialize();
	void Deserialize();

	void AddEntity(const std::string& name, const std::string& tag);
	void DestroyEntity(entt::entity handle);
	
	void SpriteSystemRendering();
	void CreatureSystemUpdate(Transform& target, float deltaTime);
	void CreatureSystemRender(float deltaTime);

	std::string OpenFileDialog();

	bool initialized = false;
	bool inBuild = true;

	Transform* origin = new Transform();

	entt::registry registry;
	std::vector<Entity> entities;

	std::string sceneName = "Not Loaded", scenePath;
private:
	
};