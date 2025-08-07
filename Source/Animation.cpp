#include "precomp.h"
#include "Animation.hpp"

Animation::Animation()
{
	SpriteInfo frame1;
	frame1.PixelX = 0;
	frame1.PixelY = 0;
	frame1.SpriteRow = 0;
	frame1.SpriteCol = 4;
	frame1.SpriteWidth = 64;

	frames.push_back(frame1);
}