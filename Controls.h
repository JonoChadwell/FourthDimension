#pragma once

#define _USE_MATH_DEFINES
#include <math.h>
#define ROT_AMT M_PI/200

#include <vector>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

// OpenGL
#include <GL/glew.h>
#include <GL/freeglut.h>
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
    extern bool strange_color;
    extern int sphere_quality;
    extern float slice_offset;
    extern int size_mode;
    extern int physics_dimensions;
    extern bool clear;
    extern bool bound_cube;
    extern bool project_strange;

    extern float r1;
    extern float r2;
    extern float r3;
    extern float r4;
    extern float r5;

    extern glm::vec4 gravity;

    extern int num_controllers;
    extern std::vector<glm::mat4> controller_positions;
    extern glm::mat4 hmd_position;

    void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
    void mouse_callback(GLFWwindow *window, int button, int action, int mods);
    void cursor_callback(GLFWwindow *window, double x, double y);
}
