#pragma once
#include "SpriteBatch.hpp"
#include <string>

class Animation
{
public:
	Animation();
	~Animation() = default;

	std::string name;
	std::string texturePath;

	SpriteBatch texture;
	std::vector<SpriteInfo> frames;

	int spriteWidth, spriteHeight;
	float duration;
	bool loop;
};