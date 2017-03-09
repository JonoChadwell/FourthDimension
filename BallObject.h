#pragma once

#include <glm/vec4.hpp>

class BallObject
{
public:
    BallObject(glm::vec4 position, float radius, float mass);
    ~BallObject();

    float radius;
    float mass;
    glm::vec4 position;
    glm::vec4 velocity;
};
