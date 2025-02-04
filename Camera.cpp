#include "Camera.h"


Camera::Camera(float fieldOfView, glm::vec3 position, int cameraWidth, int cameraHeight)
	: Front(glm::vec3(0.0f, 0.0f, -1.0f)), WorldUp(glm::vec3(0.0f, 1.0f, 0.0f))
{
	transform = new Transform();
	transform->position = position;
	width = cameraWidth;
	height = cameraHeight;

	FieldOfView = fieldOfView;
	AspectRatio = width / height;
	NearClipping = 1000.0f;
	FarClipping = 1000.f; // Check for a smarter way to set clipping spaces.
}

void Camera::UpdateCamera(Transform* target, float speed)
{
	Transform followTarget = *target;

	float targetx = static_cast<float>(followTarget.position.x - width / 2);
	float targety = static_cast<float>(followTarget.position.y - height / 2);

	transform->position.x = Interpolate(transform->position.x, targetx, speed, true);
	transform->position.y = Interpolate(transform->position.y, targety, speed, true);
}

bool Camera::isPlayerInView(const glm::vec3& playerPosition)
{
	// Check if the player is within the camera's viewport.
		// We assume the camera's position is at the center of its view.

		// Camera's view boundaries
	float cameraLeft = transform->position.x - SCREEN_WIDTH / 2.0f;
	float cameraRight = transform->position.x + SCREEN_WIDTH / 2.0f;
	float cameraTop = transform->position.y - SCREEN_HEIGHT / 2.0f;
	float cameraBottom = transform->position.y + SCREEN_HEIGHT / 2.0f;

	// Check if the player is within these boundaries
	if (playerPosition.x >= cameraLeft && playerPosition.x <= cameraRight &&
		playerPosition.y >= cameraBottom && playerPosition.y <= cameraTop) {
		return true;  // Player is in view
	}

	return false;  // Player is out of view
}

glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(transform->position, transform->position + Front, Up);
}
