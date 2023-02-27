//                  Copyright (c) 2016 QUALCOMM Technologies Inc.
//                              All Rights Reserved.

#include "sampleThread.h"
#include "sample.h"

///////////////////////////////////////////////////////////////////////////////

void* ThreadFunc(void *pData)
{
    ThreadData* pThreadData = reinterpret_cast<ThreadData*>(pData);

    //LOGI("ThreadFunc  %d executing in group %d", pThreadData->index, pThreadData->group);

    // Create the Vertex Buffer
    InitVertexBuffers(pThreadData);

    // Create the Layouts
    InitLayouts(pThreadData);

    // Create the DescriptorSet
    InitDescriptorSet(pThreadData);

    // Update the Descriptor Set
    UpdateDescriptorSet(pThreadData);

    // Create the Pipeline
    InitPipeline(pThreadData);

    // Create the command buffer
    InitCommandBuffer(pThreadData);

    // Update the Vertex Buffer
    UpdateVertexBuffer(pThreadData);

    // Update the command buffer
    UpdateCommandBuffer(pThreadData);

};


// Our vertex bufferdata is a cube, with associated vertex threadColors and UVs.
float vb[][9] = {
        //      position             color                   uv

        //left
        { -1.0f,-1.0f,-1.0f,    1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 0.0f },
        { -1.0f,-1.0f, 1.0f,    1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 0.0f },
        { -1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f },

        { -1.0f,-1.0f,-1.0f,    1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 0.0f },
        { -1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f },
        { -1.0f, 1.0f,-1.0f,    1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 1.0f },

        //back
        { 1.0f, 1.0f,-1.0f,     1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 1.0f },
        {-1.0f,-1.0f,-1.0f,     1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 0.0f },
        {-1.0f, 1.0f,-1.0f,     1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f },

        { 1.0f, 1.0f,-1.0f,     1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 1.0f },
        { 1.0f,-1.0f,-1.0f,     1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 0.0f },
        {-1.0f,-1.0f,-1.0f,     1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 0.0f },

        //top
        { 1.0f, -1.0f, 1.0f,    1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f },
        {-1.0f, -1.0f,-1.0f,    1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 0.0f },
        { 1.0f, -1.0f,-1.0f,    1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 0.0f },

        { 1.0f, -1.0f, 1.0f,    1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f },
        { -1.0f,-1.0f, 1.0f,    1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 1.0f },
        { -1.0f,-1.0f,-1.0f,    1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 0.0f },

        //front
        { -1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 1.0f },
        { -1.0f,-1.0f, 1.0f,    1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 0.0f },
        {  1.0f,-1.0f, 1.0f,    1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 0.0f },

        { 1.0f, 1.0f, 1.0f,     1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f },
        { -1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 1.0f },
        { 1.0f,-1.0f, 1.0f,     1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 0.0f },

        //right
        { 1.0f, 1.0f, 1.0f,     1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 1.0f },
        { 1.0f,-1.0f,-1.0f,     1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 0.0f },
        { 1.0f, 1.0f,-1.0f,     1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f },

        { 1.0f,-1.0f,-1.0f,     1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 0.0f },
        { 1.0f, 1.0f, 1.0f,     1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 1.0f },
        { 1.0f,-1.0f, 1.0f,     1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 0.0f },

        //bottom
        { 1.0f, 1.0f, 1.0f,     1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 0.0f },
        { 1.0f, 1.0f,-1.0f,     1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f },
        { -1.0f, 1.0f,-1.0f,    1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 1.0f },

        { 1.0f, 1.0f, 1.0f,     1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 0.0f },
        { -1.0f, 1.0f,-1.0f,    1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 1.0f },
        { -1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 0.0f },
};

//  Thread colors
float_t threadColors[MAX_NUM_THREADS_PER_GROUP][4] =
        {
        {1.000f, 0.000f, 0.000f, 1.0f },
        {1.000f, 0.384f, 0.000f, 1.0f },
        {1.000f, 0.749f, 0.000f, 1.0f },
        {0.882f, 1.000f, 0.000f, 1.0f },

        {0.502f, 1.000f, 0.000f, 1.0f },
        {0.117f, 1.000f, 0.000f, 1.0f },
        {0.000f, 1.000f, 0.169f, 1.0f },
        {0.000f, 1.000f, 0.631f, 1.0f },

        {0.000f, 1.000f, 1.000f, 1.0f },
        {0.000f, 0.631f, 1.000f, 1.0f },
        {0.000f, 0.251f, 1.000f, 1.0f },
        {0.133f, 0.000f, 1.000f, 1.0f },

        {0.502f, 0.000f, 1.000f, 1.0f },
        {0.882f, 0.000f, 1.000f, 1.0f },
        {1.000f, 0.000f, 0.749f, 1.0f },
        {1.000f, 0.000f, 0.369f, 1.0f }
};

///////////////////////////////////////////////////////////////////////////////

void UpdateVertexBuffer(ThreadData* pThreadData)
{
    VkResult   err;

    float verts[36][9];
    memcpy(verts,vb, sizeof(vb));

    // update the cube colors to show in this thread
    for (uint_t i=0; i<(12*3); i++)
    {
        memcpy(&verts[i][3], &threadColors[pThreadData->index][0], sizeof(float_t)*4);
    }

    // update the positions
    float spacing = 3.2f;
    float xOffset = spacing * (pThreadData->positionIdx    %4) -1.5*spacing;
    float yOffset = spacing * (pThreadData->positionIdx/4  %4) -1.5*spacing;
    float zOffset = spacing * (pThreadData->positionIdx/16 %4) -1.5*spacing;

    for (uint_t i=0; i<(12*3); i++)
    {
        verts[i][0] += xOffset;
        verts[i][1] += yOffset;
        verts[i][2] += zOffset;
    }

    // Now we need to map the memory of this new allocation so the CPU can edit it.
    void *data;
    err = vkMapMemory(pThreadData->device, pThreadData->vertices.mem, 0, pThreadData->vertices.allocSize, 0, &data);
    VK_CHECK(!err);

    // Copy our triangle verticies and threadColors into the mapped memory area.
    memcpy(data, verts, sizeof(verts));

    // Unmap the memory back from the CPU.
    vkUnmapMemory(pThreadData->device, pThreadData->vertices.mem);

}

///////////////////////////////////////////////////////////////////////////////

void InitVertexBuffers(ThreadData* pThreadData)
{
    if (pThreadData->vertices.buf != VK_NULL_HANDLE)
    {
        return;
    }

    VkResult   err;
    bool   pass;

    // Our mVertices member contains the types required for storing
    // and defining our vertex buffer within the graphics pipeline.
    memset(&pThreadData->vertices, 0, sizeof(&pThreadData->vertices));

    // Create our buffer object.
    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.sType              = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.pNext              = nullptr;
    bufferCreateInfo.size               = sizeof(vb);
    bufferCreateInfo.usage              = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferCreateInfo.flags              = 0;
    err = vkCreateBuffer(pThreadData->device, &bufferCreateInfo, nullptr, &pThreadData->vertices.buf);
    VK_CHECK(!err);

    // Obtain the memory requirements for this buffer.
    VkMemoryRequirements mem_reqs;
    vkGetBufferMemoryRequirements(pThreadData->device, pThreadData->vertices.buf, &mem_reqs);
    VK_CHECK(!err);

    // And allocate memory according to those requirements.
    VkMemoryAllocateInfo memoryAllocateInfo = {};
    memoryAllocateInfo.sType            = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocateInfo.pNext            = nullptr;
    memoryAllocateInfo.allocationSize   = 0;
    memoryAllocateInfo.memoryTypeIndex  = 0;
    memoryAllocateInfo.allocationSize   = mem_reqs.size;
    pass = pThreadData->pSample->GetMemoryTypeFromProperties(mem_reqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &memoryAllocateInfo.memoryTypeIndex);
    VK_CHECK(pass);

    pThreadData->vertices.allocSize = memoryAllocateInfo.allocationSize;
    err = vkAllocateMemory(pThreadData->device, &memoryAllocateInfo, nullptr, &pThreadData->vertices.mem);
    VK_CHECK(!err);


    UpdateVertexBuffer(pThreadData);

    // Bind our buffer to the memory.
    err = vkBindBufferMemory(pThreadData->device, pThreadData->vertices.buf, pThreadData->vertices.mem, 0);
    VK_CHECK(!err);

    // The vertices need to be defined so that the pipeline understands how the
    // data is laid out. This is done by providing a VkPipelineVertexInputStateCreateInfo
    // structure with the correct information.
    pThreadData->vertices.vi.sType                              = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    pThreadData->vertices.vi.pNext                              = nullptr;
    pThreadData->vertices.vi.vertexBindingDescriptionCount      = 1;
    pThreadData->vertices.vi.vertexAttributeDescriptionCount    = 3;

    // Allocate the correct number of Binding descriptors.
    pThreadData->vertices.vi_bindings = new VkVertexInputBindingDescription[pThreadData->vertices.vi.vertexBindingDescriptionCount];
    pThreadData->vertices.vi.pVertexBindingDescriptions = pThreadData->vertices.vi_bindings;

    // Allocate the correct number of Attribute Descriptors.
    pThreadData->vertices.vi_attrs = new VkVertexInputAttributeDescription[pThreadData->vertices.vi.vertexAttributeDescriptionCount];
    pThreadData->vertices.vi.pVertexAttributeDescriptions = pThreadData->vertices.vi_attrs;

    // We bind the buffer as a whole, using the correct buffer ID.
    // This defines the stride for each element of the vertex array.
    pThreadData->vertices.vi_bindings[0].binding    = VERTEX_BUFFER_BIND_ID;
    pThreadData->vertices.vi_bindings[0].stride     = sizeof(vb[0]);
    pThreadData->vertices.vi_bindings[0].inputRate  = VK_VERTEX_INPUT_RATE_VERTEX;

    // Within each element, we define the attributes. At location 0,
    // the vertex positions, in float3 format, with offset 0 as they are
    // first in the array structure.
    pThreadData->vertices.vi_attrs[0].binding       = VERTEX_BUFFER_BIND_ID;
    pThreadData->vertices.vi_attrs[0].location      = 0;
    pThreadData->vertices.vi_attrs[0].format        = VK_FORMAT_R32G32B32_SFLOAT; //float3
    pThreadData->vertices.vi_attrs[0].offset        = 0;

    // The second location is the vertex threadColors, in RGBA float4 format.
    // These appear in each element in memory after the float3 vertex
    // positions, so the offset is set accordingly.
    pThreadData->vertices.vi_attrs[1].binding       = VERTEX_BUFFER_BIND_ID;
    pThreadData->vertices.vi_attrs[1].location      = 1;
    pThreadData->vertices.vi_attrs[1].format        = VK_FORMAT_R32G32B32A32_SFLOAT; //float4
    pThreadData->vertices.vi_attrs[1].offset        = sizeof(float) * 3;

    // The third location is vertex UVs
    pThreadData->vertices.vi_attrs[2].binding       = VERTEX_BUFFER_BIND_ID;
    pThreadData->vertices.vi_attrs[2].location      = 2;
    pThreadData->vertices.vi_attrs[2].format        = VK_FORMAT_R32G32_SFLOAT; //float2
    pThreadData->vertices.vi_attrs[2].offset        = sizeof(float) * 7; //float3 pos + float4 color
}

///////////////////////////////////////////////////////////////////////////////

void InitLayouts(ThreadData* pThreadData)
{
    VkResult ret = VK_SUCCESS;

    if (pThreadData->descriptorLayout == VK_NULL_HANDLE) {

        // This sample has two  bindings, a sampler in the fragment shader and a uniform in the
        // vertex shader for MVP matrix.
        VkDescriptorSetLayoutBinding uniformAndSamplerBinding[2] = {};

        // Our MVP matrix
        uniformAndSamplerBinding[0].binding              = 0;
        uniformAndSamplerBinding[0].descriptorCount     = 1;
        uniformAndSamplerBinding[0].descriptorType      = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        uniformAndSamplerBinding[0].stageFlags          = VK_SHADER_STAGE_VERTEX_BIT;
        uniformAndSamplerBinding[0].pImmutableSamplers  = nullptr;

        // Our texture sampler
        uniformAndSamplerBinding[1].binding             = 1;
        uniformAndSamplerBinding[1].descriptorCount     = 1;
        uniformAndSamplerBinding[1].descriptorType      = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        uniformAndSamplerBinding[1].stageFlags          = VK_SHADER_STAGE_FRAGMENT_BIT;
        uniformAndSamplerBinding[1].pImmutableSamplers  = nullptr;

        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
        descriptorSetLayoutCreateInfo.sType             = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorSetLayoutCreateInfo.pNext             = nullptr;
        descriptorSetLayoutCreateInfo.bindingCount      = 2;
        descriptorSetLayoutCreateInfo.pBindings         = &uniformAndSamplerBinding[0];

        ret = vkCreateDescriptorSetLayout(pThreadData->device, &descriptorSetLayoutCreateInfo, nullptr, &pThreadData->descriptorLayout);
        VK_CHECK(!ret);
    }

    if (pThreadData->pipelineLayout == VK_NULL_HANDLE)
    {
        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
        pipelineLayoutCreateInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutCreateInfo.pNext                  = nullptr;
        pipelineLayoutCreateInfo.setLayoutCount         = 1;
        pipelineLayoutCreateInfo.pSetLayouts            = &pThreadData->descriptorLayout;
        ret = vkCreatePipelineLayout(pThreadData->device, &pipelineLayoutCreateInfo, nullptr, &pThreadData->pipelineLayout);
        VK_CHECK(!ret);
    }
}

///////////////////////////////////////////////////////////////////////////////

void InitCommandBuffer(ThreadData* pThreadData)
{
    VkResult err;

    if (pThreadData->commandBufferPool == VK_NULL_HANDLE) {
        VkResult ret = VK_SUCCESS;

        // Command buffers are allocated from a pool; we define that pool here and create it.
        VkCommandPoolCreateInfo commandPoolCreateInfo = {};
        commandPoolCreateInfo.sType                     = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        commandPoolCreateInfo.pNext                     = nullptr;
        commandPoolCreateInfo.queueFamilyIndex          = pThreadData->pSample->GetQueueFamilyIndex();
        commandPoolCreateInfo.flags                     = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        ret = vkCreateCommandPool(pThreadData->device, &commandPoolCreateInfo, nullptr, &pThreadData->commandBufferPool);
        VK_CHECK(!ret);

        VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
        commandBufferAllocateInfo.sType                 = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        commandBufferAllocateInfo.pNext                 = nullptr;
        commandBufferAllocateInfo.commandPool           = pThreadData->commandBufferPool;
        commandBufferAllocateInfo.level                 = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
        commandBufferAllocateInfo.commandBufferCount    = 1;

        ret = vkAllocateCommandBuffers(pThreadData->device, &commandBufferAllocateInfo, &pThreadData->commandBuffer);
        VK_CHECK(!ret);
    }
}

///////////////////////////////////////////////////////////////////////////////

void UpdateCommandBuffer(ThreadData* pThreadData)
{
    VkResult ret = VK_SUCCESS;

    ret = vkResetCommandBuffer(pThreadData->commandBuffer,VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
    VK_CHECK(!ret);

    VkCommandBufferInheritanceInfo cmd_buf_hinfo = {};
    cmd_buf_hinfo.sType                 = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
    cmd_buf_hinfo.pNext                 = nullptr;
    cmd_buf_hinfo.renderPass            = pThreadData->renderPass;
    cmd_buf_hinfo.subpass               = 0;
    cmd_buf_hinfo.framebuffer           = VK_NULL_HANDLE;
    cmd_buf_hinfo.occlusionQueryEnable  = VK_FALSE;
    cmd_buf_hinfo.queryFlags            = 0;
    cmd_buf_hinfo.pipelineStatistics    = 0;

    VkCommandBufferBeginInfo cmd_buf_info = {};
    cmd_buf_info.sType                  = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmd_buf_info.pNext                  = nullptr;
    cmd_buf_info.flags                  = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
    cmd_buf_info.pInheritanceInfo       = &cmd_buf_hinfo;

    // By calling vkBeginCommandBuffer, cmdBuffer is put into the recording state.
    ret = vkBeginCommandBuffer(pThreadData->commandBuffer, &cmd_buf_info);

    // Set our pipeline. This holds all major state
    // the pipeline defines, for example, that the vertex buffer is a triangle list.
    vkCmdBindPipeline(pThreadData->commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,pThreadData->pipeline);

    //bind out descriptor set, which handles our uniforms and samplers
    uint32_t dynamicOffsets[1] = {0};
    vkCmdBindDescriptorSets(pThreadData->commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            pThreadData->pipelineLayout, 0, 1,
                            &pThreadData->descriptorSet, 1,
                            dynamicOffsets);

    // Bind our vertex buffer, with a 0 offset.
    VkDeviceSize offsets[1] = {0};
    vkCmdBindVertexBuffers(pThreadData->commandBuffer, VERTEX_BUFFER_BIND_ID, 1,
                           &pThreadData->vertices.buf, offsets);

    // Issue a draw command, with our vertices. 12 triangles of 3 vertices.
    vkCmdDraw(pThreadData->commandBuffer, 3 * 12, 1, 0, 0);

    ret= vkEndCommandBuffer(pThreadData->commandBuffer);
    VK_CHECK(!ret);
}

///////////////////////////////////////////////////////////////////////////////

void InitDescriptorSet(ThreadData* pThreadData)
{
    VkResult err;

    if (pThreadData->descriptorPool == VK_NULL_HANDLE) {
        //Create a pool with the amount of descriptors we require
        VkDescriptorPoolSize poolSize[2] = {};

        poolSize[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        poolSize[0].descriptorCount = 1;

        poolSize[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSize[1].descriptorCount = 1;

        VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
        descriptorPoolCreateInfo.sType                  = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolCreateInfo.pNext                  = nullptr;
        descriptorPoolCreateInfo.maxSets                = 1;
        descriptorPoolCreateInfo.poolSizeCount          = 2;
        descriptorPoolCreateInfo.pPoolSizes             = poolSize;
        descriptorPoolCreateInfo.flags                  = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

        err = vkCreateDescriptorPool(pThreadData->device, &descriptorPoolCreateInfo, NULL, &pThreadData->descriptorPool);
        VK_CHECK(!err);
    }
    else
    {
        vkResetDescriptorPool(pThreadData->device, pThreadData->descriptorPool, 0);
        pThreadData->descriptorSet = VK_NULL_HANDLE;
    }

    if (pThreadData->descriptorSet == VK_NULL_HANDLE)
    {
        VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
        descriptorSetAllocateInfo.sType                 = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        descriptorSetAllocateInfo.pNext                 = nullptr;
        descriptorSetAllocateInfo.descriptorPool        = pThreadData->descriptorPool;
        descriptorSetAllocateInfo.descriptorSetCount    = 1;
        descriptorSetAllocateInfo.pSetLayouts           = &pThreadData->descriptorLayout;
        err = vkAllocateDescriptorSets(pThreadData->device, &descriptorSetAllocateInfo, &pThreadData->descriptorSet);
        VK_CHECK(!err);
    }
}

///////////////////////////////////////////////////////////////////////////////

void UpdateDescriptorSet(ThreadData* pThreadData)
{
    // Update the descriptor set
    VkDescriptorImageInfo descriptorImageInfo = {};

    descriptorImageInfo.sampler                     = pThreadData->pSample->GetTextureObject(pThreadData->textureIdx)->GetSampler();
    descriptorImageInfo.imageView                   = pThreadData->pSample->GetTextureObject(pThreadData->textureIdx)->GetView();
    descriptorImageInfo.imageLayout                 = pThreadData->pSample->GetTextureObject(pThreadData->textureIdx)->GetLayout();

    VkWriteDescriptorSet writes[2] = {};

    writes[0].sType                                 = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[0].dstBinding                            = 0;
    writes[0].dstSet                                = pThreadData->descriptorSet;
    writes[0].descriptorCount                       = 1;
    writes[0].descriptorType                        = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    writes[0].pBufferInfo                           = &pThreadData->pModelViewMatrixUniform->bufferInfo;

    writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[1].dstBinding                            = 1;
    writes[1].dstSet                                = pThreadData->descriptorSet;
    writes[1].descriptorCount                       = 1;
    writes[1].descriptorType                        = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writes[1].pImageInfo                            = &descriptorImageInfo;

    vkUpdateDescriptorSets(pThreadData->device, 2, &writes[0], 0, nullptr);
}

///////////////////////////////////////////////////////////////////////////////

void InitPipeline(ThreadData* pThreadData)
{
    if (pThreadData->pipeline != VK_NULL_HANDLE)
    {
        return;
    }

    VkResult   err;

    // Init the pipeline containing our cube vertex and fragment shaders
    pthread_mutex_lock(pThreadData->pMutex);
    VkShaderModule sh_cube_vert = pThreadData->pSample->CreateShaderModuleFromAsset("shaders/cube.vert.spv");
    VkShaderModule sh_tex_frag  = pThreadData->pSample->CreateShaderModuleFromAsset("shaders/textured.frag.spv");
    pthread_mutex_unlock(pThreadData->pMutex);

    // State for rasterization, such as polygon fill mode is defined.
    VkPipelineRasterizationStateCreateInfo rs = {};
    rs.sType                        = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rs.polygonMode                  = VK_POLYGON_MODE_FILL;
    rs.cullMode                     = VK_CULL_MODE_BACK_BIT;
    rs.frontFace                    = VK_FRONT_FACE_CLOCKWISE;
    rs.depthClampEnable             = VK_FALSE;
    rs.rasterizerDiscardEnable      = VK_FALSE;
    rs.depthBiasEnable              = VK_FALSE;
    rs.lineWidth                    = 1.0f;

    // Our vertex input is a single vertex buffer, and its layout is defined
    // in our mVertices object already. Use this when creating the pipeline.
    VkPipelineVertexInputStateCreateInfo visci = pThreadData->vertices.vi;
    pthread_mutex_lock(pThreadData->pMutex);
    pThreadData->pSample->InitPipeline(VK_NULL_HANDLE, &visci, pThreadData->pipelineLayout, pThreadData->renderPass, &rs, nullptr, sh_cube_vert, sh_tex_frag, false, VK_NULL_HANDLE, &pThreadData->pipeline);
    pthread_mutex_unlock(pThreadData->pMutex);
    vkDestroyShaderModule(pThreadData->device, sh_tex_frag, nullptr);
    vkDestroyShaderModule(pThreadData->device, sh_cube_vert, nullptr);
}

