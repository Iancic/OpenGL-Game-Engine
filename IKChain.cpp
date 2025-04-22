#include "IKChain.h"

IKChain::IKChain(glm::vec2 startPos)
{
	for (int i = 0; i < chainLenght; i++)
	{
		segments.push_back(new IKSegment());
		segments[i]->base->position = glm::vec3{ glm::vec3(startPos.x, startPos.y, 0) + (static_cast<float>(i)) * segments[i]->lenght};
		segments[i]->endEffector->position = glm::vec3{ glm::vec3(startPos.x, startPos.y, 0) + (static_cast<float>(i) + 1.f) * segments[i]->lenght };
	}

}

IKChain::~IKChain()
{

}

void IKChain::SolveIK(float targetX, float targetY, float startX, float startY)
{
	// Backward Phase
	// Move last segment's end to the target
	segments[chainLenght - 1]->endEffector->position = glm::vec3(targetX, targetY, 0);

	for (int i = chainLenght - 1; i > 0; --i)
	{
		// Set base to current position
		segments[i]->BackwardPass(); // pulls base back to maintain length

		// Set the next segment's endEffector to this one's base
		segments[i - 1]->endEffector->position = segments[i]->base->position;
	}

	// Forward Phase
	segments[0]->base->position = glm::vec3(startX, startY, 0);

	for (int i = 0; i < chainLenght; ++i)
	{
		// Move endEffector forward from base
		segments[i]->ForwardPass(segments[i]->endEffector->position.x, segments[i]->endEffector->position.y);

		// Set the next base to this one's endEffector
		if (i + 1 < chainLenght)
		{
			segments[i + 1]->base->position = segments[i]->endEffector->position;
		}
	}
}

void IKChain::Render(Camera* camera)
{
	for (int i = 0; i < chainLenght; i++)
	{
		segments[i]->Render(camera);
	}
}

glm::vec2 IKChain::GetEndPosition()
{
	return glm::vec2(
		segments[chainLenght - 1]->endEffector->position.x,
		segments[chainLenght - 1]->endEffector->position.y
	);
}

void IKChain::AddSegment()
{
	IKSegment* lastSegment = segments.empty() ? nullptr : segments.back();
	glm::vec3 basePos = lastSegment ? lastSegment->endEffector->position : glm::vec3(0.0f);
	float newLength = lastSegment ? lastSegment->lenght : 1.0f;

	IKSegment* newSegment = new IKSegment();
	newSegment->lenght = newLength; // Optional: match previous segment
	newSegment->base->position = basePos;
	newSegment->endEffector->position = basePos + glm::vec3(newLength, 0, 0); // Default direction: right

	segments.push_back(newSegment);
	chainLenght++;
}

void IKChain::RemoveSegment()
{
	if (!segments.empty())
	{
		delete segments.back();     // Free memory
		segments.pop_back();        // Remove from vector
		chainLenght = static_cast<int>(segments.size());
	}
}
