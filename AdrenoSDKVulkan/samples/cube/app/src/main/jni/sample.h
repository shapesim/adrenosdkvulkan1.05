//                  Copyright (c) 2016 QUALCOMM Technologies Inc.
//                              All Rights Reserved.
#pragma once

#include "VkSampleframework.h"
#include <stdlib.h>

#define GLM_FORCE_CXX03
#define GLM_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Vertices
{

    Vertices() : vi_bindings(nullptr), vi_attrs(nullptr)
    {
    }

    ~Vertices()
    {
        if (!vi_bindings)
        {
            return;
        }
        delete [] vi_bindings;
        delete [] vi_attrs;
    }

    VkBuffer buf;
    VkDeviceMemory mem;
    VkPipelineVertexInputStateCreateInfo  vi;
    VkVertexInputBindingDescription*      vi_bindings;
    VkVertexInputAttributeDescription*    vi_attrs;
};

struct Uniform
{
    VkBuffer buf;
    VkDeviceMemory mem;
    VkDescriptorBufferInfo bufferInfo;
    VkDeviceSize allocSize;
};

class VkSample : public VkSampleFramework
{
public:
    VkSample();
    ~VkSample();

protected:
    // Overloaded VkSampleFramework entry points.
    bool InitSample();
    bool Update();
    bool Draw();
    bool DestroySample();
    void WindowResize(uint32_t newWidth, uint32_t newHeight);

    void InitVertexBuffers();
    void InitUniformBuffers();
    void InitLayouts();
    void InitRenderPass();
    void InitPipelines();
    void InitFrameBuffers();
    void InitDescriptorSet();
    void InitTextures();
    void BuildCmdBuffer();
    void UpdateUniforms();

    VkFramebuffer*          mFrameBuffers;
    Vertices                mVertices;

    // Pipeline
    VkPipelineLayout        mPipelineLayout;
    VkPipeline              mPipeline;

    // Render Pass
    VkRenderPass            mRenderPass;

    // Descriptor pool and set
    VkDescriptorPool        mDescriptorPool;
    VkDescriptorSet         mDescriptorSet;
    VkDescriptorSetLayout   mDescriptorLayout;

    //Uniforms
    Uniform                 mModelViewMatrixUniform;

    //Textures
    TextureObject           mTexBricks;

    // Matrices
    glm::mat4               mProjectionMatrix;
    glm::mat4               mViewMatrix;
    glm::mat4               mModelMatrix;
};
