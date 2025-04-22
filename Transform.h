#pragma once
#include "common.h"

#include <glm-master/glm/glm.hpp>
#include <glm-master/glm/gtc/matrix_transform.hpp>
#include <glm-master/glm/gtc/type_ptr.hpp>

class Transform
{
public:
	glm::vec3 position = glm::vec3(0.f); 
	glm::vec3 rotation;
	glm::vec3 scale;

	void setPosition(float pX, float pY, float pZ = 0.f);
	void setRotation(float rX, float rY, float rZ = 0.f);
	void setScale(float sX, float sY, float sZ = 1.f);
};

