#include "precomp.h"
#include "FrameBuffer.hpp"

FrameBuffer::FrameBuffer()
{
	shader = ResourceManager::getInstance()->frameBufferShader;

	vertices = new std::vector<float>
	{
		// Coords    // texCoords
		 1.0f, -1.0f,  1.0f, 0.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		-1.0f,  1.0f,  0.0f, 1.0f,

		 1.0f,  1.0f,  1.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f,  0.0f, 1.0f
	};

	shader->use();
	glUniform1i(glGetUniformLocation(shader->ID, "screenTexture"), 0);

	// Rectangle Texture
	glGenVertexArrays(1, &rectVAO);
	glGenBuffers(1, &rectVBO);
	glBindVertexArray(rectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, rectVBO);
	glBufferData(GL_ARRAY_BUFFER, vertices->size() * sizeof(float), vertices->data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	// Bind Unpostprocessed Frame Buffer
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	// Create Unpostprocessed Frame Buffer Texture
	glGenTextures(1, &framebufferTexture);
	glBindTexture(GL_TEXTURE_2D, framebufferTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // Prevents edge bleeding
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Prevents edge bleeding
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferTexture, 0);

	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCREEN_WIDTH, SCREEN_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

	// Create Postprocessed Frame Buffer
	glGenFramebuffers(1, &postprocessedFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, postprocessedFBO);

	// Create Postprocessed Texture
	glGenTextures(1, &postprocessedTexture);
	glBindTexture(GL_TEXTURE_2D, postprocessedTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postprocessedTexture, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FrameBuffer::~FrameBuffer()
{

}

/*
[Scene Render] -> framebufferTexture
framebufferTexture --(PostProcessing Shader)--> postProcessedTexture
postProcessedTexture -> Screen
postProcessedTexture -> ImGui::Image() - Post Proccesed
framebufferTexture -> ImGui::Image() - Un Post Proccesed Raw original scene
*/

void FrameBuffer::DrawRenderBuffer(float iTime)
{
	// 1. Render POSTPROCESSED result into postprocessedFBO
	glBindFramebuffer(GL_FRAMEBUFFER, postprocessedFBO);

	shader->use();
	shader->setFloat("iTime", iTime);
	shader->setBool("isPostProcessed", true);
	shader->setVec2("iResolution", glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT));
	glBindVertexArray(rectVAO);

	glEnable(GL_STENCIL_TEST);
	glDisable(GL_DEPTH_TEST);

	// The input is the raw scene to which I apply post processing
	glBindTexture(GL_TEXTURE_2D, framebufferTexture);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glEnable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// 2. Render postProcessedTexture to SCREEN
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shader->use();

	shader->setBool("isPostProcessed", true);
	shader->setBool("scanline", scanline);

	shader->setFloat("curveRight", curveRight);
	shader->setFloat("curveLeft", curveLeft);
	shader->setFloat("abberationIntensity", abberationIntensity);
	shader->setFloat("vignetteIntensity", vignetteIntensity);

	glBindVertexArray(rectVAO);

	glDisable(GL_DEPTH_TEST);

	glBindTexture(GL_TEXTURE_2D, postprocessedTexture); // OUTPUT texture
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glEnable(GL_DEPTH_TEST);
}