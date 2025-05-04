#pragma once
#include <glm-master/glm/glm.hpp>
#include <glm-master/glm/gtc/matrix_transform.hpp>
#include <glm-master/glm/gtc/type_ptr.hpp>
#include <vector>

#include "Transform.hpp"
#include "ResourceManager.hpp"
#include "Camera.hpp"
#include "Common.hpp"

class Line
{
private:
	// Singleton
	static Line* line_Instance;
	Line();

	Camera* activeCamera = nullptr;

	Transform* transformPtr = nullptr;
	GLuint VBO, VAO;

	Shader* shaderPtr = nullptr;

	glm::mat4 globalTransform;
	glm::mat4 MVP;

	const std::vector<float>* vertices;

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

	void Render(glm::vec3 startPosition, glm::vec3 endPosition, glm::vec4 color, float lineWidth = 2.f);

	void SetCamera(Camera* cameraRef);
};