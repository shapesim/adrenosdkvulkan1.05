//                  Copyright (c) 2016-2017 QUALCOMM Technologies Inc.
//                              All Rights Reserved.
#pragma once

#include <assert.h>
#include <stdlib.h>
#include <vector>
#include <vulkan/vulkan.h>
#include <android/asset_manager.h>

class VkSampleFramework;

 // Defines a simple object for creating and holding Vulkan buffer objects.
class BufferObject
{
public:
    BufferObject() : mSample(nullptr) {}
    virtual ~BufferObject();

    bool InitBuffer(VkSampleFramework* sample, VkDeviceSize size, VkBufferUsageFlags usageFlags,
                                  const void* initialData);

    void Destroy();

    VkBuffer& GetBuffer()
    {
        return mBuffer;
    }

    VkDeviceMemory& GetDeviceMemory()
    {
        return mMemory;
    }

    VkDeviceSize GetDeviceMemoryOffset()
    {
        return mMemOffset;
    }

    VkDescriptorBufferInfo GetDescriptorInfo()
    {
        return mDescriptorInfo;
    }

    VkDeviceSize GetAllocationSize()
    {
        return mAllocationSize;
    }
private:
    VkSampleFramework* mSample;
    VkBuffer mBuffer;
    VkDeviceMemory mMemory;
    VkDeviceSize mAllocationSize;
    VkDeviceSize mMemOffset;
    VkDescriptorBufferInfo mDescriptorInfo;
};

class VertexBufferObject: public BufferObject
{
public:
    VertexBufferObject();
    virtual ~VertexBufferObject();

    void AddBinding(uint32_t binding, uint32_t stride, VkVertexInputRate inputRate);
    void AddAttribute(uint32_t binding, uint32_t location, uint32_t offset, VkFormat format);
    VkPipelineVertexInputStateCreateInfo CreatePipelineState();
protected:
    std::vector<VkVertexInputBindingDescription> mBindings;
    std::vector<VkVertexInputAttributeDescription> m_attributes;
};
