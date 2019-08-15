#include "Controls.h"
#include "MatrixStack.h"
#define _USE_MATH_DEFINES
#include <math.h>
#define ROT_AMT M_PI/200

#define GRAVITY -32.2f

#include <vector>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

using namespace std;
using namespace glm;

namespace controls {
    glm::vec3 vel = glm::vec3(0, 0, 0);
    bool mouse_captured = false;
    double lastX;
    double lastY;
    bool mouse_pos_initialized = false;
    float theta = 0;
    float phi = -M_PI / 6;
    bool strange_color = false;
    int sphere_quality = 0;

    float r1;
    float r2;
    float r3;
    float r4;
    float r5;

    // Scene Exploration values
    vec4 cameraPosition;
    float cameraSize;

    // Physics Simulation values
    bool spawn = false;
    bool slice = false;
    bool paused = false;
    float slice_offset;
    int size_mode = 0;
    int physics_dimensions = 4;
    bool clear = false;
    bool bound_cube = false;
    bool project_strange = false;
    bool project_gravity = false;
    vec4 gravity = vec4(0, GRAVITY, 0, 0);

    // VR control values
    int num_controllers = 0;
    vector<mat4> controller_positions = {};
    glm::mat4 hmd_position = mat4(0);

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
            slice_offset += 0.05;
        }
        if (key == GLFW_KEY_F && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            slice_offset -= 0.05;
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
            project_gravity = false;
            gravity = -gravity;
            printf("Gravity reversed\n");
        }
        if (key == GLFW_KEY_2 && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            spawn = true;
        }
        if (key == GLFW_KEY_3 && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            slice = !slice;
            if (slice) {
                printf("Entered 'render 3d slice' mode\n");
            } else {
                printf("Left 'render 3d slice' mode\n");
            }
        }
        if (key == GLFW_KEY_P && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            paused = !paused;
            if (paused) {
                printf("Simulation Paused\n");
            } else {
                printf("Simulation Unpaused\n");
            }
        }
        if (key == GLFW_KEY_4 && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            strange_color = !strange_color;
            if (strange_color) {
                printf("Using 4th dimension to set ball color\n");
            }
        }
        if (key == GLFW_KEY_5 && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            project_strange = !project_strange;
            if (project_strange) {
                printf("Projecting 4th dimension spatially\n");
            }
        }
        if (key == GLFW_KEY_6 && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            bound_cube = !bound_cube;
        }
        if (key == GLFW_KEY_7 && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            sphere_quality++;
            if (sphere_quality > 2)
            {
                sphere_quality = 0;
            }
            printf("Adjusting hypersphere quality\n");
        }
        if (key == GLFW_KEY_9 && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            physics_dimensions++;
            if (physics_dimensions > 4)
            {
                physics_dimensions = 4;
            }
            printf("Increasing physics dimensions\n");
        }
        if (key == GLFW_KEY_8 && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            physics_dimensions--;
            if (physics_dimensions < 1)
            {
                physics_dimensions = 1;
            }
            printf("Reducing physics dimensions\n");
        }
        if (key == GLFW_KEY_0 && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            size_mode++;
            if (size_mode > 2) {
                size_mode = 0;
            }
            if (size_mode == 0) {
                printf("Now spawning 'small' balls\n");
            } else if (size_mode == 1) {
                printf("Now spawning 'big' balls\n");
            } else if (size_mode == 2) {
                printf("Now spawning 'random size' balls\n");
            }
        }
        if (key == GLFW_KEY_X && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            clear = true;
            printf("Cleared balls\n");
        }
        if (key == GLFW_KEY_Z && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            project_gravity = !project_gravity;
            if (project_gravity) {
                printf("Gravity now following visual down\n");
            } else {
                printf("Gravity no longer following visual down\n");
            }
        }
        if (project_gravity)
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
            gravity = top * vec4(0, 1, 0, 0) * GRAVITY;
        }
        if (key == GLFW_KEY_C && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            project_gravity = false;
            gravity = vec4(0, GRAVITY, 0, 0);
            printf("Gravity reset\n");
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
