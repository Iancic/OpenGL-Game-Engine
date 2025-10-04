#include "precomp.h"
#include "Boid.hpp"

Boid::Boid()
{
	transform->setPosition(800, 800);
}

void Boid::Render()
{
	Circle::getInstance()->DrawCircle(transform->position.x, transform->position.y, 10, 3, glm::vec4(255, 255, 255, 255));
}

void Boid::Update(std::vector<Boid*> boidPool)
{
	accumulatedDirection = Behaviour(boidPool);

	accumulatedDirection = glm::vec3(1.f, 1.f, 0.f);

	transform->position += glm::vec3(accumulatedDirection, 0);
}

glm::vec2 Boid::Behaviour(std::vector<Boid*> boidPool)
{
	glm::vec2 finalDirection = { 0, 0 };
	/*
	for (auto boid : boidPool)
	{

	}
	*/

	return finalDirection;
}

glm::vec2 Boid::Separation()
{
	return glm::vec2{};
}

glm::vec2 Boid::Cohesion()
{
	return glm::vec2{};
}

glm::vec2 Boid::Alignment()
{
	return glm::vec2{};
}