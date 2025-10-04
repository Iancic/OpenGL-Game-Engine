#pragma once
#include "common.hpp"
#include "precomp.h"

class Transform
{
public:
	Transform() = default;
	Transform(const Transform&) = default;

	glm::vec3 position = glm::vec3(0.f); 
	glm::vec3 rotation = glm::vec3(0.f);
	glm::vec3 scale = glm::vec3(1.f);

	void setPosition(float pX, float pY, float pZ = 0.f);
	void setRotation(float rX, float rY, float rZ = 0.f);
	void setScale(float sX, float sY, float sZ = 1.f);

	
};

