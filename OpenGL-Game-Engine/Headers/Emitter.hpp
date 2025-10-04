#pragma once
#include "Transform.hpp"
#include "Random.hpp"
#include "Camera.hpp"
#include "Circle.hpp"
#include "SpriteRenderer.hpp"
#include "TransformComponent.hpp"

struct ParticleProperties
{
	Transform transform;

	glm::vec2 velocity = glm::vec2{ 1.f, 0.5f };
	glm::vec4 colorBegin = glm::vec4{ 0.f, 0.5f, 0.f, 1.f }, colorEnd = glm::vec4{ 1.f };

	float velocityVariationX = 15.f, velocityVariationY = 15.f;

	float sizeBegin = 15.0f, sizeEnd = 0.0f, sizeVariation = 0.5f;
	float lifeTime = 3.f;
};

// TODO: turn this into DOD, SIMD
// TODO: compute? geometry shader?
// TODO: batch rendering

class Emitter
{
public:
	Emitter();

	void Emit(TransformComponent* transformArg);
	void Update(float deltaTime, TransformComponent* transformArg);
	void Render(Camera* activeCamera);

	bool isEmitting = true;

	ParticleProperties properties;

private:

	struct Particle
	{
		Transform transform;

		glm::vec2 velocity = glm::vec2{ 60.f };
		glm::vec4 color, colorBegin, colorEnd;


		float rotation = 0.f;
		float size, sizeBegin, sizeEnd;
		float lifeTime = 5.f, lifeTimeRemaining = 1.f;

		float rotationSpeed = 2.f;
	};

	std::vector<Particle*> particlePool; // We allocate one big pool of particles.

};