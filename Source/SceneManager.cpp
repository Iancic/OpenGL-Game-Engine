#include "SceneManager.hpp"

SceneManager::SceneManager()
{
	scenesPool = new std::vector<Scene*>();
}

bool SceneManager::Update()
{
	if (currentIndex + 1 >= scenesPool->size())
	{
		return true; // Last Scene Is Always Ending Scene
	}

	if (currentIndex >= 0 && currentIndex < scenesPool->size())
	{
		currentScene = (*scenesPool)[currentIndex];

		if (currentScene->requestChange)
		{
			//Next Scene
		}
		else
			currentScene->Update();
	}

	return false;
}

void SceneManager::Render()
{
	if (currentIndex >= 0 && currentIndex < scenesPool->size())
	{
		
	}
}

void SceneManager::AddScene(Scene* scene)
{
	if (scene)
	{
		scenesPool->push_back(scene);
		if (currentIndex == -1)
		{
			currentIndex = 0;
			currentScene = (*scenesPool)[currentIndex];
		}
	}
}

void SceneManager::NextScene(float deltaTime)
{
	(void)deltaTime; // Unreferenced Parameter

}