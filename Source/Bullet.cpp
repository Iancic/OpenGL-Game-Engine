#include "Bullet.hpp"

Bullet::Bullet(Transform origin, glm::vec2 Direction)
{
	direction = Direction;
	transform.position.x = origin.position.x;
	transform.position.y = origin.position.y;
}

void Bullet::Update(float deltaTime, float bulletSpeed)
{
	transform.position += glm::vec3(direction, 0.f) * deltaTime * bulletSpeed;

	elapsedTime += deltaTime;

	if (elapsedTime > bulletLifespan)
	{
		isDead = true;
	}
}

void Bullet::Render()
{
	Circle::getInstance()->DrawCircle(transform.position.x, transform.position.y, 5.f, 36, glm::vec4(255, 255, 255, 255));
}