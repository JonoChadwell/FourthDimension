#pragma once

#include <vector>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

class HyperShape;
class BallObject;
class Program;

class BallSimulation
{
public:
    BallSimulation();
    ~BallSimulation();

    void update(float dt);
    void addObject();
    void addObject(glm::vec4 position, float radius, float mass);
    void render(Program *prog, glm::mat4 hypercamera);

private:
    std::vector<BallObject*> objects;

    void applyGravity(float dt);
    void setControlledPositions(float dt);
};

