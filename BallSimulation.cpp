#include "BallSimulation.h"
#include "BallObject.h"
#include "HyperShapes.h"
#include "Program.h"
#include "MatrixStack.h"
#include <iostream>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GRAVITY -9.8f

// the amount of energy preseved in a collision
#define COEFFICIENT_OF_RESTITUTION 0.5f

// the amount of velocity preserved when bouncing off walls
#define BOUNCINESS 0.8f

// the velocity overlapping balls will apply to eachother
#define PUSH_OUT_FORCE 100.0f

#define FRICTION 0.01f

using namespace glm;
using namespace std;

namespace 
{
    void perform_collision(BallObject *b1, BallObject *b2, float dt)
    {
        vec4 collision_normal = normalize(b1->position - b2->position);

        float m1 = b1->mass;
        float m2 = b2->mass;

        float iv1 = dot(-collision_normal, b1->velocity);
        float iv2 = dot(collision_normal, b2->velocity);
        if (iv1 + iv2 > 0)
        {
            cout << "COLLIDING!\n";
            //// formula lifted from wikipedia
            float fv1 = (COEFFICIENT_OF_RESTITUTION * m2 * (iv2 + iv1) + m2 * iv2 - m1 * iv1) / (m1 + m2);
            float fv2 = (COEFFICIENT_OF_RESTITUTION * m1 * (iv1 + iv2) + m1 * iv1 - m2 * iv2) / (m1 + m2);

            //// zero out old collision oriented velocity
            b1->velocity += collision_normal * iv1;
            b2->velocity += -collision_normal * iv2;

            //// apply new collision oriented velocity
            b1->velocity += collision_normal * fv1;
            b2->velocity += -collision_normal * fv2;
        }

        // apply push out force
        float overlap = (b1->radius + b2->radius) - distance(b1->position, b2->position);
        if (overlap > 0) {
            b1->velocity += collision_normal * dt * overlap * PUSH_OUT_FORCE * m2 / (m1 + m2);
            b2->velocity += -collision_normal * dt * overlap * PUSH_OUT_FORCE * m1 / (m1 + m2);
        }
    }

    void wall_collision_min(float *position, float *velocity, float wall_pos, float radius)
    {
        if (*position - radius < wall_pos) {
            if (*velocity < 0) {
                *velocity = -BOUNCINESS * *velocity;
            }
            *position = wall_pos + radius;
        }
    }

    void wall_collision_max(float *position, float *velocity, float wall_pos, float radius)
    {
        if (*position + radius > wall_pos) {
            if (*velocity > 0) {
                *velocity = -BOUNCINESS * *velocity;
            }
            *position = wall_pos - radius;
        }
    }
}

BallSimulation::BallSimulation(HyperShape *sphere) :
    sphere(sphere)
{
    objects = vector<BallObject*>();
}


BallSimulation::~BallSimulation()
{
    for (int i = 0; i < objects.size(); i++)
    {
        BallObject *obj = objects[i];
        delete obj;
    }
    objects.clear();
}

void BallSimulation::update(float dt)
{
    // motion
    for (int i = 0; i < objects.size(); i++)
    {
        BallObject *obj = objects[i];
        obj->position += obj->velocity * dt;
    }

    // gravity
    for (int i = 0; i < objects.size(); i++)
    {
        BallObject *obj = objects[i];
        obj->velocity.y += GRAVITY * dt;
    }

    // collide with other spheres
    for (int i = 0; i < objects.size(); i++)
    {
        BallObject *b1 = objects[i];
        for (int j = i + 1; j < objects.size(); j++)
        {
            BallObject *b2 = objects[j];

            if (distance(b1->position, b2->position) < b1->radius + b2->radius) {
                perform_collision(b1, b2, dt);
            }
        }
    }

    // collide with walls
    for (int i = 0; i < objects.size(); i++)
    {
        BallObject *obj = objects[i];
        wall_collision_min(&(obj->position.y), &(obj->velocity.y), 0, obj->radius);

        wall_collision_min(&(obj->position.x), &(obj->velocity.x), -10, obj->radius);
        wall_collision_max(&(obj->position.x), &(obj->velocity.x), 10, obj->radius);

        wall_collision_min(&(obj->position.z), &(obj->velocity.z), -10, obj->radius);
        wall_collision_max(&(obj->position.z), &(obj->velocity.z), 10, obj->radius);

        wall_collision_min(&(obj->position.w), &(obj->velocity.w), -10, obj->radius);
        wall_collision_max(&(obj->position.w), &(obj->velocity.w), 10, obj->radius);
    }

    // apply friction
    for (int i = 0; i < objects.size(); i++)
    {
        BallObject *obj = objects[i];
        obj->velocity *= 1 - (FRICTION * dt);
    }

    /*for (int i = 0; i < objects.size(); i++)
    {
        BallObject *obj = objects[i];
        cout << "Position (" << i << "): " << obj->position.x << ", " << obj->position.y << ", " << obj->position.z << ", " << obj->position.w << endl;
    }*/
}

void BallSimulation::addObject(vec4 position, float radius, float mass)
{
    objects.push_back(new BallObject(position, radius, mass));
}

void BallSimulation::render(Program *prog)
{

    MatrixStack *R = new MatrixStack();

    R->pushMatrix();
    R->loadIdentity();

    for (int i = 0; i < objects.size(); i++)
    {
        BallObject *obj = objects[i];
        R->pushMatrix();

        R->scale4d(obj->radius);

        glUniformMatrix4fv(prog->getUniform("R"), 1, GL_FALSE, value_ptr(R->topMatrix()));
        glUniform4f(prog->getUniform("objPos"), obj->position.x, obj->position.y, obj->position.z, obj->position.w);

        sphere->draw(prog);

        R->popMatrix();
    }


    R->popMatrix();

    delete R;
}
