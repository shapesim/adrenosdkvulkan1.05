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

    void InitMeshLayouts();
    void InitMeshDescriptorSet();
    void InitMeshPipelines();

    void InitComputeCmdBuffer();
    void InitComputeLayouts();
    void InitComputeDescriptorSet();
    void InitComputePipeline();
    void BuildComputeCmdBuffer();

    // Graphics Pipeline state
    VkDescriptorSetLayout   mDescriptorLayout;
    VkPipelineLayout        mPipelineLayout;
    VkRenderPass            mRenderPass;
    VkFramebuffer*          mFrameBuffers;
    VkPipeline              mPipeline;
    VkDescriptorPool        mDescriptorPool;
    // we have4 'flags' to draw, at different location
    VkDescriptorSet         mDescriptorSet[4];

    // Mesh Rendering State
    VkDescriptorSetLayout   mMeshDescriptorLayout;
    VkPipelineLayout        mMeshPipelineLayout;
    VkPipeline              mMeshPipeline;
    VkDescriptorPool        mMeshDescriptorPool;
    VkDescriptorSet         mMeshDescriptorSet;

    // Compute pipeline state
    VkDescriptorSetLayout   mComputeDescLayout;
    VkPipelineLayout        mComputePipelineLayout;
    VkPipeline              mComputePipeline;
    VkDescriptorSet         mComputeDescriptorSet;
    VkDescriptorPool        mComputeDescriptorPool;
    VkCommandBuffer         mComputeCmdBuffer;
    VkCommandPool           mComputeCommandPool;

    // Vertices
    VertexBufferObject      mQuadVertices;

    // Mesh
    MeshObject              mMesh;

    struct vertex_layout
    {
        glm::vec4 pos;
        glm::vec4 color;
        glm::vec4 uv;
    };

    VertexBufferObject      mSurfaceVertices;
    BufferObject            mSurfaceIndices;
    uint32_t                mSurfaceIndexCount;

    // Textures
    TextureObject           mTexVkLogo;
    TextureObject           mTex;

    // Uniform Data
    struct VertexUniformData
    {
        glm::mat4 mvp;
    };
    VertexUniformData       mVertexUniform[4];
    BufferObject            mVertexUniformBuffer[4];

    // Used in mesh rendering vertex shader
    struct MeshVertexUniform
    {
        glm::mat4 modelViewProjection;
        glm::mat4 model;
        glm::mat4 view;
        glm::vec3 camPos;
    };
    MeshVertexUniform       mMeshVertUniformData;
    BufferObject            mMeshVertexUniformBuffer;

    glm::mat4               mModelViewMatrix;
    glm::mat4               mProjectionMatrix;

    struct ComputeBufferData
    {
        float time;
    };
    BufferObject            mStorageBuffer;
    ComputeBufferData       mStorageData;
};