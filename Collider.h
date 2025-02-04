#pragma once
#include "Transform.h"

class Collider
{
	Collider();
	~Collider() = default;

	Transform* transform = nullptr;

	float width, height;

	void DrawDebug();
	void UpdateCollider(Transform* owner);
};

