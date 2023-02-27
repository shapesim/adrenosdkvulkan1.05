//                  Copyright (c) 2016 QUALCOMM Technologies Inc.
//                              All Rights Reserved.

#include "sample.h"

// Sample Name
#define SAMPLE_NAME "Vulkan Sample: Cube"
#define SAMPLE_VERSION 1

// The vertex buffer bind id, used as a constant in various places in the sample
#define VERTEX_BUFFER_BIND_ID 0

///////////////////////////////////////////////////////////////////////////////

VkSample::VkSample() : VkSampleFramework(SAMPLE_NAME, SAMPLE_VERSION)
{
    SetUseValidation(false);
}
///////////////////////////////////////////////////////////////////////////////

VkSample::~VkSample()
{

}

///////////////////////////////////////////////////////////////////////////////

void VkSample::WindowResize(uint32_t newWidth, uint32_t newHeight)
{
    mWidth  = newWidth/2;
    mHeight = newHeight/2;
}

///////////////////////////////////////////////////////////////////////////////

void VkSample::InitVertexBuffers()
{
    // Our vertex bufferdata is a cube, with associated vertex colors and UVs.
    const float vb[][9] = {
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
            { -1.0f, 1.0f,-1.0f,    1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f },

            { 1.0f, 1.0f,-1.0f,     1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 1.0f },
            { 1.0f,-1.0f,-1.0f,     1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 0.0f },
            { -1.0f,-1.0f,-1.0f,    1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 0.0f },

            //top
            { 1.0f, -1.0f, 1.0f,    1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f },
            { -1.0f,-1.0f,-1.0f,    1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 0.0f },
            { 1.0f, -1.0f,-1.0f,    1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 0.0f },

            { 1.0f, -1.0f, 1.0f,    1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f },
            { -1.0f,-1.0f, 1.0f,    1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 1.0f },
            { -1.0f,-1.0f,-1.0f,    1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 0.0f },

            //front
            { -1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 1.0f },
            { -1.0f,-1.0f, 1.0f,    1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 0.0f },
            { 1.0f,-1.0f, 1.0f,     1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 0.0f },

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

    VkResult   err;
    bool   pass;

    // Our mVertices member contains the types required for storing
    // and defining our vertex buffer within the graphics pipeline.
    memset(&mVertices, 0, sizeof(mVertices));

    // Create our buffer object.
    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.sType              = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.pNext              = nullptr;
    bufferCreateInfo.size               = sizeof(vb);
    bufferCreateInfo.usage              = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferCreateInfo.flags              = 0;
    err = vkCreateBuffer(mDevice, &bufferCreateInfo, nullptr, &mVertices.buf);
    VK_CHECK(!err);

    // Obtain the memory requirements for this buffer.
    VkMemoryRequirements mem_reqs;
    vkGetBufferMemoryRequirements(mDevice, mVertices.buf, &mem_reqs);
    VK_CHECK(!err);

    // And allocate memory according to those requirements.
    VkMemoryAllocateInfo memoryAllocateInfo = {};
    memoryAllocateInfo.sType            = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocateInfo.pNext            = nullptr;
    memoryAllocateInfo.allocationSize   = 0;
    memoryAllocateInfo.memoryTypeIndex  = 0;
    memoryAllocateInfo.allocationSize   = mem_reqs.size;
    pass = GetMemoryTypeFromProperties(mem_reqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &memoryAllocateInfo.memoryTypeIndex);
    VK_CHECK(pass);

    err = vkAllocateMemory(mDevice, &memoryAllocateInfo, nullptr, &mVertices.mem);
    VK_CHECK(!err);

    // Now we need to map the memory of this new allocation so the CPU can edit it.
    void *data;
    err = vkMapMemory(mDevice, mVertices.mem, 0, memoryAllocateInfo.allocationSize, 0, &data);
    VK_CHECK(!err);

    // Copy our triangle verticies and colors into the mapped memory area.
    memcpy(data, vb, sizeof(vb));

    // Unmap the memory back from the CPU.
    vkUnmapMemory(mDevice, mVertices.mem);

    // Bind our buffer to the memory.
    err = vkBindBufferMemory(mDevice, mVertices.buf, mVertices.mem, 0);
    VK_CHECK(!err);

    // The vertices need to be defined so that the pipeline understands how the
    // data is laid out. This is done by providing a VkPipelineVertexInputStateCreateInfo
    // structure with the correct information.
    mVertices.vi.sType                              = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    mVertices.vi.pNext                              = nullptr;
    mVertices.vi.vertexBindingDescriptionCount      = 1;
    mVertices.vi.vertexAttributeDescriptionCount    = 3;

    // Allocate the correct number of Binding descriptors.
    mVertices.vi_bindings = new VkVertexInputBindingDescription[mVertices.vi.vertexBindingDescriptionCount];
    mVertices.vi.pVertexBindingDescriptions = mVertices.vi_bindings;

    // Allocate the correct number of Attribute Descriptors.
    mVertices.vi_attrs = new VkVertexInputAttributeDescription[mVertices.vi.vertexAttributeDescriptionCount];
    mVertices.vi.pVertexAttributeDescriptions = mVertices.vi_attrs;

    // We bind the buffer as a whole, using the correct buffer ID.
    // This defines the stride for each element of the vertex array.
    mVertices.vi_bindings[0].binding    = VERTEX_BUFFER_BIND_ID;
    mVertices.vi_bindings[0].stride     = sizeof(vb[0]);
    mVertices.vi_bindings[0].inputRate  = VK_VERTEX_INPUT_RATE_VERTEX;

    // Within each element, we define the attributes. At location 0,
    // the vertex positions, in float3 format, with offset 0 as they are
    // first in the array structure.
    mVertices.vi_attrs[0].binding       = VERTEX_BUFFER_BIND_ID;
    mVertices.vi_attrs[0].location      = 0;
    mVertices.vi_attrs[0].format        = VK_FORMAT_R32G32B32_SFLOAT; //float3
    mVertices.vi_attrs[0].offset        = 0;

    // The second location is the vertex colors, in RGBA float4 format.
    // These appear in each element in memory after the float3 vertex
    // positions, so the offset is set accordingly.
    mVertices.vi_attrs[1].binding       = VERTEX_BUFFER_BIND_ID;
    mVertices.vi_attrs[1].location      = 1;
    mVertices.vi_attrs[1].format        = VK_FORMAT_R32G32B32A32_SFLOAT; //float4
    mVertices.vi_attrs[1].offset        = sizeof(float) * 3;

    // The third location is vertex UVs
    mVertices.vi_attrs[2].binding       = VERTEX_BUFFER_BIND_ID;
    mVertices.vi_attrs[2].location      = 2;
    mVertices.vi_attrs[2].format        = VK_FORMAT_R32G32_SFLOAT; //float2
    mVertices.vi_attrs[2].offset        = sizeof(float) * 7; //float3 pos + float4 color
}

///////////////////////////////////////////////////////////////////////////////

void VkSample::InitUniformBuffers()
{
    // the uniform in this example is a matrix in the vertex stage
    memset(&mModelViewMatrixUniform, 0, sizeof(mModelViewMatrixUniform));

    VkResult err = VK_SUCCESS;

    // Create our buffer object
    VkBufferCreateInfo bufferCreateInfo;
    bufferCreateInfo.sType              = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.pNext              = NULL;
    bufferCreateInfo.size               = sizeof(glm::mat4);
    bufferCreateInfo.usage              = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    bufferCreateInfo.flags              = 0;

    err = vkCreateBuffer(mDevice, &bufferCreateInfo, NULL, &mModelViewMatrixUniform.buf);
    assert(!err);

    // Obtain the requirements on memory for this buffer
    VkMemoryRequirements mem_reqs;
    vkGetBufferMemoryRequirements(mDevice, mModelViewMatrixUniform.buf, &mem_reqs);
    assert(!err);

    // And allocate memory according to those requirements
    VkMemoryAllocateInfo memoryAllocateInfo;
    memoryAllocateInfo.sType            = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocateInfo.pNext            = NULL;
    memoryAllocateInfo.allocationSize   = 0;
    memoryAllocateInfo.memoryTypeIndex  = 0;
    memoryAllocateInfo.allocationSize   = mem_reqs.size;
    bool pass = GetMemoryTypeFromProperties(mem_reqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &memoryAllocateInfo.memoryTypeIndex);
    assert(pass);

    // We keep the size of the allocation for remapping it later when we update contents
    mModelViewMatrixUniform.allocSize = memoryAllocateInfo.allocationSize;

    err = vkAllocateMemory(mDevice, &memoryAllocateInfo, NULL, &mModelViewMatrixUniform.mem);
    assert(!err);

    // Create our initial MVP matrix
    glm::mat4 modelViewProjection = mProjectionMatrix * mViewMatrix;

    // Now we need to map the memory of this new allocation so the CPU can edit it.
    void *data;
    err = vkMapMemory(mDevice, mModelViewMatrixUniform.mem, 0, mModelViewMatrixUniform.allocSize, 0, &data);
    assert(!err);

    // Copy our verticies and colors into the mapped memory area
    memcpy(data, &modelViewProjection, sizeof(modelViewProjection));

    // Unmap the memory back from the CPU
    vkUnmapMemory(mDevice, mModelViewMatrixUniform.mem);

    // Bind our buffer to the memory
    err = vkBindBufferMemory(mDevice, mModelViewMatrixUniform.buf, mModelViewMatrixUniform.mem, 0);
    assert(!err);

    mModelViewMatrixUniform.bufferInfo.buffer = mModelViewMatrixUniform.buf;
    mModelViewMatrixUniform.bufferInfo.offset = 0;
    mModelViewMatrixUniform.bufferInfo.range = sizeof(glm::mat4);
}

///////////////////////////////////////////////////////////////////////////////

void VkSample::InitTextures()
{
    bool success = true;
    success |= TextureObject::FromKTXFile(this, "bricks.ktx", &mTexBricks);
    assert(success);
}

///////////////////////////////////////////////////////////////////////////////

void VkSample::InitLayouts()
{
    VkResult ret = VK_SUCCESS;

    // This sample has two  bindings, a sampler in the fragment shader and a uniform in the
    // vertex shader for MVP matrix.
    VkDescriptorSetLayoutBinding uniformAndSamplerBinding[2] = {};

    // Our MVP matrix
    uniformAndSamplerBinding[0].binding = 0;
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

    ret = vkCreateDescriptorSetLayout(mDevice, &descriptorSetLayoutCreateInfo, nullptr, &mDescriptorLayout);
    VK_CHECK(!ret);

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
    pipelineLayoutCreateInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.pNext                  = nullptr;
    pipelineLayoutCreateInfo.setLayoutCount         = 1;
    pipelineLayoutCreateInfo.pSetLayouts            = &mDescriptorLayout;
    ret = vkCreatePipelineLayout(mDevice, &pipelineLayoutCreateInfo, nullptr, &mPipelineLayout);
    VK_CHECK(!ret);
}

///////////////////////////////////////////////////////////////////////////////

void VkSample::InitRenderPass()
{
    // The renderpass defines the attachments to the framebuffer object that gets
    // used in the pipeline. We have two attachments, the colour buffer, and the
    // depth buffer. The operations and layouts are set to defaults for this type
    // of attachment.
    VkAttachmentDescription attachmentDescriptions[2] = {};
    attachmentDescriptions[0].flags                 = 0;
    attachmentDescriptions[0].format                = mSurfaceFormat.format;
    attachmentDescriptions[0].samples               = VK_SAMPLE_COUNT_1_BIT;
    attachmentDescriptions[0].loadOp                = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachmentDescriptions[0].storeOp               = VK_ATTACHMENT_STORE_OP_STORE;
    attachmentDescriptions[0].stencilLoadOp         = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachmentDescriptions[0].stencilStoreOp        = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachmentDescriptions[0].initialLayout         = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    attachmentDescriptions[0].finalLayout           = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    attachmentDescriptions[1].flags                 = 0;
    attachmentDescriptions[1].format                = mDepthBuffers[0].format;
    attachmentDescriptions[1].samples               = VK_SAMPLE_COUNT_1_BIT;
    attachmentDescriptions[1].loadOp                = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachmentDescriptions[1].storeOp               = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachmentDescriptions[1].stencilLoadOp         = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachmentDescriptions[1].stencilStoreOp        = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachmentDescriptions[1].initialLayout         = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    attachmentDescriptions[1].finalLayout           = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    // We have references to the attachment offsets, stating the layout type.
    VkAttachmentReference colorReference = {};
    colorReference.attachment       = 0;
    colorReference.layout           = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthReference = {};
    depthReference.attachment       = 1;
    depthReference.layout           = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    // There can be multiple subpasses in a renderpass, but this example has only one.
    // We set the color and depth references at the grahics bind point in the pipeline.
    VkSubpassDescription subpassDescription = {};
    subpassDescription.pipelineBindPoint            = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.flags                        = 0;
    subpassDescription.inputAttachmentCount         = 0;
    subpassDescription.pInputAttachments            = nullptr;
    subpassDescription.colorAttachmentCount         = 1;
    subpassDescription.pColorAttachments            = &colorReference;
    subpassDescription.pResolveAttachments          = nullptr;
    subpassDescription.pDepthStencilAttachment      = &depthReference;
    subpassDescription.preserveAttachmentCount      = 0;
    subpassDescription.pPreserveAttachments         = nullptr;

    // The renderpass itself is created with the number of subpasses, and the
    // list of attachments which those subpasses can reference.
    VkRenderPassCreateInfo renderPassCreateInfo = {};
    renderPassCreateInfo.sType                      = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.pNext                      = nullptr;
    renderPassCreateInfo.attachmentCount            = 2;
    renderPassCreateInfo.pAttachments               = attachmentDescriptions;
    renderPassCreateInfo.subpassCount               = 1;
    renderPassCreateInfo.pSubpasses                 = &subpassDescription;
    renderPassCreateInfo.dependencyCount            = 0;
    renderPassCreateInfo.pDependencies              = nullptr;

    VkResult ret;
    ret = vkCreateRenderPass(mDevice, &renderPassCreateInfo, nullptr, &mRenderPass);
    VK_CHECK(!ret);
}

///////////////////////////////////////////////////////////////////////////////

void VkSample::InitPipelines()
{
    VkResult   err;

    // Init the pipeline containing our cube vertex and fragment shaders
    VkShaderModule sh_cube_vert = CreateShaderModuleFromAsset("shaders/cube.vert.spv");
    VkShaderModule sh_tex_frag  = CreateShaderModuleFromAsset("shaders/textured.frag.spv");

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
    VkPipelineVertexInputStateCreateInfo visci = mVertices.vi;
    VkSampleFramework::InitPipeline(VK_NULL_HANDLE, &visci, mPipelineLayout, mRenderPass, &rs, nullptr, sh_cube_vert, sh_tex_frag, false, VK_NULL_HANDLE, &mPipeline);

    vkDestroyShaderModule(mDevice, sh_tex_frag, nullptr);
    vkDestroyShaderModule(mDevice, sh_cube_vert, nullptr);
}

///////////////////////////////////////////////////////////////////////////////

void VkSample::InitDescriptorSet()
{
    //Create a pool with the amount of descriptors we require
    VkDescriptorPoolSize poolSize[2] = {};

    poolSize[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    poolSize[0].descriptorCount = 1;

    poolSize[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSize[1].descriptorCount = 1;

    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
    descriptorPoolCreateInfo.sType          = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolCreateInfo.pNext          = nullptr;
    descriptorPoolCreateInfo.maxSets        = 1;
    descriptorPoolCreateInfo.poolSizeCount  = 2;
    descriptorPoolCreateInfo.pPoolSizes     = poolSize;

    VkResult  err;
    err = vkCreateDescriptorPool(mDevice, &descriptorPoolCreateInfo, NULL, &mDescriptorPool);
    VK_CHECK(!err);

    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
    descriptorSetAllocateInfo.sType                 = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorSetAllocateInfo.pNext                 = nullptr;
    descriptorSetAllocateInfo.descriptorPool        = mDescriptorPool;
    descriptorSetAllocateInfo.descriptorSetCount    = 1;
    descriptorSetAllocateInfo.pSetLayouts           = &mDescriptorLayout;

    err = vkAllocateDescriptorSets(mDevice, &descriptorSetAllocateInfo, &mDescriptorSet);
    VK_CHECK(!err);

    VkDescriptorImageInfo descriptorImageInfo = {};

    descriptorImageInfo.sampler                     = mTexBricks.GetSampler();
    descriptorImageInfo.imageView                   = mTexBricks.GetView();
    descriptorImageInfo.imageLayout                 = mTexBricks.GetLayout();

    VkWriteDescriptorSet writes[2] = {};

    writes[0].sType                                 = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[0].dstBinding                            = 0;
    writes[0].dstSet                                = mDescriptorSet;
    writes[0].descriptorCount                       = 1;
    writes[0].descriptorType                        = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    writes[0].pBufferInfo                           = &mModelViewMatrixUniform.bufferInfo;

    writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[1].dstBinding                            = 1;
    writes[1].dstSet                                = mDescriptorSet;
    writes[1].descriptorCount                       = 1;
    writes[1].descriptorType                        = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writes[1].pImageInfo                            = &descriptorImageInfo;

    vkUpdateDescriptorSets(mDevice, 2, &writes[0], 0, nullptr);
}

///////////////////////////////////////////////////////////////////////////////

void VkSample::InitFrameBuffers()
{
    //The framebuffer objects reference the renderpass, and allow
    // the references defined in that renderpass to now attach to views.
    // The views in this example are the colour view, which is our swapchain image,
    // and the depth buffer created manually earlier.
    VkImageView attachments[2] = {};
    VkFramebufferCreateInfo framebufferCreateInfo = {};
    framebufferCreateInfo.sType                     = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferCreateInfo.pNext                     = nullptr;
    framebufferCreateInfo.renderPass                = mRenderPass;
    framebufferCreateInfo.attachmentCount           = 2;
    framebufferCreateInfo.pAttachments              = attachments;
    framebufferCreateInfo.width                     = mWidth;
    framebufferCreateInfo.height                    = mHeight;
    framebufferCreateInfo.layers                    = 1;

    VkResult ret;

    mFrameBuffers = new VkFramebuffer[mSwapchainImageCount];
    // Reusing the framebufferCreateInfo to create mSwapchainImageCount framebuffers,
    // only the attachments to the relevent image views change each time.
    for (uint32_t i = 0; i < mSwapchainImageCount; i++)
    {
        attachments[0] = mSwapchainBuffers[i].view;
        attachments[1] = mDepthBuffers[i].view;

        ret = vkCreateFramebuffer(mDevice, &framebufferCreateInfo, nullptr, &mFrameBuffers[i]);
        VK_CHECK(!ret);
    }
}

///////////////////////////////////////////////////////////////////////////////

void VkSample::BuildCmdBuffer()
{
    // For the triangle sample, we pre-record our command buffer, as it is static.
    // We have a buffer per swap chain image, so loop over the creation process.
    for (uint32_t i = 0; i < mSwapchainImageCount; i++)
    {
        VkCommandBuffer &cmdBuffer = mSwapchainBuffers[i].cmdBuffer;

        // vkBeginCommandBuffer should reset the command buffer, but Reset can be called
        // to make it more explicit.
        VkResult err;
        err = vkResetCommandBuffer(cmdBuffer, 0);
        VK_CHECK(!err);

        VkCommandBufferInheritanceInfo cmd_buf_hinfo = {};
        cmd_buf_hinfo.sType                 = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
        cmd_buf_hinfo.pNext                 = nullptr;
        cmd_buf_hinfo.renderPass            = VK_NULL_HANDLE;
        cmd_buf_hinfo.subpass               = 0;
        cmd_buf_hinfo.framebuffer           = VK_NULL_HANDLE;
        cmd_buf_hinfo.occlusionQueryEnable  = VK_FALSE;
        cmd_buf_hinfo.queryFlags            = 0;
        cmd_buf_hinfo.pipelineStatistics    = 0;

        VkCommandBufferBeginInfo cmd_buf_info = {};
        cmd_buf_info.sType                  = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        cmd_buf_info.pNext                  = nullptr;
        cmd_buf_info.flags                  = 0;
        cmd_buf_info.pInheritanceInfo       = &cmd_buf_hinfo;

        // By calling vkBeginCommandBuffer, cmdBuffer is put into the recording state.
        err = vkBeginCommandBuffer(cmdBuffer, &cmd_buf_info);
        VK_CHECK(!err);

        // Before we can use the back buffer from the swapchain, we must change the
        // image layout from the PRESENT mode to the COLOR_ATTACHMENT mode.
        // PRESENT mode is optimal for sending to the screen for users to see, so the
        // image will be set back to that mode after we have completed rendering.
        SetImageLayout(mSwapchainBuffers[i].image,
                       cmdBuffer,
                       VK_IMAGE_ASPECT_COLOR_BIT,
                       VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                       VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                       VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                       VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT);
        SetImageLayout(mDepthBuffers[i].image,
                       cmdBuffer,
                       VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT,
                       VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                       VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                       VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                       VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT);

        // When starting the render pass, we can set clear values.
        VkClearValue clear_values[2] = {};
        clear_values[0].color.float32[0]        = 0.3f;
        clear_values[0].color.float32[1]        = 0.3f;
        clear_values[0].color.float32[2]        = 0.3f;
        clear_values[0].color.float32[3]        = 1.0f;
        clear_values[1].depthStencil.depth      = 1.0f;
        clear_values[1].depthStencil.stencil    = 0;

        VkRenderPassBeginInfo rp_begin = {};
        rp_begin.sType                          = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        rp_begin.pNext                          = nullptr;
        rp_begin.renderPass                     = mRenderPass;
        rp_begin.framebuffer                    = mFrameBuffers[i];
        rp_begin.renderArea.offset.x            = 0;
        rp_begin.renderArea.offset.y            = 0;
        rp_begin.renderArea.extent.width        = mWidth;
        rp_begin.renderArea.extent.height       = mHeight;
        rp_begin.clearValueCount                = 2;
        rp_begin.pClearValues                   = clear_values;

        vkCmdBeginRenderPass(cmdBuffer, &rp_begin, VK_SUBPASS_CONTENTS_INLINE);

        // Set our pipeline. This holds all major state
        // the pipeline defines, for example, that the vertex buffer is a triangle list.
        vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline);

        //bind out descriptor set, which handles our uniforms and samplers
        uint32_t dynamicOffsets[1] = {0};
        vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout, 0, 1, &mDescriptorSet, 1, dynamicOffsets);

        // Bind our vertex buffer, with a 0 offset.
        VkDeviceSize offsets[1] = {0};
        vkCmdBindVertexBuffers(cmdBuffer, VERTEX_BUFFER_BIND_ID, 1, &mVertices.buf, offsets);

        // Issue a draw command, with our vertices. 12 triangles of 3 vertices.
        vkCmdDraw(cmdBuffer, 3*12, 1, 0, 0);

        // Now our render pass has ended.
        vkCmdEndRenderPass(cmdBuffer);

        // As stated earlier, now transition the swapchain image to the PRESENT mode.
        SetImageLayout(mSwapchainBuffers[i].image,
                       cmdBuffer,
                       VK_IMAGE_ASPECT_COLOR_BIT,
                       VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                       VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                       VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                       VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);
        SetImageLayout(mDepthBuffers[i].image,
                       cmdBuffer,
                       VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT,
                       VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                       VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                       VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                       VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);

        // By ending the command buffer, it is put out of record mode.
        err = vkEndCommandBuffer(cmdBuffer);
        VK_CHECK(!err);
    }
}

///////////////////////////////////////////////////////////////////////////////

bool VkSample::InitSample()
{
    float aspect = (float) mWidth / (float) mHeight;
    mProjectionMatrix = glm::perspectiveRH(glm::radians(45.0f), aspect, 0.1f, 100.0f );

    // Initialize our matrices
    glm::vec3 position(3.0f, 2.5f, 5.0f);
    glm::vec3 lookat(0.0f, 0.0f, 0.0f);
    glm::vec3 up(0.0f, 1.0f, 0.0f);

    mViewMatrix = glm::lookAtRH(position, lookat, up);
    mModelMatrix = glm::mat4();


    InitVertexBuffers();
    InitUniformBuffers();
    InitTextures();
    InitLayouts();
    InitDescriptorSet();
    InitRenderPass();
    InitPipelines();

    InitFrameBuffers();


    // Initialize our command buffers
    BuildCmdBuffer();
}

///////////////////////////////////////////////////////////////////////////////

bool VkSample::Draw()
{
    const VkPipelineStageFlags WaitDstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    VkSubmitInfo submitInfo = {};
    submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext                = nullptr;
    submitInfo.waitSemaphoreCount   = 1;
    submitInfo.pWaitSemaphores      = &mBackBufferSemaphore;
    submitInfo.pWaitDstStageMask    = &WaitDstStageMask;
    submitInfo.commandBufferCount   = 1;
    submitInfo.pCommandBuffers      = &mSwapchainBuffers[mSwapchainCurrentIdx].cmdBuffer;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores    = &mRenderCompleteSemaphore;

    VkResult err;
    err = vkQueueSubmit(mQueue, 1, &submitInfo,  VK_NULL_HANDLE);
    VK_CHECK(!err);
}

///////////////////////////////////////////////////////////////////////////////

bool VkSample::Update()
{
    UpdateUniforms();
}

///////////////////////////////////////////////////////////////////////////////

void VkSample::UpdateUniforms()
{
    static float spinAngle = 0.01f;

    glm::mat4 rotationx = glm::rotate(glm::mat4(), glm::radians(spinAngle*3), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 rotationy = glm::rotate(glm::mat4(), glm::radians(spinAngle), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 rotation = rotationx * rotationy;
    glm::mat4 model = mModelMatrix * rotation;
    glm::mat4 modelView = mViewMatrix * model;
    glm::mat4 modelViewProjection = mProjectionMatrix * modelView;

    VkResult ret = VK_SUCCESS;
    uint8_t *pData;

    ret = vkMapMemory(mDevice, mModelViewMatrixUniform.mem, 0, mModelViewMatrixUniform.allocSize, 0, (void **) &pData);
    assert(!ret);

    memcpy(pData, (const void*) &modelViewProjection, sizeof(modelViewProjection));

    vkUnmapMemory(mDevice, mModelViewMatrixUniform.mem);

    spinAngle += 0.8f;
}

///////////////////////////////////////////////////////////////////////////////

bool VkSample::DestroySample()
{
    vkDestroyPipeline(mDevice, mPipeline, nullptr);

    vkDestroyPipelineLayout(mDevice, mPipelineLayout, nullptr);

    vkDestroyRenderPass(mDevice, mRenderPass, nullptr);
    vkDestroyDescriptorSetLayout(mDevice, mDescriptorLayout, nullptr);

    // Destroy pool and all sets allocated from it.
    vkDestroyDescriptorPool(mDevice, mDescriptorPool,nullptr);
}

///////////////////////////////////////////////////////////////////////////////
// Begin Android Glue entry point


// Shared state for our app.
struct engine
{
    struct android_app* app;
    int animating;
    VkSampleFramework* sample;
};


// Process the next main command.
static void engine_handle_cmd(struct android_app* app, int32_t cmd)
{
    struct engine* engine = (struct engine*)app->userData;
    switch (cmd)
    {
        case APP_CMD_SAVE_STATE:
            // Teardown, and recreate each time
            engine->animating = 0;
            engine->sample->TearDown();
            break;
        case APP_CMD_INIT_WINDOW:
        {
            if(!engine->sample->Initialize(engine->app->window))
            {
                LOGE("VkSample::Initialize Error");
                engine->sample->TearDown();
            }
            else
            {
                LOGI("VkSample::Initialize Success");
            }
            engine->animating = 1;
            break;
        }

        case APP_CMD_TERM_WINDOW:
            engine->sample->TearDown();
            break;
        case APP_CMD_GAINED_FOCUS:
            engine->animating = 1;
            break;
        case APP_CMD_LOST_FOCUS:
            engine->animating = 0;
            break;
    }
}

// This is the main entry point of a native application that is using
// android_native_app_glue.  It runs in its own thread, with its own
// event loop for receiving input events and doing other things.
void android_main(struct android_app* state)
{
    struct engine engine;
    memset(&engine, 0, sizeof(engine));

    // Make sure glue isn't stripped.
    app_dummy();

    engine.sample = new VkSample();

    assert(engine.sample);

    state->userData = &engine;
    state->onAppCmd = engine_handle_cmd;
    engine.app = state;
    // Give the assetManager instance to the sample, as to allow it
    // to load assets such as shaders and images from our project.
    engine.sample->SetAssetManager(state->activity->assetManager);

    // loop waiting for stuff to do.
    while (1)
    {
        int ident;
        int events;
        struct android_poll_source* source;

        // If not animating, we will block forever waiting for events.
        // If animating, we loop until all events are read, then continue
        // to draw the next frame of animation.
        while ((ident=ALooper_pollAll(engine.animating ? 0 : -1, nullptr, &events,
                                      (void**)&source)) >= 0)
        {
            // Process this event.
            if (source != nullptr)
            {
                source->process(state, source);
            }

            // Check if we are exiting.
            if (state->destroyRequested != 0)
            {
                engine.sample->TearDown();
                delete engine.sample;
                return;
            }
        }

        if (engine.animating && engine.sample->IsInitialized())
        {
            engine.sample->DrawFrame();
        }
    }
}
//END Android Glue
///////////////////////////////////////////////////////////////////////////////