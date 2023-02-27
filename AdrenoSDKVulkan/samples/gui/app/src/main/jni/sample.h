//                  Copyright (c) 2016 QUALCOMM Technologies Inc.
//                              All Rights Reserved.
#pragma once

#include "VkSampleframework.h"
#include "Gui.h"
#include <stdlib.h>

#define GLM_FORCE_CXX03
#define GLM_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "MeshObject.h"

class VkSample: public VkSampleFramework
{
public:
    VkSample();
    virtual ~VkSample();

protected:

    // Overloaded VkSampleFramework entry points.
    bool InitSample();
    bool Update();
    bool Draw();
    bool DestroySample();
    void WindowResize(uint32_t newWidth, uint32_t newHeight);

    // Sample Initialization
    void InitRenderPass();
    void InitFrameBuffers();
    void BuildCmdBuffer();

    // Graphics Pipeline state
    VkRenderPass            mRenderPass;
    VkFramebuffer*          mFrameBuffers;

    // Fences
    VkFence                 mSubmitFences[MAX_SWAPCHAIN_COUNT];
    bool                    mFenceSubmitted[MAX_SWAPCHAIN_COUNT];

    // Gui
    Gui*                    mGui;
};
