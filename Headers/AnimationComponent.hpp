#pragma once
#include <vector>
#include <unordered_map>
#include "Animation.hpp"

struct AnimationComponent
{
	std::unordered_map<std::string, Animation> animations;
	std::string currentAnimation;
};