#pragma once
#include "Entity.hpp"

class Scene
{
public:
	Scene();
	~Scene() = default;

	// TODO: SEE COMMENT IN CONSTRUCTOR
	Entity* player = nullptr;

	entt::registry registry;
private:

};