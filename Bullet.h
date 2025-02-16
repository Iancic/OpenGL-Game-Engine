#pragma once
#include "Transform.h"
#include "Camera.h"
#include "Circle.h"

class Bullet
{
public:
	Bullet(Transform origin, Camera* camera, glm::vec2 Direction);
	~Bullet() = default;

	float elapsedTime = 0.f;
	float bulletLifespan = 2.f; // How many seconds till bullet gets destroyed.

	glm::vec2 direction;

	Transform transform;

	void Update(float deltaTime, float bulletSpeed);
	void Render();

private:
};

