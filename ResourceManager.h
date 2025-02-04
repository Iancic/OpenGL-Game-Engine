#pragma once
#include "Shader.h"
#include "Texture2D.h"

class ResourceManager
{
private:
	// Singleton
	static ResourceManager* resources_Instance;
	ResourceManager();

public:

	// Prevent copying or assignment 
	ResourceManager(const ResourceManager&) = delete;
	ResourceManager& operator=(const ResourceManager&) = delete;

	// Singleton
	static ResourceManager* getInstance()
	{
		if (resources_Instance == nullptr)
		{
			resources_Instance = new ResourceManager();
		}

		return resources_Instance;
	}

	void loadResources();

	// Resources:
	Shader* frameBufferShader = nullptr;
	Shader* lineShader = nullptr;
};