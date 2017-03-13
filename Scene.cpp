#include "Scene.h"

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

void Scene::addObject(HyperShape* obj, vec4 pos, float scale, hyper_orientation rot)
{
    addObject({ obj, pos, scale, rot });
}

void Scene::render(Program* prog, glm::mat4 hypercamera, float bound)
{

}
