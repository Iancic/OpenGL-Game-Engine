#pragma once
#include "BoxCollider.hpp"
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

	// Collision Solver provides you with:
	// An interface to all broad phase colliders.
	// When a broad phase collision has occured, each colldier contains pointer to Owner.
	// Owner -> OnCollision() and Owner -> narrowPhaseColliders()

	void AddBroadPhaseCollider(BoxCollider* colliderArg, std::vector<BoxCollider*> narrowPhaseCollidersArg);
	void SolveBroadCollisions();
	void SolveNarrowCollisions(std::vector<BoxCollider*> narrowA, std::vector<BoxCollider*> narrowB);
	bool CheckCollision(BoxCollider* colliderA, BoxCollider* colliderB);

private:

	std::vector<BoxCollider*> broadPhaseColliders;
};