#include "precomp.h"
#include "Transform.hpp"

void Transform::setPosition(float pX, float pY, float pZ)
{
	/*
	float normalizedX = (2.0f * pX / SCREEN_WIDTH) - 1.0f;
	float normalizedY = 1.0f - (2.0f * pY / SCREEN_HEIGHT);
	*/
	position = glm::vec3(pX, pY, pZ);
}

void Transform::setRotation(float rX, float rY, float rZ)
{
	rotation = glm::vec3(rX, rY, rZ);
}

void Transform::setScale(float sX, float sY, float sZ)
{
	scale = glm::vec3(sX, sY, sZ);
}