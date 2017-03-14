#pragma once
#ifdef VR_ENABLE

#include <openvr.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace vrs {
    void initialize();
    void updateHmdDevicePositions();
    glm::mat4 startVrEyeRender(vr::Hmd_Eye eye);
    void stopVrEyeRender();
    void finishVrFrame();
    void shutdown();
    glm::vec3 getPosition(glm::mat4 pose);
}
#endif
