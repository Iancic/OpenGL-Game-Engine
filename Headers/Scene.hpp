#pragma once
#include "Entity.hpp"
#include "Camera.hpp"

class Scene
{
public:
	Scene();
	~Scene() = default;

	void Start();
	void Render();
	void Update();

	void BindTypes(sol::state& lua);
	void BindEntity(sol::state& lua);

	void Serialize();
	void AddEntity(const std::string& name, const std::string& tag);
	void DestroyEntity(entt::entity handle);
	
	void RenderingSystem(Camera* activeCamera);

	Transform* origin = new Transform();
	entt::registry registry;
	std::vector<Entity> entities;
private:
	
};