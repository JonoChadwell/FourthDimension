#pragma once

#include <glm/vec4.hpp>

class BallObject
{
public:
    BallObject(glm::vec4 position, float radius, float mass);
    ~BallObject();

    float radius;
    float mass;
    int rot1;
    int rot2;
    int contacts;
    glm::vec4 position;
    glm::vec4 velocity;
};
