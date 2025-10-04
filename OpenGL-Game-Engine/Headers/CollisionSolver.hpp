#pragma once
#include "BoxCollider.hpp"
#include "CircleCollider.hpp"
#include "Logger.hpp"

class CollisionSolver
{
private:
	static CollisionSolver* collision_solver_Instance;

	CollisionSolver() {};

public:

	// Prevent copying or assignment 
	CollisionSolver(const CollisionSolver&) = delete;
	CollisionSolver& operator=(const CollisionSolver&) = delete;

	// Get the instance
	static CollisionSolver* getInstance()
	{
		if (collision_solver_Instance == nullptr)
		{
			collision_solver_Instance = new CollisionSolver();
		}

		return collision_solver_Instance;
	}

	void AddCollider(Collider* colliderArg);
	void SolveCollisions(std::vector<Collider*> colliderA, std::vector<Collider*> colliderB);

	bool CheckBoxBoxCollision(BoxCollider* colliderA, BoxCollider* colliderB);
	bool CheckCircleBoxCollision(CircleCollider* colliderA, BoxCollider* colliderB);
	bool CheckCircleCircleCollision(CircleCollider* colliderA, CircleCollider* colliderB);

private:
	std::vector<Collider*> colliders;
};