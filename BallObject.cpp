#include "BallObject.h"
#include <math.h>
#include <cstdlib>

BallObject::BallObject(glm::vec4 position, float radius, float mass) :
    position(position),
    radius(radius),
    mass(mass),
    velocity(glm::vec4(0, 0, 0, 0)),
    contacts(0)
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
