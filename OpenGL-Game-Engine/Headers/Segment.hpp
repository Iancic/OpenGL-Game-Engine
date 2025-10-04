#pragma once
#include "precomp.h"
#include "common.hpp"
#include "Transform.hpp"
#include "Camera.hpp"
#include "Line.hpp"
#include "Circle.hpp"
#include "BoxCollider.hpp"

class Segment
{
public:
	Segment(Segment* Target, int desiredShape, int desiredRadius, int desiredSpacing);
	~Segment() = default;

	bool isHead = false;

	glm::vec2 vector, normalized, scaled, direction;
	glm::vec2 sideA, sideB;

	Segment* target = nullptr;
	Transform transform {};
	BoxCollider* collider;

	void Update();
	void Render(Camera* camera);
	void RenderDebug();

	void Constraint();

	int segmentRadius;
	int segmentSpacing;
	int segmentShape;
};

