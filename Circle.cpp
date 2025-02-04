#include "Circle.h"

Circle* Circle::circle_Instance = nullptr;

Circle::Circle()
{
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

		int x1 = centerX + radius * cos(theta1);
		int y1 = centerY + radius * sin(theta1);
		int x2 = centerX + radius * cos(theta2);
		int y2 = centerY + radius * sin(theta2);

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

		int x1 = centerX + radius * cos(theta1);
		int y1 = centerY + radius * sin(theta1);
		int x2 = centerX + radius * cos(theta2);
		int y2 = centerY + radius * sin(theta2);

		Line::getInstance()->Render(glm::vec3(x1, y1, 0.f), glm::vec3(x2, y2, 0.f), color);
	}
}
