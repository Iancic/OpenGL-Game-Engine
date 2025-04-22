#include "IKSegment.h"

IKSegment::IKSegment()
{
	endEffector->position = glm::vec3{ 0 };
	base->position = glm::vec3{ 0 };
}

void IKSegment::Render(Camera* activeCamera)
{
	Line::getInstance()->Render(glm::vec3(endEffector->position.x, endEffector->position.y, 0.f), glm::vec3(base->position.x, base->position.y, 0.f), glm::vec4(255, 255, 255, 255));
}

// From end effector to the base
void IKSegment::ForwardPass(float target_x, float target_y) 
{
    // Move the end effector to the target
    endEffector->position.x = target_x;
    endEffector->position.y = target_y;

    // Move the leg segment backwards to the base
    float dist = glm::distance(glm::vec2(base->position.x, base->position.y), glm::vec2(endEffector->position.x, endEffector->position.y));
    float ratio = lenght / dist;

    // Normalize the direction vector and adjust the joint
    endEffector->position.x = base->position.x + ratio * (endEffector->position.x - base->position.x);
    endEffector->position.y = base->position.y + ratio * (endEffector->position.y - base->position.y);
}

// From base to end effector.
void IKSegment::BackwardPass()
{
    // Move the leg segment forwards from base
    float dist = glm::distance(glm::vec2(endEffector->position.x, endEffector->position.y), glm::vec2(base->position.x, base->position.y));
    float ratio = lenght / dist;

    // Normalize the direction vector and adjust the joint
    base->position.x = endEffector->position.x + ratio * (base->position.x - endEffector->position.x);
    base->position.y = endEffector->position.y + ratio * (base->position.y - endEffector->position.y);
}
