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
	void Render();
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
	void CreatureSystemRender();

	std::string OpenFileDialog();

	bool initialized = false;

	Transform* origin = new Transform();

	entt::registry registry;
	std::vector<Entity> entities;

	std::string sceneName, scenePath;
private:
	
};