#include "MatrixStack.h"
#include "VrSubsystem.h"
#include "Controls.h"

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

// OpenVR
#include <openvr.h>

using namespace std;
using namespace glm;
using namespace vr;

namespace vrs {

    IVRSystem *vr_system;

    struct FramebufferDesc
    {
        GLuint m_nDepthBufferId;
        GLuint m_nRenderTextureId;
        GLuint m_nRenderFramebufferId;
        GLuint m_nResolveTextureId;
        GLuint m_nResolveFramebufferId;
    };

    FramebufferDesc activeEyeDesc;
    FramebufferDesc leftEyeDesc;
    FramebufferDesc rightEyeDesc;

    vr::TrackedDevicePose_t devicePose[vr::k_unMaxTrackedDeviceCount];
    vr::TrackedDeviceClass deviceClass[vr::k_unMaxTrackedDeviceCount];
    mat4 deviceMat[vr::k_unMaxTrackedDeviceCount];
    bool deviceVisible[vr::k_unMaxTrackedDeviceCount];

    uint32_t m_nRenderWidth;
    uint32_t m_nRenderHeight;

    int m_iTrackedControllerCount;
    int m_iTrackedControllerCount_Last;
    int m_iValidPoseCount;
    int m_iValidPoseCount_Last;
    string m_strPoseClasses;

    mat4 hmd_pose;
    mat4 left_eye_pose;
    mat4 right_eye_pose;

    mat4 center_projection;
    mat4 left_eye_projection;
    mat4 right_eye_projection;

    mat4 convertMatrix(vr::HmdMatrix44_t mat) {
        return mat4(
            mat.m[0][0], mat.m[1][0], mat.m[2][0], mat.m[3][0],
            mat.m[0][1], mat.m[1][1], mat.m[2][1], mat.m[3][1],
            mat.m[0][2], mat.m[1][2], mat.m[2][2], mat.m[3][2],
            mat.m[0][3], mat.m[1][3], mat.m[2][3], mat.m[3][3]
        );
    }

    mat4 convertMatrix(vr::HmdMatrix34_t mat) {
        return mat4(
            mat.m[0][0], mat.m[1][0], mat.m[2][0], 0.0,
            mat.m[0][1], mat.m[1][1], mat.m[2][1], 0.0,
            mat.m[0][2], mat.m[1][2], mat.m[2][2], 0.0,
            mat.m[0][3], mat.m[1][3], mat.m[2][3], 1.0f
        );
    }

    mat4 getCurrentMvpMatrix(vr::Hmd_Eye nEye)
    {
        mat4 matMVP;
        if (nEye == vr::Eye_Left)
        {
            matMVP = left_eye_projection * left_eye_pose * hmd_pose;
        }
        else if (nEye == vr::Eye_Right)
        {
            matMVP = right_eye_projection * right_eye_pose * hmd_pose;
        }

        return matMVP;
    }

    mat4 startVrEyeRender(vr::Hmd_Eye eye)
    {
        if (eye == vr::Eye_Left) {
            activeEyeDesc = leftEyeDesc;
        }
        else if (eye == vr::Eye_Right) {
            activeEyeDesc = rightEyeDesc;
        } 
        else {
            cerr << "Too many eyes\n";
            getchar();
            exit(EXIT_FAILURE);
        }
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glEnable(GL_MULTISAMPLE);

        glEnable(GL_MULTISAMPLE);

        glBindFramebuffer(GL_FRAMEBUFFER, activeEyeDesc.m_nRenderFramebufferId);
        glViewport(0, 0, m_nRenderWidth, m_nRenderHeight);

        return getCurrentMvpMatrix(eye);
    }

    void stopVrEyeRender()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glDisable(GL_MULTISAMPLE);

        glBindFramebuffer(GL_READ_FRAMEBUFFER, activeEyeDesc.m_nRenderFramebufferId);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, activeEyeDesc.m_nResolveFramebufferId);

        glBlitFramebuffer(0, 0, m_nRenderWidth, m_nRenderHeight, 0, 0, m_nRenderWidth, m_nRenderHeight,
            GL_COLOR_BUFFER_BIT,
            GL_LINEAR);

        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    }

    void finishVrFrame()
    {
        vr::Texture_t leftEyeTexture = { (void*)(uintptr_t)leftEyeDesc.m_nResolveTextureId, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
        vr::VRCompositor()->Submit(vr::Eye_Left, &leftEyeTexture);
        vr::Texture_t rightEyeTexture = { (void*)(uintptr_t)rightEyeDesc.m_nResolveTextureId, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
        vr::VRCompositor()->Submit(vr::Eye_Right, &rightEyeTexture);
    }

    void createFrameBuffer(int nWidth, int nHeight, FramebufferDesc &framebufferDesc)
    {
        glGenFramebuffers(1, &framebufferDesc.m_nRenderFramebufferId);
        glBindFramebuffer(GL_FRAMEBUFFER, framebufferDesc.m_nRenderFramebufferId);

        glGenRenderbuffers(1, &framebufferDesc.m_nDepthBufferId);
        glBindRenderbuffer(GL_RENDERBUFFER, framebufferDesc.m_nDepthBufferId);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT, nWidth, nHeight);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, framebufferDesc.m_nDepthBufferId);

        glGenTextures(1, &framebufferDesc.m_nRenderTextureId);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, framebufferDesc.m_nRenderTextureId);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA8, nWidth, nHeight, true);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, framebufferDesc.m_nRenderTextureId, 0);

        glGenFramebuffers(1, &framebufferDesc.m_nResolveFramebufferId);
        glBindFramebuffer(GL_FRAMEBUFFER, framebufferDesc.m_nResolveFramebufferId);

        glGenTextures(1, &framebufferDesc.m_nResolveTextureId);
        glBindTexture(GL_TEXTURE_2D, framebufferDesc.m_nResolveTextureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, nWidth, nHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferDesc.m_nResolveTextureId, 0);

        // check FBO status
        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE)
        {
            cerr << "Failed to create frame buffers\n";
            exit(EXIT_FAILURE);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void initialize()
    {
        EVRInitError eError = vr::VRInitError_None;
        vr_system = VR_Init(&eError, vr::VRApplication_Scene);

        if (eError != vr::VRInitError_None)
        {
            cerr << "Failed to initialize HMD\n";
            getchar();
            exit(EXIT_FAILURE);
        }

        vr_system->GetRecommendedRenderTargetSize(&m_nRenderWidth, &m_nRenderHeight);

        createFrameBuffer(m_nRenderWidth, m_nRenderHeight, leftEyeDesc);
        createFrameBuffer(m_nRenderWidth, m_nRenderHeight, rightEyeDesc);

        left_eye_projection = convertMatrix(vr_system->GetProjectionMatrix(vr::Eye_Left, 0.1f, 30.0f));
        right_eye_projection = convertMatrix(vr_system->GetProjectionMatrix(vr::Eye_Right, 0.1f, 30.0f));

        left_eye_pose = inverse(convertMatrix(vr_system->GetEyeToHeadTransform(vr::Eye_Left)));
        right_eye_pose = inverse(convertMatrix(vr_system->GetEyeToHeadTransform(vr::Eye_Right)));

    }

    void updateHmdDevicePositions()
    {
        vr::VRCompositor()->WaitGetPoses(devicePose, vr::k_unMaxTrackedDeviceCount, NULL, 0);

        m_iValidPoseCount = 0;
        for (int nDevice = 0; nDevice < vr::k_unMaxTrackedDeviceCount; ++nDevice)
        {
            if (devicePose[nDevice].bPoseIsValid)
            {
                m_iValidPoseCount++;
                deviceMat[nDevice] = convertMatrix(devicePose[nDevice].mDeviceToAbsoluteTracking);
                deviceClass[nDevice] = vr_system->GetTrackedDeviceClass(nDevice);
            }
        }

        if (devicePose[vr::k_unTrackedDeviceIndex_Hmd].bPoseIsValid)
        {
            hmd_pose = inverse(deviceMat[vr::k_unTrackedDeviceIndex_Hmd]);
        }
    }

    void shutdown()
    {
        vr::VR_Shutdown();
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_FALSE);
        glDebugMessageCallback(nullptr, nullptr);
        
        glDeleteRenderbuffers(1, &leftEyeDesc.m_nDepthBufferId);
        glDeleteTextures(1, &leftEyeDesc.m_nRenderTextureId);
        glDeleteFramebuffers(1, &leftEyeDesc.m_nRenderFramebufferId);
        glDeleteTextures(1, &leftEyeDesc.m_nResolveTextureId);
        glDeleteFramebuffers(1, &leftEyeDesc.m_nResolveFramebufferId);
        
        glDeleteRenderbuffers(1, &rightEyeDesc.m_nDepthBufferId);
        glDeleteTextures(1, &rightEyeDesc.m_nRenderTextureId);
        glDeleteFramebuffers(1, &rightEyeDesc.m_nRenderFramebufferId);
        glDeleteTextures(1, &rightEyeDesc.m_nResolveTextureId);
        glDeleteFramebuffers(1, &rightEyeDesc.m_nResolveFramebufferId);
    }
}
