#include "Circle.hpp"

Circle* Circle::circle_Instance = nullptr;

Circle::Circle()
{
	glGenBuffers(1, &circleVBO);
	glGenVertexArrays(1, &circleVAO);

	glBindVertexArray(circleVAO);
	glBindBuffer(GL_ARRAY_BUFFER, circleVBO);

	// Attribute 0
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(0);

	// Attribute 1
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
	glEnableVertexAttribArray(1);

	// Unbind to keep state clean
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Circle::DrawCircle(int centerX, int centerY, int radius, int segmentShape, glm::vec4 color, float dynamicAngle, int rotationDirection)
{
	if (segmentShape == 4)
		offsetAngle = M_PI / 4 + dynamicAngle;  // Square
	else
		offsetAngle = dynamicAngle;  // Spin Effect

	for (int i = 0; i < segmentShape; i++)
	{
		float theta1, theta2;
		if (rotationDirection == 1)
		{
			theta1 = (i * 2.0 * M_PI) / segmentShape + offsetAngle;
			theta2 = ((i + 1) * 2.0 * M_PI) / segmentShape + offsetAngle;
		}
		else
		{
			theta1 = (i * 2.0 * M_PI) / segmentShape - offsetAngle;
			theta2 = ((i + 1) * 2.0 * M_PI) / segmentShape - offsetAngle;
		}

		float x1 = centerX + radius * cos(theta1);
		float y1 = centerY + radius * sin(theta1);
		float x2 = centerX + radius * cos(theta2);
		float y2 = centerY + radius * sin(theta2);

		Line::getInstance()->Render(glm::vec3(x1, y1, 0.f), glm::vec3(x2, y2, 0.f), color);
	}
}

void Circle::DrawSemiCircle(float startX, float startY, float endX, float endY, int radius, float rotationAngle, glm::vec4 color)
{
	// Calculate the center of the semicircle
	float centerX = (startX + endX) / 2;
	float centerY = (startY + endY) / 2;

	// Calculate the angle between the start and end points to determine the orientation of the semicircle
	float dx = endX - startX;
	float dy = endY - startY;
	float angle = atan2(dy, dx);

	// Adjust the angles to span 180 degrees
	float startAngle = angle - M_PI / 2;  // Start at 90 degrees counterclockwise from the angle
	float endAngle = angle + M_PI / 2;    // End at 90 degrees clockwise from the angle

	// Shift The Arc
	startAngle += rotationAngle;  // Start at 90 degrees counterclockwise from the angle
	endAngle += rotationAngle;    // End at 90 degrees clockwise from the angle

	// Draw the semicircle
	int numSegments = 5;  // Number of line segments to approximate the curve
	for (int i = 0; i < numSegments; i++)
	{
		float theta1 = startAngle + i * (endAngle - startAngle) / numSegments;
		float theta2 = startAngle + (i + 1) * (endAngle - startAngle) / numSegments;

		float x1 = centerX + radius * cos(theta1);
		float y1 = centerY + radius * sin(theta1);
		float x2 = centerX + radius * cos(theta2);
		float y2 = centerY + radius * sin(theta2);

		Line::getInstance()->Render(glm::vec3(x1, y1, 0.f), glm::vec3(x2, y2, 0.f), color);
		Line::getInstance()->Render(glm::vec3(x1, y1, 0.f), glm::vec3(centerX, centerY, 0.f), color);
		Line::getInstance()->Render(glm::vec3(x2, y2, 0.f), glm::vec3(centerX, centerY, 0.f), color);
	}
}

void Circle::DrawSemiCircleFilled(float startX, float startY, float endX, float endY, int radius, float rotationAngle, glm::vec4 color, Camera* camera)
{
	// Calculate the center of the semicircle
	float centerX = (startX + endX) / 2;
	float centerY = (startY + endY) / 2;

	// Calculate the angle between the start and end points to determine the orientation of the semicircle
	float dx = endX - startX;
	float dy = endY - startY;
	float angle = atan2(dy, dx);

	// Adjust the angles to span 180 degrees
	float startAngle = angle - M_PI / 2;  // Start at 90 degrees counterclockwise from the angle
	float endAngle = angle + M_PI / 2;    // End at 90 degrees clockwise from the angle

	// Shift The Arc
	startAngle += rotationAngle;  // Start at 90 degrees counterclockwise from the angle
	endAngle += rotationAngle;    // End at 90 degrees clockwise from the angle

	// Draw the semicircle
	int numSegments = 12;  // Number of line segments to approximate the curve
	for (int i = 0; i < numSegments; i++)
	{
		float theta1 = startAngle + i * (endAngle - startAngle) / numSegments;
		float theta2 = startAngle + (i + 1) * (endAngle - startAngle) / numSegments;

		float x1 = centerX + radius * cos(theta1);
		float y1 = centerY + radius * sin(theta1);
		float x2 = centerX + radius * cos(theta2);
		float y2 = centerY + radius * sin(theta2);

		ResourceManager::getInstance()->lineShader->use();
		ResourceManager::getInstance()->lineShader->setMat4("projection", camera->GetProjectionMatrix());
		ResourceManager::getInstance()->lineShader->setMat4("view", camera->GetViewMatrix());
		glm::mat4 model = glm::mat4(1.0f);
		ResourceManager::getInstance()->lineShader->setMat4("model", model);

		circleVertices.push_back({ glm::vec3(x1, y1, 0.f) , glm::vec4(0, 255, 255, 255) });
		circleVertices.push_back({ glm::vec3(x2, y2, 0.f) , glm::vec4(0, 255, 255, 255) });
		circleVertices.push_back({ glm::vec3(centerX, centerY, 0.f) , glm::vec4(0, 255, 255, 255) });
	}

	// Draw Triangles
	glBindBuffer(GL_ARRAY_BUFFER, circleVBO);
	glBufferData(GL_ARRAY_BUFFER, circleVertices.size() * sizeof(Vertex), circleVertices.data(), GL_DYNAMIC_DRAW);

	glBindVertexArray(circleVAO);
	glDrawArrays(GL_TRIANGLES, 0, circleVertices.size() - (circleVertices.size() % 3));

	circleVertices.clear();
	glBindVertexArray(0);
}

void Circle::SendSemiCircleTriangles(float startX, float startY, float endX, float endY, int radius, float rotationAngle, glm::vec4 color, std::vector<Vertex>& spineVertices)
{
	// Calculate the center of the semicircle
	float centerX = (startX + endX) / 2;
	float centerY = (startY + endY) / 2;

	// Calculate the angle between the start and end points to determine the orientation of the semicircle
	float dx = endX - startX;
	float dy = endY - startY;
	float angle = atan2(dy, dx);

	// Adjust the angles to span 180 degrees
	float startAngle = angle - M_PI / 2;  // Start at 90 degrees counterclockwise from the angle
	float endAngle = angle + M_PI / 2;    // End at 90 degrees clockwise from the angle

	// Shift The Arc
	startAngle += rotationAngle;  // Start at 90 degrees counterclockwise from the angle
	endAngle += rotationAngle;    // End at 90 degrees clockwise from the angle

	// Draw the semicircle
	int numSegments = 5;  // Number of line segments to approximate the curve
	for (int i = 0; i < numSegments; i++)
	{
		float theta1 = startAngle + i * (endAngle - startAngle) / numSegments;
		float theta2 = startAngle + (i + 1) * (endAngle - startAngle) / numSegments;

		float x1 = centerX + radius * cos(theta1);
		float y1 = centerY + radius * sin(theta1);
		float x2 = centerX + radius * cos(theta2);
		float y2 = centerY + radius * sin(theta2);

		// ChatGPT: calculate UVs based on angle around center
		float uvx1 = 0.5f + 0.5f * cos(theta1 - angle);
		float uvy1 = 0.5f + 0.5f * sin(theta1 - angle);
		float uvx2 = 0.5f + 0.5f * cos(theta2 - angle);
		float uvy2 = 0.5f + 0.5f * sin(theta2 - angle);
		float uvcx = 0.5f;
		float uvcy = 0.5f;

		spineVertices.push_back({ glm::vec3(x1, y1, 0.f), color, glm::vec2(uvx1, uvy1) });
		spineVertices.push_back({ glm::vec3(x2, y2, 0.f), color, glm::vec2(uvx2, uvy2) });
		spineVertices.push_back({ glm::vec3(centerX, centerY, 0.f), color, glm::vec2(uvcx, uvcy) });
	}
}