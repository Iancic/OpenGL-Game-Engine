#pragma once
#include "Transform.hpp"

class Camera
{
public:
	Camera(float fieldOfView, glm::vec3 position, int cameraWidth, int cameraHeight);
	~Camera() = default;

	Transform* transform = nullptr;
	int width, height;

	float desiredZoom = 3.5f;
	float followSpeed = 1.f;
	float distanceFromPlayer = 1.f;

	float FieldOfView, AspectRatio, NearClipping, FarClipping;

	int counterLeft = 0, counterRight = 0, counterTop = 0, counterBot = 0;

	glm::vec3 Front, Up, Right, WorldUp;

	float Interpolate(float a0, float a1, float w, bool isLinear);
	void UpdateCamera(Transform* target, glm::vec2 playerDir, float deltaTime, bool inputPressed);

	float left = 0.0f, right = SCREEN_WIDTH, bottom = SCREEN_HEIGHT, top = 0.0f;
	float zoom = 1.0f;  // Default zoom level, 1.0 means no zoom

	glm::mat4 projectionMatrix;
	glm::mat4 view = glm::mat4(1.0f);

	void zoomOut(float factor, const glm::vec2& target);


	void updateProjection(const glm::vec2& target);

	glm::mat4 GetProjectionMatrix();
	glm::mat4 GetViewMatrix();

};

