#pragma once
#include "Entity.hpp"
#include "Camera.hpp"

class Scene
{
public:
	Scene();
	~Scene() = default;
	
	std::vector<Entity> entities;
	// TODO: SEE COMMENT IN CONSTRUCTOR
	Entity* player = nullptr;

	entt::registry registry;

	SpriteRenderer* testerMata;

	void AddEntity(const std::string& name, const std::string& tag);
	void DestroyEntity(entt::entity handle);

	void RenderingSystem(Camera* activeCamera);
private:

};