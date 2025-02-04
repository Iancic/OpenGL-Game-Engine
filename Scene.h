#pragma once
#include "Actor.h"

class Scene
{
public:
	Scene(Actor* root);
	~Scene() = default;

	bool requestChange = false;

	std::vector<Actor*> actors;

	void Update();
	void Render();

	void AddActor();
	void RemoveActor();

	void AddSceneGraphRoot();
};