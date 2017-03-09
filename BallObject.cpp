#include "BallObject.h"
#include <math.h>

BallObject::BallObject(glm::vec4 position, float radius, float mass) :
    position(position),
    radius(radius),
    mass(mass)
{
    rot1 = rand() % 4;
    rot2 = rand() % 4;
    rotdir = rand() % 2 * 2 - 1;
    while (rot2 == rot1)
    {
        rot2 = rand() % 4;
    }
}

BallObject::~BallObject()
{
}
