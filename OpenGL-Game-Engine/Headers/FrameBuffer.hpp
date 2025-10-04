#pragma once
#include "Common.hpp"

#include <glad/glad.h>
#include <vector>
#include "ResourceManager.hpp"

class FrameBuffer
{

public:
	FrameBuffer();
	~FrameBuffer();
	
	GLuint rectVAO, rectVBO;
	GLuint FBO, RBO;
	GLuint framebufferTexture;

	GLuint postprocessedFBO;
	GLuint postprocessedTexture;

	Shader* shader;

	bool scanline = true;

	float vignetteIntensity = 0.2f;
	float curveLeft = 6.f;
	float curveRight = 4.f;
	float abberationIntensity = 0.15f;

	const std::vector<float>* vertices;

	void DrawRenderBuffer(float iTime);

private:

};