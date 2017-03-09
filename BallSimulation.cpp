#include "BallSimulation.h"
#include "BallObject.h"
#include "HyperShapes.h"
#include "Program.h"
#include "MatrixStack.h"
#include "Controls.h"

#include <random>
#include <iostream>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

#define BOUND 10.0f

// objects below this point will recieve quadratically less gravity to prevent jitter
#define GRAVITY_FALLOFF_HEIGHT 0.02f

// the amount of energy preseved in a collision
#define COEFFICIENT_OF_RESTITUTION 0.5f

// the amount of velocity preserved when bouncing off walls
#define BOUNCINESS 0.8f

// the velocity overlapping balls will apply to eachother
#define PUSH_OUT_FORCE 200.0f

#define FRICTION 0.08f

using namespace glm;
using namespace std;

namespace 
{
    void perform_collision(BallObject *b1, BallObject *b2, float dt)
    {
        b1->contacts++;
        b2->contacts++;

        vec4 collision_normal = normalize(b1->position - b2->position);

        float m1 = b1->mass;
        float m2 = b2->mass;

        float iv1 = dot(-collision_normal, b1->velocity);
        float iv2 = dot(collision_normal, b2->velocity);
        if (iv1 + iv2 > 0)
        {
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
    if (controls::clear)
    {
        controls::clear = false;
        for (int i = 0; i < objects.size(); i++)
        {
            BallObject *obj = objects[i];
            delete obj;
        }
        objects.clear();
    }

    if (controls::spawn)
    {
        addObject();
        controls::spawn = false;
    }

    for (int i = 0; i < objects.size(); i++)
    {
        BallObject *obj = objects[i];
        obj->contacts = 0;
    }

    // motion
    for (int i = 0; i < objects.size(); i++)
    {
        BallObject *obj = objects[i];
        obj->position += obj->velocity * dt;
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
        wall_collision_min(&(obj->position.y), &(obj->velocity.y), -BOUND, obj->radius);
        wall_collision_max(&(obj->position.y), &(obj->velocity.y), BOUND, obj->radius);

        if (controls::physicsDimensions > 1)
        {
            wall_collision_min(&(obj->position.x), &(obj->velocity.x), -BOUND, obj->radius);
            wall_collision_max(&(obj->position.x), &(obj->velocity.x), BOUND, obj->radius);
        }
        else
        {
            wall_collision_min(&(obj->position.x), &(obj->velocity.x), -obj->radius - 0.005f, obj->radius);
            wall_collision_max(&(obj->position.x), &(obj->velocity.x), obj->radius + 0.005f, obj->radius);
        }

        if (controls::physicsDimensions > 2)
        {
            wall_collision_min(&(obj->position.z), &(obj->velocity.z), -BOUND, obj->radius);
            wall_collision_max(&(obj->position.z), &(obj->velocity.z), BOUND, obj->radius);
        }
        else
        {
            wall_collision_min(&(obj->position.z), &(obj->velocity.z), -obj->radius - 0.005f, obj->radius);
            wall_collision_max(&(obj->position.z), &(obj->velocity.z), obj->radius + 0.005f, obj->radius);
        }

        if (controls::physicsDimensions > 3)
        {
            wall_collision_min(&(obj->position.w), &(obj->velocity.w), -BOUND, obj->radius);
            wall_collision_max(&(obj->position.w), &(obj->velocity.w), BOUND, obj->radius);
        }
        else
        {
            wall_collision_min(&(obj->position.w), &(obj->velocity.w), -obj->radius - 0.005f, obj->radius);
            wall_collision_max(&(obj->position.w), &(obj->velocity.w), obj->radius + 0.005f, obj->radius);
        }
    }

    applyGravity(dt);

    // apply friction
    for (int i = 0; i < objects.size(); i++)
    {
        BallObject *obj = objects[i];
        obj->velocity *= 1 - (FRICTION * dt);
    }
}

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> dis(0, 1);

void BallSimulation::addObject()
{
    cout << "Objects: " << objects.size() << endl;
    if (controls::uneven_sizes)
    {
        float radius = dis(gen) * dis(gen) * dis(gen) * 4 + 1;
        objects.push_back(new BallObject(vec4(dis(gen) * 16 - 8, dis(gen) * 8 + 0, dis(gen) * 16 - 8, dis(gen) * 16 - 8), radius, radius * radius * radius));
    }
    else
    {
        objects.push_back(new BallObject(vec4(dis(gen) * 16 - 8, dis(gen) * 8 + 0, dis(gen) * 16 - 8, dis(gen) * 16 - 8), 2, 8));
    }
}

void BallSimulation::addObject(vec4 position, float radius, float mass)
{
    objects.push_back(new BallObject(position, radius, mass));
}

void BallSimulation::render(Program *prog)
{
    if (controls::strange_color && prog->mode != RENDER_STRANGE_COLORED) return;
    if (!controls::strange_color && prog->mode != sphere->defaultRenderMode) return;

    MatrixStack *R = new MatrixStack();

    R->pushMatrix();
    R->loadIdentity();

    for (int i = 0; i < objects.size(); i++)
    {
        BallObject *obj = objects[i];
        R->pushMatrix();

        R->rotate4d(glfwGetTime() * obj->rotdir / 10.0, obj->rot1, obj->rot2);
        R->scale4d(obj->radius);

        glUniformMatrix4fv(prog->getUniform("R"), 1, GL_FALSE, value_ptr(R->topMatrix()));
        glUniform4f(prog->getUniform("objPos"), obj->position.x, obj->position.y, obj->position.z, obj->position.w);

        sphere->draw(prog);

        R->popMatrix();
    }


    R->popMatrix();

    delete R;
}

void BallSimulation::applyGravity(float dt)
{
    for (int i = 0; i < objects.size(); i++)
    {
        BallObject *obj = objects[i];
        float scalar = 1 - obj->contacts / 5.5f;
        if (dot(vec4(1, 0, 1, 1), controls::gravity) == 0)
        {
            float height;
            if (controls::gravity.y < 0) {
                height = obj->position.y - obj->radius + BOUND;
            }
            else
            {
                height = -obj->position.y - obj->radius + BOUND;
            }
            if (height < GRAVITY_FALLOFF_HEIGHT)
            {
                float falloff = height / GRAVITY_FALLOFF_HEIGHT;
                scalar = scalar * falloff * falloff;
            }
        }
        if (scalar < 0) continue;
        obj->velocity += controls::gravity * dt * scalar;
    }
}

