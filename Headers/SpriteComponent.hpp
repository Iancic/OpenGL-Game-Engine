#pragma once
#include "SpriteRenderer.hpp"

struct SpriteComponent
{
	std::string texturePath;

	Texture2D texture;
	SpriteRenderer spriteRenderer;
};