#include "BallObject.h"



BallObject::BallObject(glm::vec4 position, float radius, float mass) :
    position(position),
    radius(radius),
    mass(mass)
{
}


BallObject::~BallObject()
{
}
