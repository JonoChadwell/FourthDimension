#include "Controls.h"
#define _USE_MATH_DEFINES
#include <math.h>
#define ROT_AMT M_PI/200

#include <glm/vec3.hpp>
#include <GLFW/glfw3.h>

namespace controls {
    glm::vec3 vel = glm::vec3(0, 0, 0);
    bool mouse_captured = false;
    double lastX;
    double lastY;
    bool mouse_pos_initialized = false;
    float theta = 0;
    float phi = -M_PI / 6;
    bool spawn = false;
    bool slice = false;
    bool paused = false;
    bool strange_color = false;
    float slice_offset;
    bool uneven_sizes = false;

    float r1;
    float r2;
    float r3;
    float r4;
    float r5;

    float gravity = -9.8f;

    void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
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
        if (key == GLFW_KEY_R && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            slice_offset += 0.1;
        }
        if (key == GLFW_KEY_F && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            slice_offset -= 0.1;
        }
        if (key == GLFW_KEY_V && action == GLFW_PRESS)
        {
            slice_offset = 0;
        }
        if (key == GLFW_KEY_T && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            r1 += ROT_AMT;
        }
        if (key == GLFW_KEY_G && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            r1 -= ROT_AMT;
        }
        if (key == GLFW_KEY_B && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            r1 = 0;
        }
        if (key == GLFW_KEY_Y && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            r2 += ROT_AMT;
        }
        if (key == GLFW_KEY_H && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            r2 -= ROT_AMT;
        }
        if (key == GLFW_KEY_N && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            r2 = 0;
        }
        if (key == GLFW_KEY_U && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            r3 += ROT_AMT;
        }
        if (key == GLFW_KEY_J && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            r3 -= ROT_AMT;
        }
        if (key == GLFW_KEY_M && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            r3 = 0;
        }
        if (key == GLFW_KEY_I && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            r4 += ROT_AMT;
        }
        if (key == GLFW_KEY_K && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            r4 -= ROT_AMT;
        }
        if (key == GLFW_KEY_COMMA && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            r4 = 0;
        }
        if (key == GLFW_KEY_O && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            r5 += ROT_AMT;
        }
        if (key == GLFW_KEY_L && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            r5 -= ROT_AMT;
        }
        if (key == GLFW_KEY_PERIOD && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            r5 = 0;
        }
        if (key == GLFW_KEY_SLASH && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            r1 = 0;
            r2 = 0;
            r3 = 0;
            r4 = 0;
            r5 = 0;
        }
        if (key == GLFW_KEY_1 && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            gravity = -gravity;
        }
        if (key == GLFW_KEY_2 && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            spawn = true;
        }
        if (key == GLFW_KEY_3 && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            slice = !slice;
        }
        if (key == GLFW_KEY_P && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            paused = !paused;
        }
        if (key == GLFW_KEY_4 && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            strange_color = !strange_color;
        }
        if (key == GLFW_KEY_0 && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            uneven_sizes = !uneven_sizes;
        }
    }

    void mouse_callback(GLFWwindow *window, int button, int action, int mods)
    {
        double posX, posY;
        if (action == GLFW_PRESS) {
            if (!mouse_captured) {
                mouse_captured = true;
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            }
        }
    }

    void cursor_callback(GLFWwindow *window, double x, double y)
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