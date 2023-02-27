//                  Copyright (c) 2016-2017 QUALCOMM Technologies Inc.
//                              All Rights Reserved.
#pragma once

#include "../common/imgui/imgui.h"
#include <stdlib.h>

// This definition enables the Android extensions
#define VK_USE_PLATFORM_ANDROID_KHR
#include <vulkan/vulkan.h>
#include "TextureObject.h"

#define MAX_SWAPCHAIN_COUNT 3
#define NUM_INITIAL_GUI_VERTICES 1000
#define NUM_INITIAL_GUI_INDICES 1000

struct GuiVertices
{
    GuiVertices() : vi_bindings(nullptr), vi_attrs(nullptr)
    {
    }

    ~GuiVertices()
    {
        if (!vi_bindings)
        {
            return;
        }
        delete [] vi_bindings;
        delete [] vi_attrs;
    }

    VkBuffer                                buf;
    VkDeviceMemory                          mem;
    VkPipelineVertexInputStateCreateInfo    vi;
    VkVertexInputBindingDescription*        vi_bindings;
    VkVertexInputAttributeDescription*      vi_attrs;
    VkDeviceSize                            allocSize;
};

struct GuiIndices
{
    GuiIndices()
    {
    }
    ~GuiIndices()
    {
    }

    VkBuffer                                buf;
    VkDeviceMemory                          mem;
    VkDeviceSize                            allocSize;
};

struct GuiUniform
{
    VkBuffer                buf;
    VkDeviceMemory          mem;
    VkDescriptorBufferInfo  bufferInfo;
    VkDeviceSize            allocSize;
};

struct GuiUniformData
{
    float scale[2];
    float translate[2];
};


const uint shader_gui_frag[]= {
        // Overload400-PrecQual.1442 26-Aug-2016
        0x07230203,0x00010000,0x00080001,0x00000018,0x00000000,0x00020011,0x00000001,0x0006000b,
        0x00000001,0x4c534c47,0x6474732e,0x3035342e,0x00000000,0x0003000e,0x00000000,0x00000001,
        0x0008000f,0x00000004,0x00000004,0x6e69616d,0x00000000,0x00000009,0x0000000b,0x00000014,
        0x00030010,0x00000004,0x00000007,0x00030003,0x00000002,0x00000190,0x00090004,0x415f4c47,
        0x735f4252,0x72617065,0x5f657461,0x64616873,0x6f5f7265,0x63656a62,0x00007374,0x00090004,
        0x415f4c47,0x735f4252,0x69646168,0x6c5f676e,0x75676e61,0x5f656761,0x70303234,0x006b6361,
        0x00040005,0x00000004,0x6e69616d,0x00000000,0x00050005,0x00000009,0x61724675,0x6c6f4367,
        0x0000726f,0x00040005,0x0000000b,0x6f6c6f63,0x00000072,0x00030005,0x00000010,0x00786574,
        0x00030005,0x00000014,0x00007675,0x00040047,0x00000009,0x0000001e,0x00000000,0x00040047,
        0x0000000b,0x0000001e,0x00000000,0x00040047,0x00000010,0x00000022,0x00000000,0x00040047,
        0x00000010,0x00000021,0x00000001,0x00040047,0x00000014,0x0000001e,0x00000001,0x00020013,
        0x00000002,0x00030021,0x00000003,0x00000002,0x00030016,0x00000006,0x00000020,0x00040017,
        0x00000007,0x00000006,0x00000004,0x00040020,0x00000008,0x00000003,0x00000007,0x0004003b,
        0x00000008,0x00000009,0x00000003,0x00040020,0x0000000a,0x00000001,0x00000007,0x0004003b,
        0x0000000a,0x0000000b,0x00000001,0x00090019,0x0000000d,0x00000006,0x00000001,0x00000000,
        0x00000000,0x00000000,0x00000001,0x00000000,0x0003001b,0x0000000e,0x0000000d,0x00040020,
        0x0000000f,0x00000000,0x0000000e,0x0004003b,0x0000000f,0x00000010,0x00000000,0x00040017,
        0x00000012,0x00000006,0x00000002,0x00040020,0x00000013,0x00000001,0x00000012,0x0004003b,
        0x00000013,0x00000014,0x00000001,0x00050036,0x00000002,0x00000004,0x00000000,0x00000003,
        0x000200f8,0x00000005,0x0004003d,0x00000007,0x0000000c,0x0000000b,0x0004003d,0x0000000e,
        0x00000011,0x00000010,0x0004003d,0x00000012,0x00000015,0x00000014,0x00050057,0x00000007,
        0x00000016,0x00000011,0x00000015,0x00050085,0x00000007,0x00000017,0x0000000c,0x00000016,
        0x0003003e,0x00000009,0x00000017,0x000100fd,0x00010038
};
const int shader_gui_frag_size=724;

const uint shader_gui_vert[]= {
        // Overload400-PrecQual.1442 26-Aug-2016
        0x07230203,0x00010000,0x00080001,0x0000002b,0x00000000,0x00020011,0x00000001,0x0006000b,
        0x00000001,0x4c534c47,0x6474732e,0x3035342e,0x00000000,0x0003000e,0x00000000,0x00000001,
        0x000b000f,0x00000000,0x00000004,0x6e69616d,0x00000000,0x00000009,0x0000000b,0x0000000f,
        0x00000011,0x00000015,0x00000018,0x00030003,0x00000002,0x00000190,0x00090004,0x415f4c47,
        0x735f4252,0x72617065,0x5f657461,0x64616873,0x6f5f7265,0x63656a62,0x00007374,0x00090004,
        0x415f4c47,0x735f4252,0x69646168,0x6c5f676e,0x75676e61,0x5f656761,0x70303234,0x006b6361,
        0x00040005,0x00000004,0x6e69616d,0x00000000,0x00030005,0x00000009,0x00007675,0x00040005,
        0x0000000b,0x74726576,0x00005655,0x00040005,0x0000000f,0x6f6c6f63,0x00000072,0x00050005,
        0x00000011,0x74726576,0x6f6c6f43,0x00000072,0x00060005,0x00000013,0x505f6c67,0x65567265,
        0x78657472,0x00000000,0x00060006,0x00000013,0x00000000,0x505f6c67,0x7469736f,0x006e6f69,
        0x00030005,0x00000015,0x00000000,0x00030005,0x00000018,0x00736f70,0x00050005,0x0000001a,
        0x74726556,0x42557865,0x0000004f,0x00050006,0x0000001a,0x00000000,0x6c616373,0x00000065,
        0x00060006,0x0000001a,0x00000001,0x6e617274,0x74616c73,0x00000065,0x00050005,0x0000001c,
        0x74726576,0x42557865,0x0000004f,0x00040047,0x00000009,0x0000001e,0x00000001,0x00040047,
        0x0000000b,0x0000001e,0x00000001,0x00040047,0x0000000f,0x0000001e,0x00000000,0x00040047,
        0x00000011,0x0000001e,0x00000002,0x00050048,0x00000013,0x00000000,0x0000000b,0x00000000,
        0x00030047,0x00000013,0x00000002,0x00040047,0x00000018,0x0000001e,0x00000000,0x00050048,
        0x0000001a,0x00000000,0x00000023,0x00000000,0x00050048,0x0000001a,0x00000001,0x00000023,
        0x00000008,0x00030047,0x0000001a,0x00000002,0x00040047,0x0000001c,0x00000022,0x00000000,
        0x00040047,0x0000001c,0x00000021,0x00000000,0x00020013,0x00000002,0x00030021,0x00000003,
        0x00000002,0x00030016,0x00000006,0x00000020,0x00040017,0x00000007,0x00000006,0x00000002,
        0x00040020,0x00000008,0x00000003,0x00000007,0x0004003b,0x00000008,0x00000009,0x00000003,
        0x00040020,0x0000000a,0x00000001,0x00000007,0x0004003b,0x0000000a,0x0000000b,0x00000001,
        0x00040017,0x0000000d,0x00000006,0x00000004,0x00040020,0x0000000e,0x00000003,0x0000000d,
        0x0004003b,0x0000000e,0x0000000f,0x00000003,0x00040020,0x00000010,0x00000001,0x0000000d,
        0x0004003b,0x00000010,0x00000011,0x00000001,0x0003001e,0x00000013,0x0000000d,0x00040020,
        0x00000014,0x00000003,0x00000013,0x0004003b,0x00000014,0x00000015,0x00000003,0x00040015,
        0x00000016,0x00000020,0x00000001,0x0004002b,0x00000016,0x00000017,0x00000000,0x0004003b,
        0x0000000a,0x00000018,0x00000001,0x0004001e,0x0000001a,0x00000007,0x00000007,0x00040020,
        0x0000001b,0x00000002,0x0000001a,0x0004003b,0x0000001b,0x0000001c,0x00000002,0x00040020,
        0x0000001d,0x00000002,0x00000007,0x0004002b,0x00000016,0x00000021,0x00000001,0x0004002b,
        0x00000006,0x00000025,0x00000000,0x0004002b,0x00000006,0x00000026,0x3f800000,0x00050036,
        0x00000002,0x00000004,0x00000000,0x00000003,0x000200f8,0x00000005,0x0004003d,0x00000007,
        0x0000000c,0x0000000b,0x0003003e,0x00000009,0x0000000c,0x0004003d,0x0000000d,0x00000012,
        0x00000011,0x0003003e,0x0000000f,0x00000012,0x0004003d,0x00000007,0x00000019,0x00000018,
        0x00050041,0x0000001d,0x0000001e,0x0000001c,0x00000017,0x0004003d,0x00000007,0x0000001f,
        0x0000001e,0x00050085,0x00000007,0x00000020,0x00000019,0x0000001f,0x00050041,0x0000001d,
        0x00000022,0x0000001c,0x00000021,0x0004003d,0x00000007,0x00000023,0x00000022,0x00050081,
        0x00000007,0x00000024,0x00000020,0x00000023,0x00050051,0x00000006,0x00000027,0x00000024,
        0x00000000,0x00050051,0x00000006,0x00000028,0x00000024,0x00000001,0x00070050,0x0000000d,
        0x00000029,0x00000027,0x00000028,0x00000025,0x00000026,0x00050041,0x0000000e,0x0000002a,
        0x00000015,0x00000017,0x0003003e,0x0000002a,0x00000029,0x000100fd,0x00010038

};
const int shader_gui_vert_size=1340;


// The vertex buffer bind id, used as a constant in various places in the sample
#define VERTEX_BUFFER_BIND_ID 0

class VkSampleFramework;
class Gui
{
public:
    Gui(VkSampleFramework* sample);
    ~Gui();

    void Initialize(VkRenderPass renderPass);
    void Update();
    void Draw(VkCommandBuffer cmdBuffer);
    void Destroy();

private:

    void InitGuiVertexBuffer(uint32_t bufferID, size_t size);
    void InitGuiIndexBuffer(uint32_t bufferID, size_t size);
    void InitUniforms();
    void InitTextures();
    void InitLayouts();
    void InitPipelines();
    void InitDescriptorSets();

    VkSampleFramework* mSample;
    VkRenderPass mRenderPass;
    VkDevice mDevice;

    GuiVertices             mGuiVertices[MAX_SWAPCHAIN_COUNT];
    GuiIndices              mGuiIndices[MAX_SWAPCHAIN_COUNT];
    GuiUniform              mGuiUniform;
    TextureObject*          mpGuiTextureObject;
    VkDescriptorSetLayout   mGuiDescriptorLayout;
    VkDescriptorPool        mGuiDescriptorPool;
    VkDescriptorSet         mGuiDescriptorSet;
    VkPipelineLayout        mGuiPipelineLayout;
    VkPipeline              mGuiPipeline;
};
