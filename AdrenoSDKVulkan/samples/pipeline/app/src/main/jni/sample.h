//                  Copyright (c) 2016 QUALCOMM Technologies Inc.
//                              All Rights Reserved.

#pragma once

#include "VkSampleframework.h"
#include <stdlib.h>

#define GLM_FORCE_CXX03
#define GLM_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "MeshObject.h"

#define NUM_PIPELINES 3

class VkSample : public VkSampleFramework
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
    void InitDescriptorSet();
    void InitUniformBuffers();
    void InitLayouts();
    void InitPipelines();
    void InitRenderPass();
    void InitFrameBuffers();
    void InitVertexBuffers();
    void BuildCmdBuffer();

    // Graphics Pipeline state
    VkDescriptorSetLayout       mDescriptorLayout;
    VkPipelineLayout            mPipelineLayout;
    VkRenderPass                mRenderPass;
    VkFramebuffer*              mFrameBuffers;
    VkPipeline                  mPipelines[NUM_PIPELINES];
    VkPipelineCache             mPipelineCache;

    VkDescriptorPool            mDescriptorPool;
    VkDescriptorSet             mDescriptorSet;

    // Vertices
    MeshObject                  mMesh;

    struct VertexUniform
    {
        glm::mat4 modelViewProjection;
        glm::mat4 model;
        glm::mat4 view;
        glm::vec3 camPos;
    };
    VertexUniform               mVertUniformData;
    BufferObject                mVertShaderUniformBuffer;
    glm::mat4                   mProjectionMatrix;

    static const char*          CACHE_FILE_NAME;
};