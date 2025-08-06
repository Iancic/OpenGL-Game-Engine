#pragma once
#include "SpriteBatch.hpp"
#include <string>

class Animation
{
public:
	Animation() = default;
	~Animation() = default;

	std::string name;
	std::string texturePath;

	SpriteBatch texture;

	int spriteWidth, spriteHeight;
	float duration;
	bool loop;
};