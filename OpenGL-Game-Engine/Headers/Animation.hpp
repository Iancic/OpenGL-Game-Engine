#pragma once
#include "precomp.h"
#include "Texture2D.hpp"
#include "Logger.hpp"

class Animation
{
public:
	Animation(const std::string& path, const std::string& animFileName, const bool looped, const bool onStartArg, const int NumSpritesX, const int NumSpritesY, const float animDuration);
	~Animation() = default;

	std::string name;
	std::string texturePath;

	Texture2D texture;

	float speed = 32.f;

	// Indices that dictate which sprite is picked.
	int SpriteRow = 0;    // Sprite location in the sprite sheet - row
	int SpriteColumn = 0;    // Sprite location in the sprite sheet - col

	int totalFrames; // pre-calculated so I don't multiply over and over during animation step.

	float m_texUSize = 0.0f;  // Sprite size in tex space - U
	float m_texVSize = 0.0f;  // Sprite size in tex space - V
	int m_numSpritesX;
	int m_numSpritesY;

	bool loop = false, onStart = false, playing = true;

private:

};