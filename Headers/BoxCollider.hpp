#pragma once
#include "Transform.hpp"
#include "Line.hpp"
#include "Collider.hpp"
#include <functional> // Support for callbacks

class BoxCollider : Collider
{
public:
	BoxCollider();
	~BoxCollider();
	
	void Update(Transform* target, float widthArg, float heightArg); // Synchronise collider 
	void Render();

	Transform* transform = nullptr;

	float width = 50; // Default
	float height = 50; // Default

	// Getter functions for AABB Check
	glm::vec2 UpperLeft();
	glm::vec2 UpperRight();
	glm::vec2 BottomLeft();
	glm::vec2 BottomRight();

	void SetColor(glm::vec4 colorArg);

	glm::vec4 defaultColor{ 255.f, 0.f, 0.f, 255.f };

	ColliderType type = Collider::ColliderType::BOX;

private:
	glm::vec4 color = defaultColor;
};