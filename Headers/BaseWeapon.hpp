#pragma once
#include <vector>
#include "Transform.hpp"
#include "Bullet.hpp"
#include "Camera.hpp"
#include "Line.hpp"

class BaseWeapon
{
public:
	BaseWeapon(Transform* Slot, Camera* camera);
	~BaseWeapon() = default;

	// TODO: SET THESE AS WEAPON TYPE
	float magSize = 16;
	float reloadTime = 2.5f;
	float reloadCounter = 0.f;
	float lenght = 30.f;
	float bulletSpeed = 500.f;

	bool canShoot = false;

	std::vector<Bullet*> ammunition{};

	Transform* slotPosition;

	Transform transform{};
	Transform target{};

	Camera* activeCamera;

	glm::vec2 vector{}, normalized{}, magnitude{}, direction{};

	void Update(Transform& Target, float deltaTime);
	void Render();

	void Shoot();

private:
};

