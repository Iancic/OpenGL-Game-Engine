#pragma once
#include "Shader.h"
#include "Texture2D.h"
#include <stb/stb_image.h>

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

	// Shaders:
	Shader* frameBufferShader = nullptr;
	Shader* lineShader = nullptr;
	Shader* spriteShader = nullptr;
	Shader* spineShader = nullptr;

	// Textures:
	Texture2D* smileyFace = nullptr;
};