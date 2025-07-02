#pragma once
#include "Transform.hpp"
#include "Line.hpp"

class IKSegment
{
public:
    IKSegment();
    ~IKSegment() {};

    float lenght = 10;

    Transform* endEffector = new Transform();
    Transform* base = new Transform();

    void Render(float legWidth);
    
    void ForwardPass(float target_x, float target_y);  // From end effector to the base
    void BackwardPass(); // From base to end effector

private:
};