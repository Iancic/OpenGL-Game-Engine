#pragma once
#include "common.h"
#include <SDL.h>
#include "Transform.h"
#include "Camera.h"
#include "Line.h"
#include "Circle.h"

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

	void Update();
	void Render(Camera* camera);

	void Constraint();

	int segmentRadius;
	int segmentSpacing;
	int segmentShape;
};

