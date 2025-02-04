#include "ResourceManager.h"

ResourceManager* ResourceManager::resources_Instance = nullptr;

ResourceManager::ResourceManager()
{
	
}

void ResourceManager::loadResources()
{
	lineShader = new Shader("Assets/Shaders", "vTest.glsl", "fTest.glsl");
	frameBufferShader = new Shader("Assets/Shaders", "vFrameBuffer.glsl", "fFrameBuffer.glsl");
}