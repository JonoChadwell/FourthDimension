#pragma once

#include <vector>
#include <glm/vec4.hpp>

class HyperShape;
class BallObject;
class Program;

class BallSimulation
{
public:
    BallSimulation(HyperShape *sphere);
    ~BallSimulation();

    void update(float dt);
    void addObject(glm::vec4 position, float radius, float mass);
    void render(Program *prog);

private:
    HyperShape *sphere;
    std::vector<BallObject*> objects;
};

