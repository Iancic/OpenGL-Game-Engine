#include "precomp.h"
#include "Emitter.hpp"

Emitter::Emitter()
{
	//particlePool.resize(10);
}

void Emitter::Emit(TransformComponent* transformArg)
{
	float emitterPosX = transformArg->GetTranslation(transformArg->WorldMatrix).x;
	float emitterPosY = transformArg->GetTranslation(transformArg->WorldMatrix).y;

	// Set particle witht the next index iin pool to active and initialize it's properties.
	particlePool.push_back(new Particle());

	particlePool[particlePool.size() - 1]->transform.position.x = emitterPosX;
	particlePool[particlePool.size() - 1]->transform.position.y = emitterPosY;

	// Velocity
	particlePool[particlePool.size() - 1]->velocity.x += Random::RandomFloat() * properties.velocityVariationX;
	particlePool[particlePool.size() - 1]->velocity.y += Random::RandomFloat() * properties.velocityVariationY;

	// Color
	particlePool[particlePool.size() - 1]->colorBegin = properties.colorBegin;
	particlePool[particlePool.size() - 1]->colorEnd = properties.colorEnd;

	// Others
	particlePool[particlePool.size() - 1]->lifeTime = properties.lifeTime;
	particlePool[particlePool.size() - 1]->lifeTimeRemaining = properties.lifeTime;
	particlePool[particlePool.size() - 1]->sizeBegin = properties.sizeBegin + properties.sizeVariation * (Random::RandomFloat() - 0.5f);
	particlePool[particlePool.size() - 1]->sizeEnd = properties.sizeEnd; // Size end and begin used for interpolation
}

void Emitter::Update(float deltaTime, TransformComponent* transformArg)
{
	//if (isEmitting) Emit(properties, transformArg);

	for (size_t i = 0; i < particlePool.size(); ++i )
	{
		Particle* particle = particlePool[i];

		if (particle->lifeTimeRemaining <= 0.0f)
		{
			delete particle;
			particlePool.erase(particlePool.begin() + i);
			continue;
		}

		particle->lifeTimeRemaining -= deltaTime;
		particle->transform.position += glm::vec3{ particle->velocity.x * deltaTime,  particle->velocity.y * deltaTime, 0.f };
		particle->rotation -= particle->rotationSpeed * deltaTime;

		float interpolationFactor = 1.0f - (particle->lifeTimeRemaining / particle->lifeTime);

		particle->color = glm::mix(particle->colorBegin, particle->colorEnd, interpolationFactor);
		particle->size = glm::mix(particle->sizeBegin, particle->sizeEnd, interpolationFactor);
	}
}

void Emitter::Render(Camera* activeCamera)
{
	for (auto& particle : particlePool)
		Circle::getInstance()->DrawCircle(particle->transform.position.x, particle->transform.position.y, particle->size, 5, particle->color, particle->rotation);
}