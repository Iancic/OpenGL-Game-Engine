#pragma once
#include <glm-master/glm/glm.hpp>
#include <glm-master/glm/gtc/matrix_transform.hpp>
#include <glm-master/glm/gtc/type_ptr.hpp>

struct Vertex 
{
	glm::vec3 position;
	glm::vec4 color;
	glm::vec2 texCoord;
};