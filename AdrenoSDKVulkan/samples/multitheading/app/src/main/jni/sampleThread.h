//                  Copyright (c) 2016 QUALCOMM Technologies Inc.
//                              All Rights Reserved.
#pragma once

#include "VkSampleframework.h"
#include <stdlib.h>

#define MAX_NUM_THREADS_PER_GROUP 16
#define MAX_NUM_THREAD_GROUPS 8
#define MAX_NUM_POSITIONS 64

// The vertex buffer bind id, used as a constant in various places in the sample
#define VERTEX_BUFFER_BIND_ID 0
#define NUM_VERTICES 36

class VkSample;
class Uniform;

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
    VkDeviceSize                          allocSize;
};


struct Uniform
{
    VkBuffer buf;
    VkDeviceMemory mem;
    VkDescriptorBufferInfo bufferInfo;
    VkDeviceSize allocSize;
};

struct ThreadData
{
    Mutex*                  pMutex;
    VkSample*               pSample;

    VkDevice                device;
    VkRenderPass            renderPass;

    VkPipeline              pipeline;
    VkDescriptorPool        descriptorPool;
    VkDescriptorSetLayout   descriptorLayout;
    VkPipelineLayout        pipelineLayout;
    VkDescriptorSet         descriptorSet;
    Vertices                vertices;
    Uniform*                pModelViewMatrixUniform;
    uint32_t                textureIdx;
    uint32_t                positionIdx;
    VkCommandPool           commandBufferPool;
    VkCommandBuffer         commandBuffer;

    uint32_t                group;
    uint32_t                index;
};

void* ThreadFunc(void* pData);

void InitLayouts(ThreadData* pThreadData);
void InitDescriptorSet(ThreadData* pThreadData);
void InitVertexBuffers(ThreadData* pThreadData);
void InitUniformBuffers(ThreadData* pThreadData);
void InitPipeline(ThreadData* pThreadData);
void InitCommandBuffer(ThreadData* pThreadData);

void UpdateVertexBuffer(ThreadData* pThreadData);
void UpdateDescriptorSet(ThreadData* pThreadData);
void UpdateCommandBuffer(ThreadData* pThreadData);