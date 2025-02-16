#pragma once
#include "common.h"

#include <glad/glad.h>
#include <vector>
#include "ResourceManager.h"

class FrameBuffer
{

public:
	FrameBuffer();
	~FrameBuffer();

	
	GLuint rectVAO, rectVBO;
	GLuint FBO, RBO;
	GLuint framebufferTexture;

	Shader* shader;

	const std::vector<float>* vertices;

	void DrawRenderBuffer(float iTime, bool isPostProcessed);

private:

};