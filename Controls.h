#pragma once

#define _USE_MATH_DEFINES
#include <math.h>
#define ROT_AMT M_PI/200

#include <glm/vec3.hpp>
#include <GLFW/glfw3.h>

namespace controls {
    extern glm::vec3 vel;
    extern bool mouse_captured;
    extern double lastX;
    extern double lastY;
    extern bool mouse_pos_initialized;
    extern float theta;
    extern float phi;
    extern bool spawn;
    extern bool slice;
    extern bool paused;
    
    extern float r1;
    extern float r2;
    extern float r3;
    extern float r4;
    extern float r5;

    extern float gravity;

    void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
    void mouse_callback(GLFWwindow *window, int button, int action, int mods);
    void cursor_callback(GLFWwindow *window, double x, double y);
}
