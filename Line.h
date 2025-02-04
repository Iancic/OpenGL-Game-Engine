#pragma once
#include <glm-master/glm/glm.hpp>
#include <glm-master/glm/gtc/matrix_transform.hpp>
#include <glm-master/glm/gtc/type_ptr.hpp>
#include <vector>

#include "Transform.h"
#include "ResourceManager.h"
#include "Camera.h"
#include "common.h"

class Line
{
private:
	// Singleton
	static Line* line_Instance;
	Line();

public:

	// Prevent copying or assignment 
	Line(const Line&) = delete;
	Line& operator=(const Line&) = delete;

	// Singleton
	static Line* getInstance()
	{
		if (line_Instance == nullptr)
		{
			line_Instance = new Line();
		}

		return line_Instance;
	}

	glm::mat4 projectionMatrix;

	const std::vector<float>* vertices;

	void Render(glm::vec3 startPosition, glm::vec3 endPosition, glm::vec4 color);

	Transform* transformPtr = nullptr;
	GLuint VBO, VAO;

	Shader* shaderPtr = nullptr;
	

	glm::mat4 globalTransform;
	glm::mat4 MVP;

	glm::vec3 MapToScreen(const glm::vec3& position);

	float left = 0.0f, right = SCREEN_WIDTH, bottom = SCREEN_HEIGHT, top = 0.0f;
	float zoom = 1.0f;  // Default zoom level, 1.0 means no zoom

	void zoomOut(float factor) 
	{
		zoom /= factor; // Decrease the zoom factor
		updateProjection();
	}

	void updateProjection() 
	{
		// Adjust the left, right, top, bottom to zoom in/out
		float width = SCREEN_WIDTH / zoom;
		float height = SCREEN_HEIGHT / zoom;

		// Set the new orthographic projection values
		left = -width / 2.0f;
		right = width / 2.0f;
		bottom = -height / 2.0f;
		top = height / 2.0f;

		// Create the new orthographic projection matrix
		projectionMatrix = glm::ortho(0.0f, width, 0.0f, height);
	}

	glm::mat4 getProjectionMatrix() 
	{
		return projectionMatrix;
	}
};