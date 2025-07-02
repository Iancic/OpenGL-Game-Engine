#include "Camera.hpp"
#include <algorithm> // for std::clamp
#include <cmath>     // for std::fabs if needed

Camera::Camera(float fieldOfView, glm::vec3 position, int cameraWidth, int cameraHeight)
	: Front(glm::vec3(0.0f, 0.0f, -1.0f)), WorldUp(glm::vec3(0.0f, 1.0f, 0.0f))
{
	transform = new Transform();
	transform->position = position;
	width = cameraWidth;
	height = cameraHeight;

	FieldOfView = fieldOfView;
	AspectRatio = width / height;
	NearClipping = 0.1f;
	FarClipping = 1000.f;
}

void Camera::UpdateCamera(Transform* target, glm::vec2 playerDir, float deltaTime, bool inputPressed)
{

	if (!inputPressed)
	{
		// Get the target position and center the camera on it
		float targetx = static_cast<float>(target->position.x);
		float targety = static_cast<float>(target->position.y);

		// Apply time-based interpolation
		float speed = followSpeed * deltaTime; //std::clamp(followSpeed * deltaTime, 0.0f, 1.0f);

		transform->position.x = Interpolate(transform->position.x, targetx, speed, true);
		transform->position.y = Interpolate(transform->position.y, targety, speed, true);
	}
	else
	{
		// Get the target position and center the camera on it
		float targetx = static_cast<float>(target->position.x);
		float targety = static_cast<float>(target->position.y);

		transform->position.x = targetx - playerDir.x * distanceFromPlayer;
		transform->position.y = targety - playerDir.y * distanceFromPlayer;
	}
}

void Camera::zoomOut(float factor, const glm::vec2& target)
{
	zoom /= factor; // Decrease the zoom factor
	updateProjection();
}

void Camera::updateProjection()
{
	// Adjust the left, right, top, bottom to zoom in/out
	float viewWidth = SCREEN_WIDTH / zoom;
	float viewHeight = SCREEN_HEIGHT / zoom;

	glm::vec2 center(transform->position.x, transform->position.y);

	left = center.x - viewWidth / 2.0f;
	right = center.x + viewWidth / 2.0f;
	bottom = center.y - viewHeight / 2.0f;
	top = center.y + viewHeight / 2.0f;

	projectionMatrix = glm::ortho(left, right, bottom, top);
}

glm::mat4 Camera::GetProjectionMatrix()
{
	return projectionMatrix;
}

glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(transform->position, transform->position + Front, Up);
}

float Camera::Interpolate(float a0, float a1, float w, bool isLinear)
{
	if (isLinear)
		return a0 * (1 - w) + a1 * w;
	else
		return (a1 - a0) * (3.f - w * 2.f) * w * w + a0;
}
