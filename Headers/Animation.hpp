#pragma once
#include "precomp.h"
#include "Common.hpp"
#include "SpriteBatch.hpp"

class Animation
{
public:
	Animation(std::string path, std::string animFileName, bool looped, bool onStartArg, int individualFrameWidthArg, int NumSpritesX, int NumSpritesY, float AnimSpeed);
	~Animation() = default;

	void Render(glm::vec2 position, float size, float rotate);

	std::string name;
	std::string texturePath;

	SpriteBatch texture;
	SpriteInfo frameInfo;

	int individualFrameWidth;
	float speed;
	bool loop, onStart;
};