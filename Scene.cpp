#include "Scene.h"
#include "HyperShapes.h"
#include "Controls.h"
#include "MatrixStack.h"
#include "Program.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;
using namespace std;

namespace {
    bool inBounds(vec4 p, float bound) {
        if (abs(p.x) > bound) {
            return false;
        }
        if (abs(p.y) > bound) {
            return false;
        }
        if (abs(p.z) > bound) {
            return false;
        }
        if (abs(p.w) > bound) {
            return false;
        }
        return true;
    }
}
        


Scene::Scene()
{
}

Scene::~Scene()
{
}

void Scene::addObject(hyper_object obj)
{
    objects.push_back(obj);
}

void Scene::addObject(HyperShape* model, vec4 pos, vec4 scale, hyper_orientation rot)
{
    addObject({ model, pos, scale, rot });
}

void Scene::render(Program* prog, glm::mat4 hypercamera, vec4 hypercameraPos, float bound)
{
    MatrixStack *R = new MatrixStack();

    R->pushMatrix();
    R->loadIdentity();

    float maxRadius = bound;

    for (int i = 0; i < objects.size(); i++)
    {
        hyper_object obj = objects[i];
        R->pushMatrix();
        R->loadIdentity();

        R->rotate4d(obj.rot.xy, 0, 1);
        R->rotate4d(obj.rot.xz, 0, 2);
        R->rotate4d(obj.rot.xw, 0, 3);
        R->rotate4d(obj.rot.yz, 1, 2);
        R->rotate4d(obj.rot.yw, 1, 3);
        R->rotate4d(obj.rot.zw, 2, 3);
        R->scale4d(obj.scale);

        glUniformMatrix4fv(prog->getUniform("R"), 1, GL_FALSE, value_ptr(R->topMatrix()));
        glUniform4f(prog->getUniform("objPos"), obj.pos.x, obj.pos.y, obj.pos.z, obj.pos.w);

        R->popMatrix();
    }

    delete R;
}
