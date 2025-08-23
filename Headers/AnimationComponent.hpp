#pragma once
#include "precomp.h"
#include "Animation.hpp"
#include "SpriteRenderer.hpp"

struct AnimationComponent
{
	std::string currentAnimation;

	std::unordered_map<std::string, Animation> animations;
	SpriteRenderer renderer;

	float duration = 10.f, elapsedTime = 0.f;

	void Update(float deltaTime)
	{
		if (!animations.at(currentAnimation).playing) return;

		elapsedTime += deltaTime * animations.at(currentAnimation).speed;

		if (elapsedTime >= duration)
		{
			// NOTE: we assume all frames are un-wrapped horrizontally in the sprite sheet.
			// TODO: this is a limitation for animations in a square grid.

			if (animations.at(currentAnimation).SpriteColumn < animations.at(currentAnimation).m_numSpritesX)
				animations.at(currentAnimation).SpriteColumn++;
			else
				animations.at(currentAnimation).SpriteColumn = 0;

			Logger::Log(animations.at(currentAnimation).SpriteColumn);

			elapsedTime = 0.f;
		}
	}

};