#include "ResourceManager.hpp"

ResourceManager* ResourceManager::resources_Instance = nullptr;

ResourceManager::ResourceManager()
{
	
}

void ResourceManager::loadResources()
{
	lineShader = new Shader("Assets/Shaders", "vBasic.glsl", "fBasic.glsl");
	frameBufferShader = new Shader("Assets/Shaders", "vFrameBuffer.glsl", "fFrameBuffer.glsl");
	spriteShader = new Shader("Assets/Shaders", "vSprite.glsl", "fSprite.glsl");
	spineShader = new Shader("Assets/Shaders", "vSpine.glsl", "fSpine.glsl");
	perlinShader = new Shader("Assets/Shaders", "vPerlin.glsl", "fPerlin.glsl");
	textShader = new Shader("Assets/Shaders", "vText.glsl", "fText.glsl");

	smileyFace = new Texture2D();
	textures.push_back(smileyFace);
	int width, height, nrChannels;
	unsigned char* data = stbi_load("Assets/Textures/awesomeface.png", &width, &height, &nrChannels, 0);
	smileyFace->Generate(width, height, data);
	stbi_image_free(data);
}