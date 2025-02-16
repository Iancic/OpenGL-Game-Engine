#pragma once
#include <vector>
#include "Line.h"
#include "Circle.h"
#include "Transform.h"

class Boid
{
public:
	Boid();
	~Boid() {};

	Transform* transform = new Transform();

	// Each Behavioural Pattern Intensity
	float separationFactor = 1.f, cohesionFactor = 1.f, alignmentFactor = 1.f;

	// Each Behavioural Pattern Range
	float separationRange = 1.f, cohesionRange = 1.f, alignmentRange = 1.f;

	glm::vec2 accumulatedDirection{};//vector{}, normalized{}, magnitude{}, direction{};

	void Render(Camera* activeCamera);
	void Update(std::vector<Boid*> boidPool);

	glm::vec2 Behaviour(std::vector<Boid*> boidPool);
	glm::vec2 Separation();
	glm::vec2 Cohesion();
	glm::vec2 Alignment();

	
};