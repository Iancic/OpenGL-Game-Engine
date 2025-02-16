#include "Bullet.h"

Bullet::Bullet(Transform origin, Camera* camera, glm::vec2 Direction)
{
	direction = Direction;
	transform.position.x = origin.position.x - camera->transform->position.x;
	transform.position.y = origin.position.y - camera->transform->position.y;
}

void Bullet::Update(float deltaTime, float bulletSpeed)
{
	transform.position += glm::vec3(direction, 0.f) * deltaTime * bulletSpeed;

	elapsedTime += deltaTime;

	if (elapsedTime > bulletLifespan)
	{
		delete this;
	}
}

void Bullet::Render()
{
	Circle::getInstance()->DrawCircle(transform.position.x, transform.position.y, 5.f, 36, glm::vec4(255, 255, 255, 255));
	//DrawCircle(renderer, transform.position.x, transform.position.y, 5.f);
}