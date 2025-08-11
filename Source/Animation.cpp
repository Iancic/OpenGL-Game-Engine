#include "precomp.h"
#include "Animation.hpp"

Animation::Animation(std::string path, std::string animFileName, bool looped, bool onStartArg, int individualFrameWidthArg, int NumSpritesX, int NumSpritesY, float AnimSpeed)
{
	individualFrameWidth = individualFrameWidthArg;

	frameInfo.SpriteRow = 0;
	frameInfo.SpriteCol = 4;
	frameInfo.SpriteWidth = individualFrameWidth;

	texturePath = path;
	name = animFileName;
	texture = SpriteBatch(path.c_str(), NumSpritesX, NumSpritesY, SCREEN_WIDTH, SCREEN_HEIGHT);
	loop = looped;
	onStart = onStartArg;
	speed = AnimSpeed;
}

void Animation::Render(glm::vec2 position, float size, float rotate)
{
	texture.RenderSingleSprite(frameInfo, position, size, rotate);
}
