#include "precomp.h"
#include "BaseWeapon.hpp"

BaseWeapon::BaseWeapon(Transform* Slot, Camera* camera)
{
	slotPosition = Slot;
	activeCamera = camera;

	// Default Value For Gun Orientation
	target.position.x = 600;
	target.position.y = 600;
}

void BaseWeapon::Update(Transform& Target, float deltaTime)
{
	// Bullet Updates
	for (auto& bullet : ammunition)
	{
		bullet->Update(deltaTime, bulletSpeed);
	}

	// Remove dead bullets
	ammunition.erase( std::remove_if(ammunition.begin(), ammunition.end(),[](Bullet* bullet) 
		{
			if (bullet->isDead)
			{
				delete bullet;
				return true;
			}
			return false;
		}
	),ammunition.end()
	);


	// Weapon Updates
	transform.position = slotPosition->position;

	target.position = Target.position;

	vector.x = target.position.x - transform.position.x;
	vector.y = target.position.y - transform.position.y;

	reloadCounter += deltaTime;
	if (reloadCounter >= reloadTime)
	{
		canShoot = true;
	}
}

void BaseWeapon::Render()
{
	magnitude.x = target.position.x - transform.position.x;
	magnitude.y = target.position.y - transform.position.y;

	float distance = sqrt(magnitude.x * magnitude.x + magnitude.y * magnitude.y);

	float normalizedX = (target.position.x - transform.position.x) / distance;
	float normalizedY = (target.position.y - transform.position.y) / distance;

	direction = { normalizedX, normalizedY };

	float finalposX = normalizedX * lenght;
	float finalposY = normalizedY * lenght;

	Line::getInstance()->Render(glm::vec3(transform.position.x, transform.position.y, 0.f), glm::vec3(transform.position.x + finalposX, transform.position.y + finalposY, 0.f), 
								glm::vec4(255, 0, 0, 255), 3.5f);

	for (auto& bullet : ammunition)
	{
		if (!bullet->isDead)
			bullet->Render();
	}
}

void BaseWeapon::Shoot()
{
	ammunition.push_back(new Bullet(transform, direction));
	reloadCounter = 0.f;
	canShoot = false;
}
