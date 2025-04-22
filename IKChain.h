#pragma once
#include "IKSegment.h"
#include <vector>

class IKChain
{
public:

	IKChain(glm::vec2 startPos);
	~IKChain();

	std::vector<IKSegment*> segments;
	int chainLenght = 5;

	void SolveIK(float targetX, float targetY, float startX, float startY);
	void Render(Camera* camera);

	glm::vec2 GetEndPosition();

	void AddSegment();
	void RemoveSegment();

private:
};