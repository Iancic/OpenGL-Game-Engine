#pragma once
#include "Line.hpp"
#include "Vertex.hpp"
#include "Logger.hpp"
#include "Camera.hpp"
#include "Vertex.hpp"

class Circle
{
private:
	// Singleton
	static Circle* circle_Instance;
	Circle();

public:

	// Prevent copying or assignment 
	Circle(const Circle&) = delete;
	Circle& operator=(const Circle&) = delete;

	// Singleton
	static Circle* getInstance()
	{
		if (circle_Instance == nullptr)
		{
			circle_Instance = new Circle();
		}

		return circle_Instance;
	}

	double offsetAngle = 0;
	double dynamicAngle;

	GLuint circleVAO, circleVBO;
	std::vector<Vertex>circleVertices;

	void DrawCircle(int centerX, int centerY, float radius, int segmentShape, glm::vec4 color, float dynamicAngleArg = 0.f, int rotationDirection = 1);
	void DrawSemiCircle(float startX, float startY, float endX, float endY, int radius, float rotationAngle, glm::vec4 color);
	void DrawSemiCircleFilled(float startX, float startY, float endX, float endY, int radius, float rotationAngle, glm::vec4 color, Camera* camera);
	void SendSemiCircleTriangles(float startX, float startY, float endX, float endY, int radius, float rotationAngle, glm::vec4 color, std::vector<Vertex>&spineVertices);

};