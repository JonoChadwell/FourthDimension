#pragma once

#include <glm/vec3.hpp>
#include <GLFW/glfw3.h>

namespace controls {
    glm::vec3 vel = vec3(0, 0, 0);
    bool mouse_captured = false;
    double lastX;
    double lastY;
    bool mouse_pos_initialized = false;
    float theta;
    float phi;


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
            lastX = x;
            lastY = y;
            mouse_pos_initialized = true;
        }
    }
}
