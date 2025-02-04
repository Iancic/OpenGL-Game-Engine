#pragma once
#include "Transform.h"
#include <glm-master/glm/glm.hpp>
#include <glm-master/glm/gtc/matrix_transform.hpp>
#include <glm-master/glm/gtc/type_ptr.hpp>

class Camera
{
public:
	Camera(float fieldOfView, glm::vec3 position, int cameraWidth, int cameraHeight);
	~Camera() = default;


	Transform* transform = nullptr;
	int width, height;

	float FieldOfView;
	float AspectRatio;
	float NearClipping;
	float FarClipping;

	glm::vec3 Front, Up, Right, WorldUp;

	void UpdateCamera(Transform* target, float speed);
	bool isPlayerInView(const glm::vec3& playerPosition);
	glm::mat4 GetViewMatrix();

	float Interpolate(float a0, float a1, float w, bool isLinear)
	{
		if (isLinear)
			return a0 * (1 - w) + a1 * w;
		else
			return (a1 - a0) * (3.f - w * 2.f) * w * w + a0;
	}
};

