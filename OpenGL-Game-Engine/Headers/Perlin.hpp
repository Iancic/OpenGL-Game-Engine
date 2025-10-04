#pragma once
#include <random>
#include <vector>
#include <glad/glad.h>
#include <glm-master/glm/glm.hpp>
#include <glm-master/glm/gtc/matrix_transform.hpp>
#include <glm-master/glm/gtc/type_ptr.hpp>

class Perlin
{
public:

	Perlin();

	unsigned int perlinTexture; // ID
	unsigned int SEED;

	// Texture Attributes
	bool ascending = true;
	
	bool isMoving = false, isPulsating = true;
	int width = 512, height = 512;
	float pulseTime;
	float zoomFactor = 15.0f, dynamicSpeed = 200.f;
	int redChannel = 255, blueChannel = 255, greenChannel = 255;

	std::vector<unsigned char> perlinData;

	float Smoothstep(float t);
	float Interpolate(float a0, float a1, float w, bool isLinear);
	float DotGridGradient(int ix, int iy, float x, float y);
	float SampleNoise(float x, float y);

	void GenerateTexture();

	void NewNoise();
	std::vector<unsigned char> GenerateNoise(int widthArg, int heightArg, float scaleArg);

	glm::vec2 RandomGradient(int ix, int iy);
};