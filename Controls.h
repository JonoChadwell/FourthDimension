#pragma once

#define _USE_MATH_DEFINES
#include <math.h>
#define ROT_AMT M_PI/200

#include <glm/vec3.hpp>
#include <GLFW/glfw3.h>

namespace controls {
    glm::vec3 vel = vec3(0, 0, 0);
    bool mouse_captured = false;
    double lastX;
    double lastY;
    bool mouse_pos_initialized = false;
    float theta = 0;
    float phi = -M_PI / 6;
    
    float r1;
    float r2;
    float r3;
    float r4;
    float r5;

    static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
    {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
        if ((key == GLFW_KEY_LEFT_CONTROL || key == GLFW_KEY_RIGHT_CONTROL) && action == GLFW_PRESS) {
            mouse_captured = false;
            mouse_pos_initialized = false;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        if (key == GLFW_KEY_W)
        {
            if (action == GLFW_PRESS)
            {
                vel.z = 1;
            }
            else if (action == GLFW_RELEASE)
            {
                vel.z = 0;
            }
        }
        if (key == GLFW_KEY_S)
        {
            if (action == GLFW_PRESS)
            {
                vel.z = -1;
            }
            else if (action == GLFW_RELEASE)
            {
                vel.z = 0;
            }
        }
        if (key == GLFW_KEY_A)
        {
            if (action == GLFW_PRESS)
            {
                vel.x = 1;
            }
            else if (action == GLFW_RELEASE)
            {
                vel.x = 0;
            }
        }
        if (key == GLFW_KEY_D)
        {
            if (action == GLFW_PRESS)
            {
                vel.x = -1;
            }
            else if (action == GLFW_RELEASE)
            {
                vel.x = 0;
            }
        }
        if (key == GLFW_KEY_SPACE)
        {
            if (action == GLFW_PRESS)
            {
                vel.y = 1;
            }
            else if (action == GLFW_RELEASE)
            {
                vel.y = 0;
            }
        }
        if (key == GLFW_KEY_LEFT_SHIFT)
        {
            if (action == GLFW_PRESS)
            {
                vel.y = -1;
            }
            else if (action == GLFW_RELEASE)
            {
                vel.y = 0;
            }
        }
        if (key == GLFW_KEY_T && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            r1 += ROT_AMT;
        }
        if (key == GLFW_KEY_G && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            r1 -= ROT_AMT;
        }
        if (key == GLFW_KEY_Y && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            r2 += ROT_AMT;
        }
        if (key == GLFW_KEY_H && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            r2 -= ROT_AMT;
        }
        if (key == GLFW_KEY_U && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            r3 += ROT_AMT;
        }
        if (key == GLFW_KEY_J && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            r3 -= ROT_AMT;
        }
        if (key == GLFW_KEY_I && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            r4 += ROT_AMT;
        }
        if (key == GLFW_KEY_K && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            r4 -= ROT_AMT;
        }
        if (key == GLFW_KEY_O && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            r5 += ROT_AMT;
        }
        if (key == GLFW_KEY_L && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            r5 -= ROT_AMT;
        }
    }

    static void mouse_callback(GLFWwindow *window, int button, int action, int mods)
    {
        double posX, posY;
        if (action == GLFW_PRESS) {
            if (!mouse_captured) {
                mouse_captured = true;
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            }
        }
    }

    static void cursor_callback(GLFWwindow *window, double x, double y)
    {
        if (mouse_captured) {
            if (mouse_pos_initialized) {
                theta -= (x - lastX) / 1000.0;
                phi -= (y - lastY) / 1000.0;
            }
            if (phi < -M_PI / 2 + 0.1) {
                phi = -M_PI / 2 + 0.1;
            }
            if (phi > M_PI / 2 - 0.1) {
                phi = M_PI / 2 - 0.1;
            }
            lastX = x;
            lastY = y;
            mouse_pos_initialized = true;
        }
    }
}
