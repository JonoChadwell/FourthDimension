#include "BallSimulation.h"
#include "BallObject.h"
#include "HyperShapes.h"
#include "Program.h"
#include "MatrixStack.h"
#include "Controls.h"

#include <cmath>
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

    vec4 convertToHyperspace(vec3 realPos)
    {
        MatrixStack *Q = new MatrixStack();
        Q->pushMatrix();
        Q->loadIdentity();
        Q->rotate4d(-controls::r5, 2, 3);
        Q->rotate4d(-controls::r4, 1, 3);
        Q->rotate4d(-controls::r3, 0, 3);
        Q->rotate4d(-controls::r2, 0, 2);
        Q->rotate4d(-controls::r1, 0, 1);
        mat4 top = Q->topMatrix();
        return top * vec4(realPos.x, realPos.y, realPos.z, controls::slice_offset);
    }

    vec3 getPosition(mat4 pose)
    {
        return vec3(
            pose[3][0],
            pose[3][1],
            pose[3][2]
        );
    }
}

BallSimulation::BallSimulation()
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

        if (controls::physics_dimensions > 1)
        {
            wall_collision_min(&(obj->position.x), &(obj->velocity.x), -BOUND, obj->radius);
            wall_collision_max(&(obj->position.x), &(obj->velocity.x), BOUND, obj->radius);
        }
        else
        {
            wall_collision_min(&(obj->position.x), &(obj->velocity.x), -obj->radius - 0.005f, obj->radius);
            wall_collision_max(&(obj->position.x), &(obj->velocity.x), obj->radius + 0.005f, obj->radius);
        }

        if (controls::physics_dimensions > 2)
        {
            wall_collision_min(&(obj->position.z), &(obj->velocity.z), -BOUND, obj->radius);
            wall_collision_max(&(obj->position.z), &(obj->velocity.z), BOUND, obj->radius);
        }
        else
        {
            wall_collision_min(&(obj->position.z), &(obj->velocity.z), -obj->radius - 0.005f, obj->radius);
            wall_collision_max(&(obj->position.z), &(obj->velocity.z), obj->radius + 0.005f, obj->radius);
        }

        if (controls::physics_dimensions > 3)
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

    // set positions of player controlled objects
    setControlledPositions();
}

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> dis(0, 1);

void BallSimulation::addObject()
{
    cout << "Objects: " << objects.size() << endl;
    switch (controls::size_mode) {
    case 0:
        objects.push_back(new BallObject(vec4(dis(gen) * 16 - 8, dis(gen) * 8 + 0, dis(gen) * 16 - 8, dis(gen) * 16 - 8), 2, 8));
        break;
    case 1:
        objects.push_back(new BallObject(vec4(dis(gen) * 16 - 8, dis(gen) * 8 + 0, dis(gen) * 16 - 8, dis(gen) * 16 - 8), 4.08, 8));
        break;
    default:
        float radius = dis(gen) * dis(gen) * dis(gen) * dis(gen) * 4 + 1;
        objects.push_back(new BallObject(vec4(dis(gen) * 16 - 8, dis(gen) * 8 + 0, dis(gen) * 16 - 8, dis(gen) * 16 - 8), radius, radius * radius * radius * radius));
    }
}

void BallSimulation::addObject(vec4 position, float radius, float mass)
{
    objects.push_back(new BallObject(position, radius, mass));
}

void BallSimulation::render(Program *prog, mat4 hypercamera)
{
    if (controls::strange_color && prog->mode != RENDER_STRANGE_COLORED) return;
    if (!controls::strange_color && prog->mode != RENDER_QUADS_WIREFRAME) return;

    MatrixStack *R = new MatrixStack();

    R->pushMatrix();
    R->loadIdentity();

    for (int i = 0; i < objects.size(); i++)
    {
        BallObject *obj = objects[i];

        // just a little vfc
        if (controls::slice)
        {
            float w = (hypercamera * obj->position).w;

            float width = 0.13f + obj->radius + 0.01f;
            if (abs(w + controls::slice_offset) > width)
            {
                continue;
            }
        }

        R->pushMatrix();

        R->rotate4d(glfwGetTime() * obj->rotdir / 10.0, obj->rot1, obj->rot2);
        R->scale4d(obj->radius);

        glUniformMatrix4fv(prog->getUniform("R"), 1, GL_FALSE, value_ptr(R->topMatrix()));
        glUniform4f(prog->getUniform("objPos"), obj->position.x, obj->position.y, obj->position.z, obj->position.w);

        switch (controls::sphere_quality) {
        case 1:
            HyperShapes::hyper_sphere_lq->draw(prog);
            break;
        case 2:
            HyperShapes::hyper_sphere_hq->draw(prog);
            break;
        default:
            HyperShapes::hyper_sphere->draw(prog);
        }
        

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

void BallSimulation::setControlledPositions()
{
    for (int i = 0; i < controls::num_controllers && i < objects.size(); i++)
    {
        objects[i]->position = convertToHyperspace(getPosition(controls::controller_positions[i])) * 10.0f - vec4(0, 15, 0, 0);
        objects[i]->radius = 0.8;
    }
}
