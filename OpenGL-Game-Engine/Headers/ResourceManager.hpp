#pragma once
#include "Shader.hpp"
#include "Texture2D.hpp"
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
	Shader* perlinShader = nullptr;
	Shader* textShader = nullptr;
	Shader* particleShader = nullptr;
	Shader* animationShader = nullptr;

	// Textures:
	std::vector<Texture2D*> textures; // Used to count them
	static inline Texture2D* smileyFace = nullptr;

	// TODO
	// -------------------------------
	// Resource manager should be custom per game so separate between engine rezouces and shaders and the custom ones per game
	// -------------------------------

	// Icons for the engine:
	static inline Texture2D* playButton = nullptr;
	static inline Texture2D* stopButton = nullptr;

	static inline Texture2D* minimizeButton = nullptr;
	static inline Texture2D* closeButton = nullptr;
	static inline Texture2D* maximizeButton = nullptr;
};