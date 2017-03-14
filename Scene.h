#pragma once

#include <vector>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

using namespace glm;
using namespace std;

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
    HyperShape *model;
    vec4 pos;
    vec4 scale;
    hyper_orientation rot;
};

class Scene
{
public:
    Scene();
    ~Scene();

    void addObject(hyper_object obj);
    void addObject(HyperShape *model, vec4 pos, vec4 scale, hyper_orientation rot);
    void render(Program *prog, glm::mat4 hypercamera, vec4 hypercameraPos, float bound);

private:
    HyperShape *sphere;
    vector<hyper_object> objects;

};

