#pragma once

#include <openvr.h>
#include <glm/mat4x4.hpp>

namespace vrs {
    void initialize();
    void updateHmdDevicePositions();
    glm::mat4 startVrEyeRender(vr::Hmd_Eye eye);
    void stopVrEyeRender();
    void finishVrFrame();
}
