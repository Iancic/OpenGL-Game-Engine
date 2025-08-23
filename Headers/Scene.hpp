#pragma once
#include "precomp.h"

#include "Entity.hpp"
#include "Camera.hpp"
#include "Input.hpp"

class Scene
{
public:
	Scene() = default;
	~Scene() = default;

	// Cycle
	void Start();
	void Render(float deltaTime, Camera* activeCamera);
	void Update(float deltaTime, Input& inputSystem);
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
	void System_Animation(float deltaTime);
	void System_Animation_Render(Camera* activeCamera);
	void System_SceneGraph();
	void System_RecurseTransform(entt::entity ID, const glm::mat4& parentMatrix);
	void System_Creature(float deltaTime);
	void System_Creature_Render();
	void System_Emitter(float deltaTime);
	void System_Emitter_Render(Camera* activeCamera);

	std::string OpenFileDialog();

	entt::registry registry;
	std::vector<Entity> entities;

	std::string sceneName = "Not Loaded", scenePath;

	bool initialized = false, inBuild = true;
private:

};