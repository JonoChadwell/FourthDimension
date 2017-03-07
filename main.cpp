
#include "Shape.h"
#include "HyperShapes.h"
#include "Program.h"
#include "MatrixStack.h"
#include "Controls.h"

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

#define PLAYER_SPEED 5

#define X_VOLUME 0
#define Y_VOLUME 1
#define Z_VOLUME 2
#define W_VOLUME 3

#define PI 3.1416

using namespace glm;
using namespace std;

GLFWwindow *window;

int window_width = 800;
int window_height = 600;

HyperShape* hyper_cube;
Program* prog;
Program* point_prog;

vec3 eye = vec3(0, 0, -10);

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

    hyper_cube = new HyperCube();
    hyper_cube->init();

    prog = new Program();
    prog->setVerbose(true);
    prog->setShaderNames("geom_vertex_shader.glsl", "geom_fragment_shader.glsl");
    prog->init();

    prog->addUniform("P");
    prog->addUniform("V");
    prog->addUniform("M");
    prog->addUniform("Q");
    prog->addUniform("objPos");
    prog->addAttribute("vertPos");
    prog->addAttribute("vertNor");
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
    vec3 y = up;
    vec3 z = forwards();
    vec3 x = cross(y, z);
    eye += x * (controls::vel.x * delta * PLAYER_SPEED);
    eye += y * (controls::vel.y * delta * PLAYER_SPEED);
    eye += z * (controls::vel.z * delta * PLAYER_SPEED);
}

static void render()
{
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float t = glfwGetTime();
    float aspect = (float)width / (float)height;

    prog->bind();

    MatrixStack *P = new MatrixStack();
    MatrixStack *V = new MatrixStack();
    MatrixStack *M = new MatrixStack();
    MatrixStack *Q = new MatrixStack();

    P->pushMatrix();
    P->loadIdentity();
    P->perspective(PI / 3, aspect, 0.01f, 100.0f);

    glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));

    V->pushMatrix();
    V->loadIdentity();
    
    V->lookAt(eye, eye + forwards(), vec3(0, 1, 0));

    glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(V->topMatrix()));

    M->pushMatrix();
    M->loadIdentity();

    glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));

    Q->pushMatrix();
    Q->loadIdentity();

    Q->rotate4d(t / 3, 1, 3);
    Q->rotate4d(t / 4, 2, 3);
    Q->rotate4d(t / 5, 0, 2);

    glUniformMatrix4fv(prog->getUniform("Q"), 1, GL_FALSE, value_ptr(Q->topMatrix()));
    glUniform4f(prog->getUniform("objPos"), 0.0f, 0.0f, 0.0f, 0.0f);
    hyper_cube->draw(prog);

    /*
    vec4 fourthDimPos = vec4(1.0f);
    print("FDP", fourthDimPos);
    vec4 projectedPos = Q->topMatrix() * fourthDimPos;
    print("PP", projectedPos);
    vec4 fragWorld = M->topMatrix() * vec4(projectedPos.x, projectedPos.y, projectedPos.z, 1.0f);
    print("FW", fragWorld);
    vec4 position = P->topMatrix() * V->topMatrix() * fragWorld;
    print("finalPosition", position);
    */

    Q->popMatrix();
    M->popMatrix();

    prog->unbind();
    delete P;
    delete V;
    delete M;
    delete Q;
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
    window = glfwCreateWindow(window_width, window_height, "Forth Dimension", NULL, NULL);
    if (!window) {
        glfwTerminate();
        cerr << "Couldn't create opengl window" << endl;
        return EXIT_FAILURE;
    }

    // Make the window's context current.
    glfwMakeContextCurrent(window);

    // Initialize GLEW.
    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        cerr << "Failed to initialize GLEW" << endl;
        return EXIT_FAILURE;
    }

    //weird bootstrap of glGetError
    glGetError();
    cout << "OpenGL version: " << glGetString(GL_VERSION) << endl;
    cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

    // Set vsync.
    glfwSwapInterval(1);
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

    // Quit program.
    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}