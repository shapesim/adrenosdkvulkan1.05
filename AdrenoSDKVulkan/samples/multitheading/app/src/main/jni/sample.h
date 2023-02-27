//                  Copyright (c) 2016 QUALCOMM Technologies Inc.
//                              All Rights Reserved.
#pragma once

#include "VkSampleframework.h"
#include <stdlib.h>

#define GLM_FORCE_CXX03
#define GLM_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "sampleThread.h"

#define NUM_TEXTURES 3


class VkSample : public VkSampleFramework
{
public:
    VkSample();
    ~VkSample();

    // Matrices
    glm::mat4               mProjectionMatrix;
    glm::mat4               mViewMatrix;

    TextureObject*          GetTextureObject(int32_t i){return mpTextureObjects[i];};
protected:
    // Overloaded VkSampleFramework entry points.
    bool InitSample();
    bool Update();
    bool Draw();
    bool DestroySample();
    void WindowResize(uint32_t newWidth, uint32_t newHeight);

    void InitRenderPass();

    void InitTextures();
    void InitThreading();

    void InitFrameBuffers();

    void InitUniformBuffers();

    void BuildCmdBuffer();
    void PrepareThreads();

    // Frame buffer
    VkFramebuffer*          mFrameBuffers;

    // RenderPass
    VkRenderPass            mRenderPass;

    // threading
    uint32_t                mNumThreadGroups;
    uint32_t                mNumThreadsPerGroup;
    ThreadData              mThreadData[MAX_NUM_THREAD_GROUPS][MAX_NUM_THREADS_PER_GROUP];
    Thread*                 mpThreads[MAX_NUM_THREAD_GROUPS][MAX_NUM_THREADS_PER_GROUP];

    int32_t                 mWorkingThreadGroup;
    int32_t                 mRenderThreadGroup;
    int32_t                 mFramesBetweenThreadRecreation;

    // Fences
    VkFence                 mSubmitFences[3];
    bool                    mFenceSubmitted[3];

    // Mutex
    Mutex                   mMutex;

    // Textures
    TextureObject*          mpTextureObjects[NUM_TEXTURES];

    // Uniform
    void                    UpdateUniforms();
    Uniform                 mModelViewMatrixUniform;
    float_t                 mSpinAngle;

};
