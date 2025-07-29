#pragma once
#include "Texture2D.hpp"
#include "SpriteRenderer.hpp"
#include <string>

struct Animation
{
	std::string name;
	std::string texturePath;

	Texture2D texture;
	SpriteRenderer spriteRenderer;

	int spriteWidth, spriteHeight;
	float duration;
	bool loop;
};