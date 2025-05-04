#pragma once
#include "Transform.hpp"
#include "Camera.hpp"
#include "Circle.hpp"

class Bullet
{
public:
	Bullet(Transform origin, glm::vec2 Direction);
	~Bullet() = default;

	float elapsedTime = 0.f;
	float bulletLifespan = 2.f;
	bool isDead = false;

	glm::vec2 direction;

	Transform transform;

	void Update(float deltaTime, float bulletSpeed);
	void Render();

private:
};

