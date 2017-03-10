#pragma once

#include <vector>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

class HyperShape;
class Program;

struct hyper_orientation {
    float xy;
    float xz;
    float xw;
    float yz;
    float yw;
    float zw;
};

struct hyper_object {
    HyperShape *obj;
    vec4 pos;
    float scale;
    hyper_orientation rot;
};

class Scene
{
public:
    Scene();
    ~Scene();

    void update(float dt);
    void addObject(hyper_object);
    void addObject(HyperShape *obj, vec4 pos, float scale, hyper_orientation rot);
    void render(Program *prog, glm::mat4 hypercamera, float bound);

private:
    HyperShape *sphere;
    std::vector<BallObject*> objects;

    void applyGravity(float dt);
};

