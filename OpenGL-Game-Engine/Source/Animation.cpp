#include "precomp.h"
#include "Animation.hpp"

Animation::Animation(const std::string& path, const std::string& animFileName, const bool looped, const bool onStartArg, const int NumSpritesX, const int NumSpritesY, const float animDuration)
{
	texturePath = path;
	name = animFileName; 

	texturePath = path;
	texture = Texture2D();
	int width, height, nrChannels;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
	texture.Generate(width, height, data, false);
	stbi_image_free(data);

	loop = looped;
	onStart = onStartArg;

	m_numSpritesX = NumSpritesX;
	m_numSpritesY = NumSpritesY;
	totalFrames = m_numSpritesX * m_numSpritesY;

	m_texUSize = 1.0f / m_numSpritesX;
	m_texVSize = 1.0f / m_numSpritesY;
}
