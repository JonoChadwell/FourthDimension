#include "BallSimulation.h"
#include "Shape.h"
#include "HyperShapes.h"
#include "Program.h"
#include "MatrixStack.h"
#include "Controls.h"
#include "RenderModes.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>
#include <iostream>

// OpenGL
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GLFW/glfw3.h>

// GLM
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

// VR
//#define VR_ENABLE true
#ifdef VR_ENABLE
#include "VrSubsystem.h"
#include <openvr.h>
#endif

#define PLAYER_SPEED 5

#define MAX_TIME_STEP 0.03f
#define MAX_PHYSICS_STEPS_PER_FRAME 2

using namespace glm;
using namespace std;

GLFWwindow *window;

int window_width = 800;
int window_height = 600;

BallSimulation *sim;

// Program* point_prog;

vec3 eye = vec3(0, 5, -10);

float lastTime = 0;

static void error_callback(int error, const char *description)
{
    cerr << description << endl;
}

static void resize_callback(GLFWwindow *window, int width, int height) {
    window_width = width;
    window_height = height;
    glViewport(0, 0, width, height);
}

static void print(string name, vec4 vec) {
    cout << name << ": " << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << endl;
}

static void init()
{
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    HyperShapes::initialize();

    rm::init();

    sim = new BallSimulation();

    for (int i = 0; i < 3; i++) {
        sim->addObject();
    }

#ifdef VR_ENABLE
    vrs::initialize();
#endif
}

static vec3 forwards()
{
    float x = cos(controls::phi) * sin(controls::theta);
    float y = sin(controls::phi);
    float z = cos(controls::phi) * cos(controls::theta);
    return vec3(x, y, z);
}

static void update()
{
    float time = glfwGetTime();
    float delta = time - lastTime;
    lastTime = time;

    vec3 up = vec3(0, 1, 0);
    vec3 z = normalize(forwards());
    vec3 x = normalize(cross(up, z));
    vec3 y = normalize(cross(z, x));

    eye += x * (controls::vel.x * delta * PLAYER_SPEED);
    eye += y * (controls::vel.y * delta * PLAYER_SPEED);
    eye += z * (controls::vel.z * delta * PLAYER_SPEED);

    if (!controls::paused) {
        int steps = 0;
        while (delta > MAX_TIME_STEP)
        {
            sim->update(MAX_TIME_STEP);
            delta -= MAX_TIME_STEP;
            if (++steps >= MAX_PHYSICS_STEPS_PER_FRAME)
            {
                break;
            }
        }
        if (steps < MAX_PHYSICS_STEPS_PER_FRAME)
        {
            sim->update(delta);
        }
    }

#ifdef VR_ENABLE
    vrs::updateHmdDevicePositions();
#endif
}

static void render4dScene(Program *prog, mat4 hypercamera)
{
    glUniform1f(prog->getUniform("sliceOffset"), controls::slice_offset);
    if (controls::slice)
    {
        glUniform1f(prog->getUniform("sliceWidth"), 0.13f);
    }
    else
    {
        glUniform1f(prog->getUniform("sliceWidth"), 20.0f);
    }

    sim->render(prog, hypercamera);

    if (prog->mode == HyperShapes::hyper_cube->defaultRenderMode)
    {
        MatrixStack *R = new MatrixStack();

        R->pushMatrix();
        R->loadIdentity();

        R->scale4d(10.0f);

        glUniformMatrix4fv(prog->getUniform("R"), 1, GL_FALSE, value_ptr(R->topMatrix()));
        glUniform1f(prog->getUniform("sliceWidth"), 1000.0f);

        if (controls::bound_cube)
        {
            glUniform4f(prog->getUniform("objPos"), 0, 0, 0, -10);
            HyperShapes::cube->draw(prog);
            glUniform4f(prog->getUniform("objPos"), 0, 0, 0, 10);
            HyperShapes::cube->draw(prog);
        }
        else
        {
            glUniform4f(prog->getUniform("objPos"), 0, 0, 0, 0);
            HyperShapes::hyper_cube->draw(prog);
        }

        R->popMatrix();

        delete R;
    }
}

static void render4d(Program *prog, float aspect, mat4 PV)
{
    prog->bind();

    if (controls::project_strange)
    {
        glUniform1i(prog->getUniform("divW"), 1);
    }
    else
    {
        glUniform1i(prog->getUniform("divW"), 0);
    }

    
    MatrixStack *M = new MatrixStack();
    MatrixStack *Q = new MatrixStack();

    glUniformMatrix4fv(prog->getUniform("PV"), 1, GL_FALSE, value_ptr(PV));

    // 4d->3d camera 3d spatial geometry result transforms
    M->pushMatrix();
    M->loadIdentity();
    M->scale(0.1f);
    M->translate(vec3(0, 15, 0));
    glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));

    Q->pushMatrix();
    Q->loadIdentity();

    // 4d->3d camera projection transforms

    // 'Normal'
    Q->rotate4d(controls::r1, 0, 1);
    Q->rotate4d(controls::r2, 0, 2);
    Q->rotate4d(controls::r3, 0, 3);
    Q->rotate4d(controls::r4, 1, 3);
    Q->rotate4d(controls::r5, 2, 3);

    glUniformMatrix4fv(prog->getUniform("Q"), 1, GL_FALSE, value_ptr(Q->topMatrix()));

    // Sets the viewable "depth" into the 4d result component of the model transforms
    // (the "Front" and "Back" planes of the 4d->3d camera's projected hypervolume
    //glUniform1f(prog->getUniform("sliceWidth"), 0.2f);

    // Renders the 4d geometry through the projection cameras
    render4dScene(prog, Q->topMatrix());

    Q->popMatrix();
    M->popMatrix();

    prog->unbind();
    
    delete M;
    delete Q;
}

static void render()
{
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float aspect = (float)width / (float)height;

    MatrixStack *PV = new MatrixStack();
    PV->pushMatrix();
    PV->loadIdentity();
    PV->perspective(M_PI / 3, aspect, 0.01f, 100.0f);

    PV->pushMatrix();

    PV->lookAt(eye, eye + forwards(), vec3(0, 1, 0));

    render4d(rm::getProgram(RENDER_TRIS_WIREFRAME), aspect, PV->topMatrix());
    render4d(rm::getProgram(RENDER_QUADS_WIREFRAME), aspect, PV->topMatrix());
    render4d(rm::getProgram(RENDER_STRANGE_COLORED), aspect, PV->topMatrix());

#ifdef VR_ENABLE
    mat4 eye = vrs::startVrEyeRender(vr::Eye_Left);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    render4d(rm::getProgram(RENDER_TRIS_WIREFRAME), aspect, eye);
    render4d(rm::getProgram(RENDER_QUADS_WIREFRAME), aspect, eye);
    render4d(rm::getProgram(RENDER_STRANGE_COLORED), aspect, eye);
    vrs::stopVrEyeRender();
    eye = vrs::startVrEyeRender(vr::Eye_Right);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    render4d(rm::getProgram(RENDER_TRIS_WIREFRAME), aspect, eye);
    render4d(rm::getProgram(RENDER_QUADS_WIREFRAME), aspect, eye);
    render4d(rm::getProgram(RENDER_STRANGE_COLORED), aspect, eye);
    vrs::stopVrEyeRender();
    vrs::finishVrFrame();
#endif

    PV->popMatrix();
    PV->popMatrix();
    delete PV;
}

int main(int argc, char **argv)
{
    // Set error callback.
    glfwSetErrorCallback(error_callback);
    // Initialize the library.
    if (!glfwInit()) {
        return -1;
    }
    //request the highest possible version of OGL - important for mac
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

    // Create a windowed mode window and its OpenGL context.
    window = glfwCreateWindow(window_width, window_height, "Fourth Dimension", NULL, NULL);
    if (!window) {
        glfwTerminate();
        cerr << "Couldn't create opengl window" << endl;
        getchar();
        return EXIT_FAILURE;
    }

    // Make the window's context current.
    glfwMakeContextCurrent(window);

    // Initialize GLEW.
    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        cerr << "Failed to initialize GLEW" << endl;
        getchar();
        return EXIT_FAILURE;
    }

    //weird bootstrap of glGetError
    glGetError();
    cout << "OpenGL version: " << glGetString(GL_VERSION) << endl;
    cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

    // Set vsync.
#ifdef VR_ENABLE
    glfwSwapInterval(0);
#else
    glfwSwapInterval(1);
#endif
    // Set keyboard callback.
    glfwSetKeyCallback(window, controls::key_callback);
    //set the mouse call back
    glfwSetMouseButtonCallback(window, controls::mouse_callback);
    //set the scroll call back
    glfwSetCursorPosCallback(window, controls::cursor_callback);
    //set the window resize call back
    glfwSetFramebufferSizeCallback(window, resize_callback);
    //lock cursor
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Initialize scene. Note geometry initialized in init now
    init();

    lastTime = glfwGetTime();

    // Loop until the user closes the window.
    while (!glfwWindowShouldClose(window)) {
        // Update game state
        update();
        // Render scene.
        render();
        // Swap front and back buffers.
        glfwSwapBuffers(window);
        // Poll for and process events.
        glfwPollEvents();
    }

#ifdef VR_ENABLE
    vrs::shutdown();
#endif

    // Quit program.
    glfwDestroyWindow(window);
    glfwTerminate();

    getchar();
    return EXIT_SUCCESS;
}
