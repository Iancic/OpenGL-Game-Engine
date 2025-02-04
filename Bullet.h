#pragma once
#include <SDL.h>
#include "Transform.h"
#include "Camera.h"

class Bullet
{
public:
	Bullet(Transform origin, Camera* camera, glm::vec2 Direction);
	~Bullet() = default;

	float speed = 180.f;

	glm::vec2 direction;

	Transform transform;

	void Update(float deltaTime, float bulletSpeed);
	void Render();

private:
};

