#include "precomp.h"
#include "BoxCollider.hpp"

BoxCollider::BoxCollider()
{
	transform = new Transform();
}

BoxCollider::~BoxCollider()
{

}

// Origin of a Box Collider is at the center of the box.
// To synchronise position pass center, for size pass width and height of target .
// For circle radius x 2 = widht = height.
void BoxCollider::Update(Transform* target, float widthArg, float heightArg)
{
	transform->position.x = target->position.x;
	transform->position.y = target->position.y;
	transform->position.z = target->position.z;

	width = widthArg;
	height = heightArg;
}

void BoxCollider::Render()
{
	Line::getInstance()->Render(glm::vec3(UpperLeft(), 0.f), glm::vec3(UpperRight(), 0.f), color, 3.f);
	Line::getInstance()->Render(glm::vec3(UpperLeft(), 0.f), glm::vec3(BottomLeft(), 0.f), color, 3.f);
	Line::getInstance()->Render(glm::vec3(BottomLeft(), 0.f), glm::vec3(BottomRight(), 0.f), color, 3.f);
	Line::getInstance()->Render(glm::vec3(UpperRight(), 0.f), glm::vec3(BottomRight(), 0.f), color, 3.f);
}

glm::vec2 BoxCollider::UpperLeft()
{
	return glm::vec2(transform->position.x - width / 2, transform->position.y + height / 2);
}

glm::vec2 BoxCollider::UpperRight()
{
	return glm::vec2(transform->position.x + width / 2, transform->position.y + height / 2);
}

glm::vec2 BoxCollider::BottomLeft()
{
	return glm::vec2(transform->position.x - width / 2, transform->position.y - height / 2);
}

glm::vec2 BoxCollider::BottomRight()
{
	return glm::vec2(transform->position.x + width / 2, transform->position.y - height / 2);
}

void BoxCollider::SetColor(glm::vec4 colorArg)
{
	color = colorArg;
}
