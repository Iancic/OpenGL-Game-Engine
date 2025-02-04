#pragma once
#include "Scene.h"

class SceneManager
{

public:
	SceneManager();
	~SceneManager() = default;

	float timer = 0.f;

	bool Update();
	void Render();

	void AddScene(Scene* scene);
	void NextScene(float deltaTime);

	std::vector<Scene*>* scenesPool;

	Scene* currentScene = nullptr;
	int currentIndex = -1;
};