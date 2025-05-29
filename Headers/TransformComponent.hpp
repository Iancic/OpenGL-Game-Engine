#pragma once
#include <glm-master/glm/glm.hpp>

struct TransformComponent
{
	glm::vec3 Translation;
	glm::vec3 Rotation;
	glm::vec3 Scale;
};