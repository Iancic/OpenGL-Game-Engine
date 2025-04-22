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
	// This is the movement constraint
	//
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

	// This is the angle constraint (prevents body from overlapping)
	//
	if (target->target != nullptr)
	{
		// Vector from target->target to target
		float v1x = target->target->transform.position.x - target->transform.position.x;
		float v1y = target->target->transform.position.y - target->transform.position.y;
		float lenght1 = sqrt(v1x * v1x + v1y * v1y);
		glm::vec2 vector1(v1x, v1y);

		// Vector from target to this segment
		float v2x = target->transform.position.x - this->transform.position.x;
		float v2y = target->target->transform.position.y - this->transform.position.y;
		float lenght2 = sqrt(v2x * v2x + v2x * v2x);
		glm::vec2 vector2(v2x, v2y);

		if (lenght1 > 0 && lenght2 > 0)
		{
			vector1 = glm::normalize(vector1);
			vector2 = glm::normalize(vector2);

			// Angle between the vectors
			float dot = glm::dot(vector1, vector2);
			float angle = acosf(dot);

			// Limit the angle if too sharp of a turn
			float maxAngle = M_PI / 2.f; // TODO: This is 90 degress hardcoded but make this adjustable.
			if (angle > maxAngle)
			{
				// Determine what direction should the angle correction be applied to
				float correctionDirection = v1x * v2y - v1y * v2x < 0 ? -1.f : 1.f;
				float correctionAngle = maxAngle * correctionDirection;

				// Rotate the vector by the determine angle correction
				float cosA = cos(correctionAngle);
				float sinA = sin(correctionAngle);

				float rx = v1x * cosA - v1y * sinA;
				float ry = v1x * sinA + v1y * cosA;

				// Move the segment taking into account new positioning
				transform.position.x = target->transform.position.x + rx * segmentSpacing;
				transform.position.y = target->transform.position.y + ry * segmentSpacing;
			}
		}
	}
}
