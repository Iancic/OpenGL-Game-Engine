#pragma once
#include <glm-master/glm/glm.hpp>
#include <glm-master/glm/gtc/matrix_transform.hpp>
#include <glm-master/glm/gtc/type_ptr.hpp>

#include <cstdint>
#include <random>

//constexpr uint32_t starColours[3]
//{
//	0xFFFFFFFF, 0xFFD9FFFF, 0xFFA3FFFF
//};

class Star
{

public:
	Star(float x, float y, int difficulty);


	float positionX;
	float positionY;

	uint32_t nLehmer = 0;
	uint32_t Lehmer32()
	{
		nLehmer += 0xe120fc15;
		uint64_t tmp;
		tmp = (uint64_t)nLehmer * 0x4a39b70d;
		uint32_t m1 = (tmp >> 32) ^ tmp;
		tmp = (uint64_t)m1 * 0x12fad5c9;
		uint32_t m2 = (tmp >> 32) ^ tmp;
		return m2;
	}

	bool isDead = false;

	int segments = 0;
	int rotationSpeed = 0;
	int rotationDirection = 1; // -1 or 1
	bool starExists = false;
	float starDiameter = 0.f;
	glm::vec4 starColour  = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

	glm::vec4 starColours[8] = {
		glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), // Red (Fully opaque)
		glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), // Green (Fully opaque)
		glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), // Blue (Fully opaque)
		glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), // Yellow (Fully opaque)
		glm::vec4(1.0f, 0.0f, 1.0f, 1.0f), // Magenta (Fully opaque)
		glm::vec4(0.0f, 1.0f, 1.0f, 1.0f), // Cyan (Fully opaque)
		glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), // White (Fully opaque)
		glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)  // Gray (Fully opaque)
	};

	double rndDouble(double min, double max);
	int rndInt(int min, int max);
};