#include "CollisionSolver.hpp"

CollisionSolver* CollisionSolver::collision_solver_Instance = nullptr;

void CollisionSolver::AddBroadPhaseCollider(BoxCollider* colliderArg, std::vector<BoxCollider*> narrowPhaseCollidersArg)
{
	broadPhaseColliders.push_back(colliderArg);
	colliderArg->narrowPhaseColliders = narrowPhaseCollidersArg;
}

bool CollisionSolver::CheckCollision(BoxCollider* colliderA, BoxCollider* colliderB)
{
	// Upper Left = return glm::vec2(transform->position.x - width / 2, transform->position.y + height / 2);
	return (colliderA->UpperLeft().x + colliderA->width >= colliderB->UpperLeft().x &&
		colliderB->UpperLeft().x + colliderB->width >= colliderA->UpperLeft().x) &&
		(colliderA->UpperLeft().y >= colliderB->UpperLeft().y - colliderB->height &&
			colliderB->UpperLeft().y >= colliderA->UpperLeft().y - colliderA->height);
}

void CollisionSolver::SolveBroadCollisions()
{
	if (broadPhaseColliders.size() >= 2)
	{
		for (int i = 0; i < broadPhaseColliders.size(); i++)
		{
			for (int j = i + 1; j < broadPhaseColliders.size(); j++)
			{
				if (CheckCollision(broadPhaseColliders[i], broadPhaseColliders[j]))
				{
					broadPhaseColliders[i]->SetColor(glm::vec4(0.f, 255.f, 0.f, 255.f));
					broadPhaseColliders[j]->SetColor(glm::vec4(0.f, 255.f, 0.f, 255.f));
					SolveNarrowCollisions(broadPhaseColliders[i]->narrowPhaseColliders, broadPhaseColliders[j]->narrowPhaseColliders);
				}
				else
				{
					broadPhaseColliders[i]->SetColor(broadPhaseColliders[i]->defaultColor);
					broadPhaseColliders[j]->SetColor(broadPhaseColliders[i]->defaultColor);
				}
			}
		}
	}
}

void CollisionSolver::SolveNarrowCollisions(std::vector<BoxCollider*> narrowA, std::vector<BoxCollider*> narrowB)
{
	for (auto& colliderA : narrowA)
	{
		for (auto& colliderB : narrowB)
		{
			if (CheckCollision(colliderA, colliderB))
			{
				colliderA->SetColor(glm::vec4(0.f, 255.f, 0.f, 255.f));
				colliderB->SetColor(glm::vec4(0.f, 255.f, 0.f, 255.f));
			}
			else
			{
				colliderA->SetColor(colliderA->defaultColor);
				colliderB->SetColor(colliderB->defaultColor);
			}
		}
	}
}