#pragma once
#include <glm-master/glm/glm.hpp> // GLM: Math

struct TransformComponent
{
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
};