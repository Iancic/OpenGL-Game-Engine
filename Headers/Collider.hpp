#pragma once
#include "Transform.hpp"

class Collider
{
public:
	Collider() = default;
	~Collider() = default;

	enum class ColliderType
	{
		CIRCLE,
		BOX
	};
private:
};

