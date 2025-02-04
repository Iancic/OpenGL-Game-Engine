#include "Segment.h"

Segment::Segment(Segment* Target, int desiredShape, int desiredRadius, int desiredSpacing)
{
	segmentSpacing = desiredSpacing;
	segmentShape = desiredShape;
	segmentRadius = desiredRadius;

	if (Target == NULL)
		isHead = true;
	else
		target = Target;
}


void Segment::Update()
{
	if (!isHead)
		Constraint();
}

void Segment::Render(Camera* camera)
{
	Circle::getInstance()->DrawCircle(transform.position.x - camera->transform->position.x, transform.position.y - camera->transform->position.y, segmentRadius, segmentShape, glm::vec4(255, 255, 255, 255));
}

void Segment::Constraint()
{
	vector.x = transform.position.x - target->transform.position.x;
	vector.y = transform.position.y - target->transform.position.y;

	float distance = sqrt(vector.x * vector.x + vector.y * vector.y);

	if (distance > segmentSpacing)
	{
		normalized.x = vector.x / distance;
		normalized.y = vector.y / distance;

		scaled.x = segmentSpacing * normalized.x;
		scaled.y = segmentSpacing * normalized.y;

		transform.position.x = (target->transform.position.x + scaled.x);
		transform.position.y = (target->transform.position.y + scaled.y);
	}
}
