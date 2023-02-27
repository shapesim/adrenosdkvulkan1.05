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
    void InitDescriptorSet();
    void InitLayouts();
    void InitPipelines();
    void InitRenderPass();
    void InitFrameBuffers();
    void InitVertexBuffers();
    void InitUniformBuffers();
    void InitTextures();
    void BuildCmdBuffer();

    void InitComputeCmdBuffer();
    void InitComputeLayouts();
    void InitComputeDescriptorSet();
    void InitComputePipeline();
    void BuildComputeCmdBuffer();


    // Graphics Pipeline state
    VkDescriptorSetLayout mDescriptorLayout;
    VkPipelineLayout mPipelineLayout;
    VkRenderPass mRenderPass;
    VkFramebuffer*       mFrameBuffers;
    VkPipeline mPipeline;
    VkDescriptorPool mDescriptorPool;
    VkDescriptorSet mDescriptorSet;

    // Compute pipeline state
    VkDescriptorSetLayout  mComputeDescLayout;
    VkPipelineLayout        mComputePipelineLayout;
    VkPipeline              mComputePipeline;
    VkDescriptorSet         mComputeDescriptorSet;
    VkDescriptorPool        mComputeDescriptorPool;

    // Compute command buffer
    VkCommandPool           mComputeCommandPool;
    VkCommandBuffer         mComputeCmdBuffer;

    // Vertices
    VertexBufferObject m_quadVertices;


    struct vertex_layout {
        glm::vec4 pos;
        glm::vec4 color;
        glm::vec4 uv;
    };

    VertexBufferObject m_surfaceVertices;
    BufferObject       m_surfaceIndices;
    uint32_t           m_surfaceIndexCount;

    // Textures
    TextureObject m_texVkLogo;

    // Uniform Data
    struct VertexUniformData
    {
        glm::mat4 mvp;
    };
    VertexUniformData m_vertexUniform;
    BufferObject m_vertexUniformBuffer;

    glm::mat4 m_modelViewMatrix;
    glm::mat4 m_projectionMatrix;

    struct ComputeBufferData
    {
        float time;
    };
    BufferObject      m_storageBuffer;
    ComputeBufferData m_storageData;

};
