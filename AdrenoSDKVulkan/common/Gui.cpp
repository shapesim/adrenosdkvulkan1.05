//                  Copyright (c) 2016-2017 QUALCOMM Technologies Inc.
//                              All Rights Reserved.

#include "Gui.h"
#include "VkSampleFramework.h"

///////////////////////////////////////////////////////////////////////////////

Gui::Gui(VkSampleFramework* sample)
{
    mSample = sample;
}

///////////////////////////////////////////////////////////////////////////////

Gui::~Gui()
{

}

///////////////////////////////////////////////////////////////////////////////

void Gui::Initialize(VkRenderPass renderPass)
{
    mRenderPass = renderPass;
    mDevice = mSample->GetDevice();

    // Initialize ImGui
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize.x        = mSample->GetWidth();
    io.DisplaySize.y        = mSample->GetHeight();
    io.IniFilename          = "imgui.ini";
    io.RenderDrawListsFn    = NULL;

    // Create the vertex and index buffers to render the Gui
    for (uint32_t i = 0; i < mSample->GetSwapChainCount(); i++)
    {
        memset(&mGuiVertices[i],0,sizeof(GuiVertices));
        memset(&mGuiIndices[i], 0,sizeof(GuiIndices));
        InitGuiVertexBuffer(i, NUM_INITIAL_GUI_VERTICES * sizeof(ImDrawVert));
        InitGuiIndexBuffer( i, NUM_INITIAL_GUI_INDICES  * sizeof(ImDrawIdx));
    }

    InitUniforms();
    InitTextures();
    InitLayouts();
    InitPipelines();
    InitDescriptorSets();
}

///////////////////////////////////////////////////////////////////////////////

void Gui::Update()
{
    uint32_t swapIndex = mSample->GetSwapChainIndex();

    ImDrawData *pDrawData = ImGui::GetDrawData();
    if (pDrawData)
    {
        // Recreate the Vertex Buffer if its not big enough
        size_t vertexBufferSize = pDrawData->TotalVtxCount * sizeof(ImDrawVert);
        if (mGuiVertices[swapIndex].allocSize < vertexBufferSize)
        {
            vkDeviceWaitIdle(mDevice);
            InitGuiVertexBuffer(swapIndex, vertexBufferSize);
        }

        // Recreate the Index Buffer if its not big enough
        size_t indexBufferSize = pDrawData->TotalIdxCount * sizeof(ImDrawIdx);
        if (mGuiIndices[swapIndex].allocSize < indexBufferSize)
        {
            vkDeviceWaitIdle(mDevice);
            InitGuiIndexBuffer(swapIndex, indexBufferSize);
        }

        // Upload the Vertex Data
        VkResult ret = VK_SUCCESS;
        ImDrawVert *pVertices;
        ret = vkMapMemory(mDevice, mGuiVertices[swapIndex].mem, 0, vertexBufferSize, 0, (void **) (&pVertices));
        assert(!ret);

        ImDrawIdx *pIndices;
        ret = vkMapMemory(mDevice, mGuiIndices[swapIndex].mem, 0, indexBufferSize, 0, (void **) (&pIndices));
        assert(!ret);

        // For each command list
        for (uint32_t cl = 0; cl < pDrawData->CmdListsCount; cl++)
        {
            // Copy the vertices
            ImDrawList *pDrawList = pDrawData->CmdLists[cl];
            memcpy(pVertices, pDrawList->VtxBuffer.Data, pDrawList->VtxBuffer.Size * sizeof(ImDrawVert));

            pVertices += pDrawList->VtxBuffer.Size;

            // Copy the indices
            memcpy(pIndices, pDrawList->IdxBuffer.Data, pDrawList->IdxBuffer.Size * sizeof(ImDrawIdx));
            pIndices += pDrawList->IdxBuffer.Size;
        }

        // Flush memory?

        // Unmap the vertices and indices
        vkUnmapMemory(mDevice, mGuiVertices[swapIndex].mem);
        vkUnmapMemory(mDevice, mGuiIndices[swapIndex].mem);

        // Update Uniform data
        uint8_t *pData;

        ImGuiIO& io = ImGui::GetIO();

        GuiUniform* pUniform = &mGuiUniform;
        ret = vkMapMemory(mDevice, pUniform->mem, 0, pUniform->allocSize, 0, (void **) &pData);
        assert(!ret);

        GuiUniformData data;
        data.scale[0]       = 2.0f/io.DisplaySize.x;
        data.scale[1]       = 2.0f/io.DisplaySize.y;
        data.translate[0]   = -1.0f;
        data.translate[1]   = -1.0f;

        // copy uniform data to the buffer
        memcpy(pData,  (const void*) &data, sizeof(GuiUniformData));

        vkUnmapMemory(mDevice, mGuiUniform.mem);
    }
}

///////////////////////////////////////////////////////////////////////////////

void Gui::Draw(VkCommandBuffer cmdBuffer)
{
    uint32_t swapIndex = mSample->GetSwapChainIndex();

    // Bind the pipeline
    vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mGuiPipeline);

    // Bind the descriptor set
    uint32_t dynamicOffsets[1] = {0};
    vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mGuiPipelineLayout, 0, 1, &mGuiDescriptorSet, 1, dynamicOffsets);

    // Bind the Vertex buffer
    VkBuffer vertexBuffers[1] = {mGuiVertices[swapIndex].buf};
    VkDeviceSize vertexOffsets[1] = {0};
    vkCmdBindVertexBuffers(cmdBuffer, 0, 1, vertexBuffers, vertexOffsets);
    vkCmdBindIndexBuffer(cmdBuffer, mGuiIndices[swapIndex].buf, 0, VK_INDEX_TYPE_UINT16);

    // Set the viewport
    VkViewport viewport;
    viewport.x          = 0;
    viewport.y          = 0;
    viewport.width      = ImGui::GetIO().DisplaySize.x;
    viewport.height     = ImGui::GetIO().DisplaySize.y;
    viewport.minDepth   = 0.0f;
    viewport.maxDepth   = 1.0f;
    vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);

    // Render the command lists:
    ImDrawData *pDrawData = ImGui::GetDrawData();
    if (pDrawData)
    {
        uint32_t vertexOffset = 0;
        uint32_t indexOffset = 0;
        for (uint32_t cl = 0; cl < pDrawData->CmdListsCount; cl++)
        {
            ImDrawList *pCommandList = pDrawData->CmdLists[cl];
            for (uint32_t command = 0; command < pCommandList->CmdBuffer.Size; command++)
            {
                ImDrawCmd *pCommand = &pCommandList->CmdBuffer[command];
                if (pCommand->UserCallback)
                {
                    pCommand->UserCallback(pCommandList, pCommand);
                }
                else
                {
                    VkRect2D scissor;
                    scissor.offset.x = (int32_t) (pCommand->ClipRect.x);
                    scissor.offset.y = (int32_t) (pCommand->ClipRect.y);
                    scissor.extent.width  = (uint32_t) (pCommand->ClipRect.z - pCommand->ClipRect.x);
                    scissor.extent.height = (uint32_t) (pCommand->ClipRect.w - pCommand->ClipRect.y);
                    vkCmdSetScissor(cmdBuffer, 0, 1, &scissor);
                    vkCmdDrawIndexed(cmdBuffer, pCommand->ElemCount, 1, indexOffset, vertexOffset, 0);
                }
                indexOffset += pCommand->ElemCount;
            }
            vertexOffset += pCommandList->VtxBuffer.Size;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

void Gui::Destroy()
{
    VkDevice mDevice = mSample->GetDevice();

    for (uint32_t i = 0; i < mSample->GetSwapChainCount(); i++)
    {
        vkFreeMemory(mDevice, mGuiVertices[i].mem, nullptr);
        vkFreeMemory(mDevice, mGuiIndices[i].mem, nullptr);
        vkDestroyBuffer(mDevice, mGuiVertices[i].buf, nullptr);
        vkDestroyBuffer(mDevice, mGuiIndices[i].buf, nullptr);
    }
    vkFreeMemory(mDevice, mGuiUniform.mem, nullptr);
    vkDestroyBuffer(mDevice, mGuiUniform.buf, nullptr);

    // Destroy the texture
    delete mpGuiTextureObject;

    vkDestroyDescriptorSetLayout(mDevice, mGuiDescriptorLayout, nullptr);

    vkDestroyDescriptorPool(mDevice, mGuiDescriptorPool, nullptr);

    vkDestroyPipelineLayout(mDevice, mGuiPipelineLayout, nullptr);

    vkDestroyPipeline(mDevice, mGuiPipeline, nullptr);
}

///////////////////////////////////////////////////////////////////////////////

void Gui::InitUniforms()
{
    // The Gui uniform for the Gui contains scaling and translation data
    memset(&mGuiUniform, 0, sizeof(GuiUniform));

    VkResult err = VK_SUCCESS;

    // Create our buffer object
    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.pNext = NULL;
    bufferCreateInfo.size = sizeof(GuiUniformData);
    bufferCreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    bufferCreateInfo.flags = 0;

    err = vkCreateBuffer(mDevice, &bufferCreateInfo, NULL, &mGuiUniform.buf);
    assert(!err);

    // Obtain the requirements on memory for this buffer
    VkMemoryRequirements mem_reqs;
    vkGetBufferMemoryRequirements(mDevice, mGuiUniform.buf, &mem_reqs);
    assert(!err);

    // And allocate memory according to those requirements
    VkMemoryAllocateInfo memoryAllocateInfo;
    memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocateInfo.pNext = NULL;
    memoryAllocateInfo.allocationSize = 0;
    memoryAllocateInfo.memoryTypeIndex = 0;
    memoryAllocateInfo.allocationSize = mem_reqs.size;
    bool pass = mSample->GetMemoryTypeFromProperties(mem_reqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &memoryAllocateInfo.memoryTypeIndex);
    assert(pass);

    // We keep the size of the allocation for remapping it later when we update contents
    mGuiUniform.allocSize = memoryAllocateInfo.allocationSize;

    err = vkAllocateMemory(mDevice, &memoryAllocateInfo, NULL, &mGuiUniform.mem);
    assert(!err);

    // Bind our buffer to the memory
    err = vkBindBufferMemory(mDevice, mGuiUniform.buf, mGuiUniform.mem, 0);
    assert(!err);

    mGuiUniform.bufferInfo.buffer = mGuiUniform.buf;
    mGuiUniform.bufferInfo.offset = 0;
    mGuiUniform.bufferInfo.range = sizeof(GuiUniformData);
}

///////////////////////////////////////////////////////////////////////////////

void Gui::InitGuiVertexBuffer(uint32_t bufferID, size_t size)
{
    VkResult   err;
    bool        pass;

    if (mGuiVertices[bufferID].buf != VK_NULL_HANDLE)
    {
        vkDestroyBuffer(mDevice, mGuiVertices[bufferID].buf, nullptr);
        mGuiVertices[bufferID].allocSize = 0;
    }

    // Create our buffer object.
    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.sType              = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.pNext              = nullptr;
    bufferCreateInfo.size               = size;
    bufferCreateInfo.usage              = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferCreateInfo.flags              = 0;
    err = vkCreateBuffer(mDevice, &bufferCreateInfo, nullptr, &mGuiVertices[bufferID].buf);
    VK_CHECK(!err);

    // Obtain the memory requirements for this buffer.
    VkMemoryRequirements mem_reqs;
    vkGetBufferMemoryRequirements(mDevice, mGuiVertices[bufferID].buf, &mem_reqs);
    VK_CHECK(!err);

    // Remember the memory size
    mGuiVertices[bufferID].allocSize = mem_reqs.size;

    // And allocate memory according to those requirements.
    VkMemoryAllocateInfo memoryAllocateInfo = {};
    memoryAllocateInfo.sType            = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocateInfo.pNext            = nullptr;
    memoryAllocateInfo.allocationSize   = 0;
    memoryAllocateInfo.memoryTypeIndex  = 0;
    memoryAllocateInfo.allocationSize   = mem_reqs.size;
    pass = mSample->GetMemoryTypeFromProperties(mem_reqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &memoryAllocateInfo.memoryTypeIndex);
    VK_CHECK(pass);
    err = vkAllocateMemory(mDevice, &memoryAllocateInfo, nullptr, &mGuiVertices[bufferID].mem);
    VK_CHECK(!err);

    // Bind our buffer to the memory.
    err = vkBindBufferMemory(mDevice, mGuiVertices[bufferID].buf, mGuiVertices[bufferID].mem, 0);
    VK_CHECK(!err);

    // The vertices need to be defined so that the pipeline understands how the
    // data is laid out. This is done by providing a VkPipelineVertexInputStateCreateInfo
    // structure with the correct information.
    mGuiVertices[bufferID].vi.sType                              = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    mGuiVertices[bufferID].vi.pNext                              = nullptr;
    mGuiVertices[bufferID].vi.vertexBindingDescriptionCount      = 1;
    mGuiVertices[bufferID].vi.vertexAttributeDescriptionCount    = 3;

    // Allocate the correct number of Binding descriptors.
    mGuiVertices[bufferID].vi_bindings = new VkVertexInputBindingDescription[mGuiVertices[bufferID].vi.vertexBindingDescriptionCount];
    mGuiVertices[bufferID].vi.pVertexBindingDescriptions = mGuiVertices[bufferID].vi_bindings;

    // Allocate the correct number of Attribute Descriptors.
    mGuiVertices[bufferID].vi_attrs = new VkVertexInputAttributeDescription[mGuiVertices[bufferID].vi.vertexAttributeDescriptionCount];
    mGuiVertices[bufferID].vi.pVertexAttributeDescriptions = mGuiVertices[bufferID].vi_attrs;

    // We bind the buffer as a whole, using the correct buffer ID.
    // This defines the stride for each element of the vertex array.
    mGuiVertices[bufferID].vi_bindings[0].binding    = VERTEX_BUFFER_BIND_ID;
    mGuiVertices[bufferID].vi_bindings[0].stride     = sizeof(ImDrawVert);
    mGuiVertices[bufferID].vi_bindings[0].inputRate  = VK_VERTEX_INPUT_RATE_VERTEX;

    // Within each element, we define the attributes. At location 0,
    // the vertex positions, in float3 format, with offset 0 as they are
    // first in the array structure.
    mGuiVertices[bufferID].vi_attrs[0].binding       = VERTEX_BUFFER_BIND_ID;
    mGuiVertices[bufferID].vi_attrs[0].location      = 0;
    mGuiVertices[bufferID].vi_attrs[0].format        = VK_FORMAT_R32G32_SFLOAT; //float2
    mGuiVertices[bufferID].vi_attrs[0].offset        = 0;

    // The second location is vertex UVSs
    // These appear in each element in memory after the float2 vertex
    // position, so the offset is set accordingly.
    mGuiVertices[bufferID].vi_attrs[1].binding       = VERTEX_BUFFER_BIND_ID;
    mGuiVertices[bufferID].vi_attrs[1].location      = 1;
    mGuiVertices[bufferID].vi_attrs[1].format        = VK_FORMAT_R32G32_SFLOAT; //float2
    mGuiVertices[bufferID].vi_attrs[1].offset        = sizeof(float) * 2; //float2 pos

    // The third location is the vertex colors, in RGBA float4 format.
    // These appear in each element in memory after the float2 vertex
    // position and float 2 uv, so the offset is set accordingly.
    mGuiVertices[bufferID].vi_attrs[2].binding       = VERTEX_BUFFER_BIND_ID;
    mGuiVertices[bufferID].vi_attrs[2].location      = 2;
    mGuiVertices[bufferID].vi_attrs[2].format        = VK_FORMAT_R8G8B8A8_UNORM;
    mGuiVertices[bufferID].vi_attrs[2].offset        = sizeof(float) * 4; //float2 pos + float2 uv
}

///////////////////////////////////////////////////////////////////////////////

void Gui::InitGuiIndexBuffer(uint32_t bufferID, size_t size)
{
    VkResult   err;
    bool   pass;

    if (mGuiIndices[bufferID].buf != VK_NULL_HANDLE)
    {
        vkDestroyBuffer(mDevice, mGuiIndices[bufferID].buf, nullptr);
        mGuiIndices[bufferID].allocSize = 0;
    }

    // Create our buffer object.
    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.sType              = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.pNext              = nullptr;
    bufferCreateInfo.size               = size;
    bufferCreateInfo.usage              = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    bufferCreateInfo.flags              = 0;
    err = vkCreateBuffer(mDevice, &bufferCreateInfo, nullptr, &mGuiIndices[bufferID].buf);
    VK_CHECK(!err);

    // Obtain the memory requirements for this buffer.
    VkMemoryRequirements mem_reqs;
    vkGetBufferMemoryRequirements(mDevice, mGuiIndices[bufferID].buf, &mem_reqs);
    VK_CHECK(!err);

    // Remember the memory size
    mGuiIndices[bufferID].allocSize = mem_reqs.size;

    // And allocate memory according to those requirements.
    VkMemoryAllocateInfo memoryAllocateInfo = {};
    memoryAllocateInfo.sType            = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocateInfo.pNext            = nullptr;
    memoryAllocateInfo.allocationSize   = 0;
    memoryAllocateInfo.memoryTypeIndex  = 0;
    memoryAllocateInfo.allocationSize   = mem_reqs.size;
    pass = mSample->GetMemoryTypeFromProperties(mem_reqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &memoryAllocateInfo.memoryTypeIndex);
    VK_CHECK(pass);
    err = vkAllocateMemory(mDevice, &memoryAllocateInfo, nullptr, &mGuiIndices[bufferID].mem);
    VK_CHECK(!err);

    // Bind our buffer to the memory.
    err = vkBindBufferMemory(mDevice, mGuiIndices[bufferID].buf, mGuiIndices[bufferID].mem, 0);
    VK_CHECK(!err);
}

///////////////////////////////////////////////////////////////////////////////

void Gui::InitTextures()
{
    ImGuiIO& io = ImGui::GetIO();

    unsigned char* pPixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pPixels, &width, &height);

    mpGuiTextureObject = new TextureObject();
    mpGuiTextureObject->SetWidth(width);
    mpGuiTextureObject->SetHeight(height);
    mpGuiTextureObject->SetFormat(VK_FORMAT_R8G8B8A8_UNORM);
    bool success = true;

    success = TextureObject::FromTGAImageData(mSample, mpGuiTextureObject, (uint32_t *) pPixels);
    assert(success);
}

///////////////////////////////////////////////////////////////////////////////

void Gui::InitLayouts() {

    // This sample has two  bindings, a sampler in the fragment shader and a uniform in the
    // vertex shader for scaling and translation information.
    VkDescriptorSetLayoutBinding uniformAndSamplerBinding[2] = {};

    // Our Uniform data
    uniformAndSamplerBinding[0].binding = 0;
    uniformAndSamplerBinding[0].descriptorCount = 1;
    uniformAndSamplerBinding[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    uniformAndSamplerBinding[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    uniformAndSamplerBinding[0].pImmutableSamplers = nullptr;

    // Our texture sampler
    uniformAndSamplerBinding[1].binding = 1;
    uniformAndSamplerBinding[1].descriptorCount = 1;
    uniformAndSamplerBinding[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    uniformAndSamplerBinding[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    uniformAndSamplerBinding[1].pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
    descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutCreateInfo.pNext = nullptr;
    descriptorSetLayoutCreateInfo.bindingCount = 2;
    descriptorSetLayoutCreateInfo.pBindings = &uniformAndSamplerBinding[0];
    VkResult err;
    err = vkCreateDescriptorSetLayout(mDevice, &descriptorSetLayoutCreateInfo, nullptr, &mGuiDescriptorLayout);
    VK_CHECK(!err);

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.pNext = nullptr;
    pipelineLayoutCreateInfo.setLayoutCount = 1;
    pipelineLayoutCreateInfo.pSetLayouts = &mGuiDescriptorLayout;
    err = vkCreatePipelineLayout(mDevice, &pipelineLayoutCreateInfo, nullptr, &mGuiPipelineLayout);
    VK_CHECK(!err);
}

///////////////////////////////////////////////////////////////////////////////

void Gui::InitPipelines()
{
    VkResult err;
    VkShaderModule sh_gui_vert  =mSample->CreateShaderModule((const uint32_t*) shader_gui_vert,shader_gui_vert_size);
    VkShaderModule sh_gui_frag  =mSample->CreateShaderModule((const uint32_t*) shader_gui_frag,shader_gui_frag_size);

    // State for rasterization, such as polygon fill mode is defined.
    VkPipelineRasterizationStateCreateInfo rs = {};
    rs.sType                                    = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rs.polygonMode                              = VK_POLYGON_MODE_FILL;
    rs.cullMode                                 = VK_CULL_MODE_NONE;
    rs.frontFace                                = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rs.depthClampEnable                         = VK_FALSE;
    rs.rasterizerDiscardEnable                  = VK_FALSE;
    rs.depthBiasEnable                          = VK_FALSE;
    rs.lineWidth                                = 1.0f;

    // Standard depth and stencil state is defined
    VkPipelineDepthStencilStateCreateInfo  ds = {};
    ds.sType                                    = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    ds.depthTestEnable                          = VK_FALSE;
    ds.depthWriteEnable                         = VK_FALSE;
    ds.depthCompareOp                           = VK_COMPARE_OP_LESS_OR_EQUAL;
    ds.depthBoundsTestEnable                    = VK_FALSE;
    ds.back.failOp                              = VK_STENCIL_OP_KEEP;
    ds.back.passOp                              = VK_STENCIL_OP_KEEP;
    ds.back.compareOp                           = VK_COMPARE_OP_ALWAYS;
    ds.stencilTestEnable                        = VK_FALSE;
    ds.front                                    = ds.back;

    // We define two shader stages: our vertex and fragment shader.
    // they are embedded as SPIR-V into a header file for ease of deployment.
    VkPipelineShaderStageCreateInfo shaderStages[2] = {};
    shaderStages[0].sType                       = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[0].stage                       = VK_SHADER_STAGE_VERTEX_BIT;
    shaderStages[0].module                      = sh_gui_vert;
    shaderStages[0].pName                       = "main";
    shaderStages[1].sType                       = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[1].stage                       = VK_SHADER_STAGE_FRAGMENT_BIT;
    shaderStages[1].module                      = sh_gui_frag;
    shaderStages[1].pName                       = "main";

    VkPipelineColorBlendAttachmentState att_state[1] = {};
    att_state[0].colorWriteMask                 = VK_COLOR_COMPONENT_R_BIT |
                                                  VK_COLOR_COMPONENT_G_BIT |
                                                  VK_COLOR_COMPONENT_B_BIT |
                                                  VK_COLOR_COMPONENT_A_BIT;
    att_state[0].blendEnable                    = VK_TRUE;
    att_state[0].srcColorBlendFactor            = VK_BLEND_FACTOR_SRC_ALPHA;
    att_state[0].dstColorBlendFactor            = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    att_state[0].alphaBlendOp                   = VK_BLEND_OP_ADD;
    att_state[0].srcAlphaBlendFactor            = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    att_state[0].dstAlphaBlendFactor            = VK_BLEND_FACTOR_ZERO;

    VkPipelineColorBlendStateCreateInfo cb = {};
    cb.sType                                    = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    cb.attachmentCount                          = 1;
    cb.pAttachments                             = &att_state[0];

    // Our vertex input is a single vertex buffer, and its layout is defined
    // in our mParticleVertices object already. Use this when creating the pipeline.
    VkPipelineVertexInputStateCreateInfo visci = mGuiVertices[0].vi;

    mSample->InitPipeline(VK_NULL_HANDLE, 2, shaderStages,  mGuiPipelineLayout, mRenderPass, 0, &visci, nullptr, nullptr, nullptr, &rs, nullptr, &ds, &cb, nullptr, false, VK_NULL_HANDLE, &mGuiPipeline);

    vkDestroyShaderModule(mDevice, sh_gui_frag, nullptr);
    vkDestroyShaderModule(mDevice, sh_gui_vert, nullptr);
}

///////////////////////////////////////////////////////////////////////////////

void Gui::InitDescriptorSets()
{
    VkResult err = VK_SUCCESS;

    //Create a pool with the amount of descriptors we require
    VkDescriptorPoolSize poolSize[2] = {};

    poolSize[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    poolSize[0].descriptorCount = 2;

    poolSize[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSize[1].descriptorCount = 2;

    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
    descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolCreateInfo.pNext = nullptr;
    descriptorPoolCreateInfo.maxSets = 2;
    descriptorPoolCreateInfo.poolSizeCount = 2;
    descriptorPoolCreateInfo.pPoolSizes = poolSize;

    err = vkCreateDescriptorPool(mDevice, &descriptorPoolCreateInfo, NULL, &mGuiDescriptorPool);
    VK_CHECK(!err);

    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
    descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorSetAllocateInfo.pNext = nullptr;
    descriptorSetAllocateInfo.descriptorPool = mGuiDescriptorPool;
    descriptorSetAllocateInfo.descriptorSetCount = 1;
    descriptorSetAllocateInfo.pSetLayouts = &mGuiDescriptorLayout;

    err = vkAllocateDescriptorSets(mDevice, &descriptorSetAllocateInfo, &mGuiDescriptorSet);
    VK_CHECK(!err);

    VkDescriptorImageInfo descriptorImageInfo = {};

    descriptorImageInfo.sampler = mpGuiTextureObject->GetSampler();
    descriptorImageInfo.imageView = mpGuiTextureObject->GetView();
    descriptorImageInfo.imageLayout = mpGuiTextureObject->GetLayout();

    VkWriteDescriptorSet writes[2] = {};

    writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[0].dstBinding = 0;
    writes[0].dstSet = mGuiDescriptorSet;
    writes[0].descriptorCount = 1;
    writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    writes[0].pBufferInfo = &mGuiUniform.bufferInfo;

    writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[1].dstBinding = 1;
    writes[1].dstSet = mGuiDescriptorSet;
    writes[1].descriptorCount = 1;
    writes[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writes[1].pImageInfo = &descriptorImageInfo;

    vkUpdateDescriptorSets(mDevice, 2, &writes[0], 0, nullptr);
}