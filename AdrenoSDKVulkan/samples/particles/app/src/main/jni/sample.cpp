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

// Our particle vertex bufferdata is a quad, with associated vertex colors and UVs.
const float vb[][10] = {
        //      position             color                   uvs
        {-1.0f, -1.0f,  0.0f,   1.0f, 1.0f, 1.0f, 1.0f,   0.0f, 0.0f, 0.0f},
        {-1.0f,  1.0f,  0.0f,   1.0f, 1.0f, 1.0f, 1.0f,   1.0f, 0.0f, 0.0f},
        { 1.0f,  1.0f,  0.0f,   1.0f, 1.0f, 1.0f, 1.0f,   1.0f, 1.0f, 0.0f},

        {-1.0f, -1.0f,  0.0f,   1.0f, 1.0f, 1.0f, 1.0f,   0.0f, 0.0f, 0.0f},
        { 1.0f,  1.0f,  0.0f,   1.0f, 1.0f, 1.0f, 1.0f,   1.0f, 1.0f, 0.0f},
        { 1.0f, -1.0f,  0.0f,   1.0f, 1.0f, 1.0f, 1.0f,   0.0f, 1.0f, 0.0f},
};

// Our vertex bufferdata is a quad, with associated vertex colors and UVs.
// We'll tile the texture a bit using the UV coordinates for some detail
const float floorVB[][9] = {
        //      position                       color                   uvs
        {-FLOOR_SIZE,  0.0f,  -FLOOR_SIZE,   1.0f, 1.0f, 1.0f, 1.0f,   0.0f, 0.0f},
        {-FLOOR_SIZE,  0.0f,   FLOOR_SIZE,   1.0f, 1.0f, 1.0f, 1.0f,   2.0f, 0.0f},
        { FLOOR_SIZE,  0.0f,   FLOOR_SIZE,   1.0f, 1.0f, 1.0f, 1.0f,   2.0f, 2.0f},

        {-FLOOR_SIZE, -0.0f,  -FLOOR_SIZE,   1.0f, 1.0f, 1.0f, 1.0f,   0.0f, 0.0f},
        { FLOOR_SIZE,  0.0f,   FLOOR_SIZE,   1.0f, 1.0f, 1.0f, 1.0f,   2.0f, 2.0f},
        { FLOOR_SIZE,  0.0f,  -FLOOR_SIZE,   1.0f, 1.0f, 1.0f, 1.0f,   0.0f, 2.0f},
};


///////////////////////////////////////////////////////////////////////////////

void VkSample::InitVertexBuffers()
{
    InitParticleVertexBuffer();
    InitFloorVertexBuffer();
}

///////////////////////////////////////////////////////////////////////////////

void VkSample::InitFloorVertexBuffer()
{
    VkResult   err;
    bool   pass;

    // Our mFloorVertices member contains the types required for storing
    // and defining our floor vertex buffer within the graphics pipeline.
    memset(&mFloorVertices, 0, sizeof(mFloorVertices));

    // Create our buffer object.
    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.sType              = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.pNext              = nullptr;
    bufferCreateInfo.size               = sizeof(floorVB);
    bufferCreateInfo.usage              = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferCreateInfo.flags              = 0;
    err = vkCreateBuffer(mDevice, &bufferCreateInfo, nullptr, &mFloorVertices.buf);
    VK_CHECK(!err);

    // Obtain the memory requirements for this buffer.
    VkMemoryRequirements mem_reqs;
    vkGetBufferMemoryRequirements(mDevice, mFloorVertices.buf, &mem_reqs);
    VK_CHECK(!err);

    // Remember the memory size
    mFloorVertices.allocSize = mem_reqs.size;

    // And allocate memory according to those requirements.
    VkMemoryAllocateInfo memoryAllocateInfo = {};
    memoryAllocateInfo.sType            = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocateInfo.pNext            = nullptr;
    memoryAllocateInfo.allocationSize   = 0;
    memoryAllocateInfo.memoryTypeIndex  = 0;
    memoryAllocateInfo.allocationSize   = mem_reqs.size;
    pass = GetMemoryTypeFromProperties(mem_reqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &memoryAllocateInfo.memoryTypeIndex);
    VK_CHECK(pass);
    err = vkAllocateMemory(mDevice, &memoryAllocateInfo, nullptr, &mFloorVertices.mem);
    VK_CHECK(!err);

    // Now we need to map the memory of this new allocation so the CPU can edit it.
    void *data;
    err = vkMapMemory(mDevice, mFloorVertices.mem, 0, memoryAllocateInfo.allocationSize, 0, &data);
    VK_CHECK(!err);

    memcpy(data, floorVB , sizeof(floorVB));

    // Unmap the memory back from the CPU.
    vkUnmapMemory(mDevice, mFloorVertices.mem);

    // Bind our buffer to the memory.
    err = vkBindBufferMemory(mDevice, mFloorVertices.buf, mFloorVertices.mem, 0);
    VK_CHECK(!err);

    // The vertices need to be defined so that the pipeline understands how the
    // data is laid out. This is done by providing a VkPipelineVertexInputStateCreateInfo
    // structure with the correct information.
    mFloorVertices.vi.sType                              = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    mFloorVertices.vi.pNext                              = nullptr;
    mFloorVertices.vi.vertexBindingDescriptionCount      = 1;
    mFloorVertices.vi.vertexAttributeDescriptionCount    = 3;

    // Allocate the correct number of Binding descriptors.
    mFloorVertices.vi_bindings = new VkVertexInputBindingDescription[mFloorVertices.vi.vertexBindingDescriptionCount];
    mFloorVertices.vi.pVertexBindingDescriptions = mFloorVertices.vi_bindings;

    // Allocate the correct number of Attribute Descriptors.
    mFloorVertices.vi_attrs = new VkVertexInputAttributeDescription[mFloorVertices.vi.vertexAttributeDescriptionCount];
    mFloorVertices.vi.pVertexAttributeDescriptions = mFloorVertices.vi_attrs;


    // We bind the buffer as a whole, using the correct buffer ID.
    // This defines the stride for each element of the vertex array.
    mFloorVertices.vi_bindings[0].binding    = VERTEX_BUFFER_BIND_ID;
    mFloorVertices.vi_bindings[0].stride     = sizeof(floorVB[0]);
    mFloorVertices.vi_bindings[0].inputRate  = VK_VERTEX_INPUT_RATE_VERTEX;

    // Within each element, we define the attributes. At location 0,
    // the vertex positions, in float3 format, with offset 0 as they are
    // first in the array structure.
    mFloorVertices.vi_attrs[0].binding       = VERTEX_BUFFER_BIND_ID;
    mFloorVertices.vi_attrs[0].location      = 0;
    mFloorVertices.vi_attrs[0].format        = VK_FORMAT_R32G32B32_SFLOAT; //float3
    mFloorVertices.vi_attrs[0].offset        = 0;

    // The second location is the vertex colors, in RGBA float4 format.
    // These appear in each element in memory after the float3 vertex
    // positions, so the offset is set accordingly.
    mFloorVertices.vi_attrs[1].binding       = VERTEX_BUFFER_BIND_ID;
    mFloorVertices.vi_attrs[1].location      = 1;
    mFloorVertices.vi_attrs[1].format        = VK_FORMAT_R32G32B32A32_SFLOAT; //float4
    mFloorVertices.vi_attrs[1].offset        = sizeof(float) * 3;

    // The third location is vertex UVSs
    mFloorVertices.vi_attrs[2].binding       = VERTEX_BUFFER_BIND_ID;
    mFloorVertices.vi_attrs[2].location      = 2;
    mFloorVertices.vi_attrs[2].format        = VK_FORMAT_R32G32_SFLOAT; //float2
    mFloorVertices.vi_attrs[2].offset        = sizeof(float) * 7; //float3 pos + float4 color
}

///////////////////////////////////////////////////////////////////////////////

void VkSample::InitParticleVertexBuffer()
{
    VkResult   err;
    bool   pass;

    // Our mParticleVertices member contains the types required for storing
    // and defining our particle vertex buffer within the graphics pipeline.
    memset(&mParticleVertices, 0, sizeof(mParticleVertices));

    // Create our buffer object.
    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.sType              = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.pNext              = nullptr;
    bufferCreateInfo.size               = sizeof(vb)* NUM_PARTICLES;
    bufferCreateInfo.usage              = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferCreateInfo.flags              = 0;
    err = vkCreateBuffer(mDevice, &bufferCreateInfo, nullptr, &mParticleVertices.buf);
    VK_CHECK(!err);

    // Obtain the memory requirements for this buffer.
    VkMemoryRequirements mem_reqs;
    vkGetBufferMemoryRequirements(mDevice, mParticleVertices.buf, &mem_reqs);
    VK_CHECK(!err);

    // Remember the memory size
    mParticleVertices.allocSize = mem_reqs.size;

    // And allocate memory according to those requirements.
    VkMemoryAllocateInfo memoryAllocateInfo = {};
    memoryAllocateInfo.sType            = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocateInfo.pNext            = nullptr;
    memoryAllocateInfo.allocationSize   = 0;
    memoryAllocateInfo.memoryTypeIndex  = 0;
    memoryAllocateInfo.allocationSize   = mem_reqs.size;
    pass = GetMemoryTypeFromProperties(mem_reqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &memoryAllocateInfo.memoryTypeIndex);
    VK_CHECK(pass);
    err = vkAllocateMemory(mDevice, &memoryAllocateInfo, nullptr, &mParticleVertices.mem);
    VK_CHECK(!err);

    // Now we need to map the memory of this new allocation so the CPU can edit it.
    void *data;
    err = vkMapMemory(mDevice, mParticleVertices.mem, 0, memoryAllocateInfo.allocationSize, 0, &data);
    VK_CHECK(!err);

    // Copy the particle vertices
    for (int p=0; p< NUM_PARTICLES; p++)
    {
        memcpy(data+p*sizeof(vb), vb, sizeof(vb));
    }

    // Unmap the memory back from the CPU.
    vkUnmapMemory(mDevice, mParticleVertices.mem);

    // Update the VB with current particle data
    UpdateParticleVertexBuffers();

    // Bind our buffer to the memory.
    err = vkBindBufferMemory(mDevice, mParticleVertices.buf, mParticleVertices.mem, 0);
    VK_CHECK(!err);

    // The vertices need to be defined so that the pipeline understands how the
    // data is laid out. This is done by providing a VkPipelineVertexInputStateCreateInfo
    // structure with the correct information.
    mParticleVertices.vi.sType                              = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    mParticleVertices.vi.pNext                              = nullptr;
    mParticleVertices.vi.vertexBindingDescriptionCount      = 1;
    mParticleVertices.vi.vertexAttributeDescriptionCount    = 3;

    // Allocate the correct number of Binding descriptors.
    mParticleVertices.vi_bindings = new VkVertexInputBindingDescription[mParticleVertices.vi.vertexBindingDescriptionCount];
    mParticleVertices.vi.pVertexBindingDescriptions = mParticleVertices.vi_bindings;

    // Allocate the correct number of Attribute Descriptors.
    mParticleVertices.vi_attrs = new VkVertexInputAttributeDescription[mParticleVertices.vi.vertexAttributeDescriptionCount];
    mParticleVertices.vi.pVertexAttributeDescriptions = mParticleVertices.vi_attrs;

    // We bind the buffer as a whole, using the correct buffer ID.
    // This defines the stride for each element of the vertex array.
    mParticleVertices.vi_bindings[0].binding    = VERTEX_BUFFER_BIND_ID;
    mParticleVertices.vi_bindings[0].stride     = sizeof(vb[0]);
    mParticleVertices.vi_bindings[0].inputRate  = VK_VERTEX_INPUT_RATE_VERTEX;

    // Within each element, we define the attributes. At location 0,
    // the vertex positions, in float3 format, with offset 0 as they are
    // first in the array structure.
    mParticleVertices.vi_attrs[0].binding       = VERTEX_BUFFER_BIND_ID;
    mParticleVertices.vi_attrs[0].location      = 0;
    mParticleVertices.vi_attrs[0].format        = VK_FORMAT_R32G32B32_SFLOAT; //float3
    mParticleVertices.vi_attrs[0].offset        = 0;

    // The second location is the vertex colors, in RGBA float4 format.
    // These appear in each element in memory after the float3 vertex
    // positions, so the offset is set accordingly.
    mParticleVertices.vi_attrs[1].binding       = VERTEX_BUFFER_BIND_ID;
    mParticleVertices.vi_attrs[1].location      = 1;
    mParticleVertices.vi_attrs[1].format        = VK_FORMAT_R32G32B32A32_SFLOAT; //float4
    mParticleVertices.vi_attrs[1].offset        = sizeof(float) * 3;

    // The third location is vertex UVSs
    mParticleVertices.vi_attrs[2].binding       = VERTEX_BUFFER_BIND_ID;
    mParticleVertices.vi_attrs[2].location      = 2;
    mParticleVertices.vi_attrs[2].format        = VK_FORMAT_R32G32B32_SFLOAT; //float3
    mParticleVertices.vi_attrs[2].offset        = sizeof(float) * 7; //float3 pos + float4 color
}

///////////////////////////////////////////////////////////////////////////////

void VkSample::UpdateParticleVertexBuffers()
{
    VkResult err;
    // Now we need to map the memory of this new allocation so the CPU can edit it.
    float *pFloat;
    err = vkMapMemory(mDevice, mParticleVertices.mem, 0, mParticleVertices.allocSize, 0, (void**)&pFloat);
    VK_CHECK(!err);

    for (int p=0; p< NUM_PARTICLES; p++)
    {
        ParticleData *pData = &mpParticles[p];

        // for each vertice
        for (int v = 0; v < 6; v++)
        {
            *pFloat++ = pData->position[0];
            *pFloat++ = pData->position[1];
            *pFloat++ = pData->position[2];
            *pFloat++ = 1.0f;
            *pFloat++ = 1.0f;
            *pFloat++ = 1.0f;
            // use color alpha to control particle visibility
            *pFloat++ = pData->bAlive? 1.0f : 0.0f;
            *pFloat++ = vb[v][7];
            *pFloat++ = vb[v][8];
            *pFloat++ = (float)pData->tileID;
        }
    }

    // Unmap the memory back from the CPU.
    vkUnmapMemory(mDevice, mParticleVertices.mem);
}

///////////////////////////////////////////////////////////////////////////////

void VkSample::InitUniformBuffers()
{
    if (mUniform.buf != VK_NULL_HANDLE)
    {
        return;
    }

    // the uniform in this example is a matrix in the vertex stage
    memset(&mUniform, 0, sizeof(mUniform));

    VkResult err = VK_SUCCESS;

    // Create our buffer object
    VkBufferCreateInfo bufferCreateInfo={};
    bufferCreateInfo.sType              = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.pNext              = NULL;
    bufferCreateInfo.size               = sizeof(UniformData);
    bufferCreateInfo.usage              = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    bufferCreateInfo.flags              = 0;
    err = vkCreateBuffer(mDevice, &bufferCreateInfo, NULL, &mUniform.buf);
    assert(!err);

    // Obtain the requirements on memory for this buffer
    VkMemoryRequirements mem_reqs;
    vkGetBufferMemoryRequirements(mDevice, mUniform.buf, &mem_reqs);
    assert(!err);

    // And allocate memory according to those requirements
    VkMemoryAllocateInfo memoryAllocateInfo={};
    memoryAllocateInfo.sType            = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocateInfo.pNext            = NULL;
    memoryAllocateInfo.allocationSize   = 0;
    memoryAllocateInfo.memoryTypeIndex  = 0;
    memoryAllocateInfo.allocationSize   = mem_reqs.size;
    bool pass = GetMemoryTypeFromProperties(mem_reqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &memoryAllocateInfo.memoryTypeIndex);
    assert(pass);

    // We keep the size of the allocation for remapping it later when we update contents
    mUniform.allocSize = memoryAllocateInfo.allocationSize;

    err = vkAllocateMemory(mDevice, &memoryAllocateInfo, NULL, &mUniform.mem);
    assert(!err);

    // Bind our buffer to the memory
    err = vkBindBufferMemory(mDevice, mUniform.buf, mUniform.mem, 0);
    assert(!err);

    mUniform.bufferInfo.buffer  = mUniform.buf;
    mUniform.bufferInfo.offset  = 0;
    mUniform.bufferInfo.range   = sizeof(Uniform);
}

///////////////////////////////////////////////////////////////////////////////

void VkSample::InitTextures()
{
    mpParticleTextureObject = new TextureObject();
    bool success = true;
    success = TextureObject::FromKTXFile(this, "fire.ktx",  mpParticleTextureObject);
    assert(success);

    mTileFrameRate = 12;
    mNumTiles      = 64;
    mTileNumRows   =  8;
    mTileNumCols   =  8;

    mpFloorTextureObject = new TextureObject();
    success = TextureObject::FromKTXFile(this, "floor.ktx",  mpFloorTextureObject);
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

    ret = vkCreateDescriptorSetLayout(mDevice, &descriptorSetLayoutCreateInfo, nullptr, &mParticleDescriptorLayout);
    VK_CHECK(!ret);

    ret = vkCreateDescriptorSetLayout(mDevice, &descriptorSetLayoutCreateInfo, nullptr, &mFloorDescriptorLayout);
    VK_CHECK(!ret);

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
    pipelineLayoutCreateInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.pNext                  = nullptr;
    pipelineLayoutCreateInfo.setLayoutCount         = 1;
    pipelineLayoutCreateInfo.pSetLayouts            = &mParticleDescriptorLayout;
    ret = vkCreatePipelineLayout(mDevice, &pipelineLayoutCreateInfo, nullptr, &mParticlePipelineLayout);
    VK_CHECK(!ret);

    pipelineLayoutCreateInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.pNext                  = nullptr;
    pipelineLayoutCreateInfo.setLayoutCount         = 1;
    pipelineLayoutCreateInfo.pSetLayouts            = &mFloorDescriptorLayout;
    ret = vkCreatePipelineLayout(mDevice, &pipelineLayoutCreateInfo, nullptr, &mFloorPipelineLayout);
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

    // Init the pipeline containing our vertex and fragment shaders
    VkShaderModule sh_particle_vert  = CreateShaderModuleFromAsset("shaders/particles.vert.spv");
    VkShaderModule sh_particle_frag  = CreateShaderModuleFromAsset("shaders/particles.frag.spv");

    // State for rasterization, such as polygon fill mode is defined.
    VkPipelineRasterizationStateCreateInfo rs = {};
    rs.sType                                    = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rs.polygonMode                              = VK_POLYGON_MODE_FILL;
    rs.cullMode                                 = VK_CULL_MODE_NONE;
    rs.frontFace                                = VK_FRONT_FACE_CLOCKWISE;
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
    shaderStages[0].module                      = sh_particle_vert;
    shaderStages[0].pName                       = "main";
    shaderStages[1].sType                       = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[1].stage                       = VK_SHADER_STAGE_FRAGMENT_BIT;
    shaderStages[1].module                      = sh_particle_frag;
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
    att_state[0].srcAlphaBlendFactor            = VK_BLEND_FACTOR_ONE;
    att_state[0].dstAlphaBlendFactor            = VK_BLEND_FACTOR_ONE;

    VkPipelineColorBlendStateCreateInfo cb = {};
    cb.sType                                    = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    cb.attachmentCount                          = 1;
    cb.pAttachments                             = &att_state[0];

    // Our vertex input is a single vertex buffer, and its layout is defined
    // in our mParticleVertices object already. Use this when creating the pipeline.
    VkPipelineVertexInputStateCreateInfo visci = mParticleVertices.vi;

    InitPipeline(VK_NULL_HANDLE, 2, shaderStages,  mParticlePipelineLayout, mRenderPass, 0, &visci, nullptr, nullptr, nullptr, &rs, nullptr, &ds, &cb, nullptr, false, VK_NULL_HANDLE, &mParticlePipeline);

    vkDestroyShaderModule(mDevice, sh_particle_frag, nullptr);
    vkDestroyShaderModule(mDevice, sh_particle_vert, nullptr);

    // Init the floor pipeline containing our floor vertex and fragment shaders
    VkShaderModule sh_floor_vert  = CreateShaderModuleFromAsset("shaders/floor.vert.spv");
    VkShaderModule sh_floor_frag  = CreateShaderModuleFromAsset("shaders/floor.frag.spv");

    // State for rasterization, such as polygon fill mode is defined.
    rs.sType                                    = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rs.polygonMode                              = VK_POLYGON_MODE_FILL;
    rs.cullMode                                 = VK_CULL_MODE_NONE;
    rs.frontFace                                = VK_FRONT_FACE_CLOCKWISE;
    rs.depthClampEnable                         = VK_FALSE;
    rs.rasterizerDiscardEnable                  = VK_FALSE;
    rs.depthBiasEnable                          = VK_FALSE;
    rs.lineWidth                                = 1.0f;

    // Standard depth and stencil state is defined
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
    shaderStages[0].sType                       = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[0].stage                       = VK_SHADER_STAGE_VERTEX_BIT;
    shaderStages[0].module                      = sh_floor_vert;
    shaderStages[0].pName                       = "main";
    shaderStages[1].sType                       = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[1].stage                       = VK_SHADER_STAGE_FRAGMENT_BIT;
    shaderStages[1].module                      = sh_floor_frag;
    shaderStages[1].pName                       = "main";

    att_state[0].colorWriteMask                 = VK_COLOR_COMPONENT_R_BIT |
                                                  VK_COLOR_COMPONENT_G_BIT |
                                                  VK_COLOR_COMPONENT_B_BIT |
                                                  VK_COLOR_COMPONENT_A_BIT;
    att_state[0].blendEnable                    = VK_FALSE;
    att_state[0].srcColorBlendFactor            = VK_BLEND_FACTOR_SRC_ALPHA;
    att_state[0].dstColorBlendFactor            = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    att_state[0].alphaBlendOp                   = VK_BLEND_OP_ADD;
    att_state[0].srcAlphaBlendFactor            = VK_BLEND_FACTOR_ONE;
    att_state[0].dstAlphaBlendFactor            = VK_BLEND_FACTOR_ONE;

    cb.sType                                    = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    cb.attachmentCount                          = 1;
    cb.pAttachments                             = &att_state[0];

    // Our vertex input is a single vertex buffer, and its layout is defined
    // in our mParticleVertices object already. Use this when creating the pipeline.
    visci = mFloorVertices.vi;

    InitPipeline(VK_NULL_HANDLE, 2, shaderStages,  mFloorPipelineLayout, mRenderPass, 0, &visci, nullptr, nullptr, nullptr, &rs, nullptr, &ds, &cb, nullptr, false, VK_NULL_HANDLE, &mFloorPipeline);

    vkDestroyShaderModule(mDevice, sh_floor_frag, nullptr);
    vkDestroyShaderModule(mDevice, sh_floor_vert, nullptr);
}

///////////////////////////////////////////////////////////////////////////////

void VkSample::InitDescriptorSets()
{
    //Create a pool with the amount of descriptors we require
    VkDescriptorPoolSize poolSize[2] = {};

    poolSize[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    poolSize[0].descriptorCount = 2;

    poolSize[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSize[1].descriptorCount = 2;

    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
    descriptorPoolCreateInfo.sType          = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolCreateInfo.pNext          = nullptr;
    descriptorPoolCreateInfo.maxSets        = 2;
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
    descriptorSetAllocateInfo.pSetLayouts           = &mParticleDescriptorLayout;

    err = vkAllocateDescriptorSets(mDevice, &descriptorSetAllocateInfo, &mParticleDescriptorSet);
    VK_CHECK(!err);

    VkDescriptorImageInfo descriptorImageInfo = {};

    descriptorImageInfo.sampler                     = mpParticleTextureObject->GetSampler();
    descriptorImageInfo.imageView                   = mpParticleTextureObject->GetView();
    descriptorImageInfo.imageLayout                 = mpParticleTextureObject->GetLayout();

    VkWriteDescriptorSet writes[2] = {};

    writes[0].sType                                 = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[0].dstBinding                            = 0;
    writes[0].dstSet                                = mParticleDescriptorSet;
    writes[0].descriptorCount                       = 1;
    writes[0].descriptorType                        = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    writes[0].pBufferInfo                           = &mUniform.bufferInfo;

    writes[1].sType                                 = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[1].dstBinding                            = 1;
    writes[1].dstSet                                = mParticleDescriptorSet;
    writes[1].descriptorCount                       = 1;
    writes[1].descriptorType                        = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writes[1].pImageInfo                            = &descriptorImageInfo;

    vkUpdateDescriptorSets(mDevice, 2, &writes[0], 0, nullptr);


    descriptorSetAllocateInfo.sType                 = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorSetAllocateInfo.pNext                 = nullptr;
    descriptorSetAllocateInfo.descriptorPool        = mDescriptorPool;
    descriptorSetAllocateInfo.descriptorSetCount    = 1;
    descriptorSetAllocateInfo.pSetLayouts           = &mFloorDescriptorLayout;

    err = vkAllocateDescriptorSets(mDevice, &descriptorSetAllocateInfo, &mFloorDescriptorSet);
    VK_CHECK(!err);

    descriptorImageInfo.sampler                     = mpFloorTextureObject->GetSampler();
    descriptorImageInfo.imageView                   = mpFloorTextureObject->GetView();
    descriptorImageInfo.imageLayout                 = mpFloorTextureObject->GetLayout();

    writes[0].sType                                 = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[0].dstBinding                            = 0;
    writes[0].dstSet                                = mFloorDescriptorSet;
    writes[0].descriptorCount                       = 1;
    writes[0].descriptorType                        = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    writes[0].pBufferInfo                           = &mUniform.bufferInfo;

    writes[1].sType                                 = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[1].dstBinding                            = 1;
    writes[1].dstSet                                = mFloorDescriptorSet;
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

        // Set our floor pipeline. This holds all major state
        // the pipeline defines, for example, that the vertex buffer is a triangle list.
        vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mFloorPipeline);

        //bind out descriptor set, which handles our uniforms and samplers
        uint32_t dynamicOffsets[1] = {0};
        vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mFloorPipelineLayout, 0, 1, &mFloorDescriptorSet, 1, dynamicOffsets);

        // Bind our vertex buffer, with a 0 offset.
        VkDeviceSize offsets[1] = {0};
        vkCmdBindVertexBuffers(cmdBuffer, VERTEX_BUFFER_BIND_ID, 1, &mFloorVertices.buf, offsets);

        // Issue a draw command, with our vertices. 2 triangles of 3 vertices.
        vkCmdDraw(cmdBuffer, 6, 1, 0, 0);

        // Set our particle pipeline. This holds all major state
        // the pipeline defines, for example, that the vertex buffer is a triangle list.
        vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mParticlePipeline);

        // Bind out descriptor set, which handles our uniforms and samplers
        vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mParticlePipelineLayout, 0, 1, &mParticleDescriptorSet, 1, dynamicOffsets);

        // Bind our vertex buffer, with a 0 offset.
        vkCmdBindVertexBuffers(cmdBuffer, VERTEX_BUFFER_BIND_ID, 1, &mParticleVertices.buf, offsets);

        // Issue a draw command, with our vertices. NUM_PARTICLES*2 triangles of 3 vertices.
        vkCmdDraw(cmdBuffer, NUM_PARTICLES*6, 1, 0, 0);

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

void VkSample::UpdateParticles()
{
    // Update particles
    for (int p=0; p< NUM_PARTICLES; p++) {
        ParticleData *pData = &mpParticles[p];

        // Update alive
        if (!pData->bAlive) {
            if (pData->spawnTime < mUpdateTimeSeconds && pData->endTime > mUpdateTimeSeconds)
            {
                pData->bAlive = true;
                pData->position[0] = pData->initialPosition[0];
                pData->position[1] = pData->initialPosition[1];
                pData->position[2] = pData->initialPosition[2];

                //Initialize velocity to a random direction on floor
                pData->velocity[0] = (((float)rand()/(float)RAND_MAX)-0.5f) * MAX_VELOCITY_PER_SECOND;
                pData->velocity[1] = 0.0f;
                pData->velocity[2] = (((float)rand()/(float)RAND_MAX)-0.5f) * MAX_VELOCITY_PER_SECOND;
            }
        }
        else {
            if (pData->bAlive && pData->endTime < mUpdateTimeSeconds)
            {
                pData->bAlive = false;
            }
        }

        // Update TileID
        if (pData->bAlive) {
            float lifePer =  (mUpdateTimeSeconds - pData->spawnTime) / (pData->endTime - pData->spawnTime);
            pData->tileID = std::min(mNumTiles - 1, (uint32_t) (mNumTiles * lifePer));
        }
        else {
            pData->tileID = 0;
        }

        // Update Position
        pData->position[0] += mDeltaTimeSeconds * pData->velocity[0];
        pData->position[1] += mDeltaTimeSeconds * pData->velocity[1];
        pData->position[2] += mDeltaTimeSeconds * pData->velocity[2];
    }
    SortParticles();

    UpdateParticleVertexBuffers();
}

///////////////////////////////////////////////////////////////////////////////

void VkSample::InitializeParticle(uint32_t p)
{
    ParticleData* pData = &mpParticles[p];

    pData->particleID = p;

    // initialize position to archimedean spiral
    float s = SPIRAL_SPACING * p;
    float b = 1.0/2.0f/3.14198f;

    float thetai = pow(2.0f*s/b,0.5f);
    pData->initialPosition[0] = b * thetai * cos(thetai);
    pData->initialPosition[1] = 0.0f;
    pData->initialPosition[2] = b * thetai * sin(thetai);

    pData->cameraDistance = pow(
            (pData->position[0] - mCameraPosition[0])*(pData->position[0] - mCameraPosition[0]) +
            (pData->position[1] - mCameraPosition[1])*(pData->position[1] - mCameraPosition[1]) +
            (pData->position[2] - mCameraPosition[2])*(pData->position[2] - mCameraPosition[2]), 0.5f);

    // Initialize times
    pData->spawnTime = 0.0f;
    pData->endTime   = 0.0f;

    // Initialize spawntime
    pData->bAlive = true;
}

///////////////////////////////////////////////////////////////////////////////

int CompareParticle(const void* pParticle1, const void* pParticle2)
{
    // Sort so that furthest particles are rendered first
    if (((ParticleData*)pParticle1)->cameraDistance <  ((ParticleData*)pParticle2)->cameraDistance)
    {
        return 1;
    }
    else if (((ParticleData*)pParticle1)->cameraDistance >  ((ParticleData*)pParticle2)->cameraDistance)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

///////////////////////////////////////////////////////////////////////////////

void VkSample::SortParticles()
{
    qsort(mpParticles,NUM_PARTICLES, sizeof(ParticleData),CompareParticle);
}

///////////////////////////////////////////////////////////////////////////////

void VkSample::InitParticles()
{
    // Allocate memory for the particles..
    mpParticles = (ParticleData*) malloc(sizeof(ParticleData)*NUM_PARTICLES);

    for (int p=0; p< NUM_PARTICLES; p++)
    {
        InitializeParticle(p);
    }

    SortParticles();
}

///////////////////////////////////////////////////////////////////////////////

void VkSample::StartParticles()
{
    for (int p=0; p< NUM_PARTICLES; p++)
    {
        ParticleData* pData = &mpParticles[p];

        // Initialize spawntime
        pData->spawnTime = mUpdateTimeSeconds + pData->particleID * PARTICLE_SPAWN_DELAY;

        // Initialize endtime
        pData->endTime = pData->spawnTime + PARTICLE_MIN_LIFETIME + (PARTICLE_MAX_LIFETIME - PARTICLE_MIN_LIFETIME)*((float)rand()/(float)RAND_MAX);

        pData->bAlive = false;
    }
}

///////////////////////////////////////////////////////////////////////////////

bool VkSample::InitSample()
{
    float aspect = (float) mWidth / (float) mHeight;
    mProjectionMatrix = glm::perspectiveRH(glm::radians(80.0f), aspect, 0.1f, 100.0f );

    // Initialize our camera matrices
    mCameraPosition = glm::vec3(0.0f, 4.0f, 10.0f);
    mCameraLookAt   = glm::vec3(0.0f, 0.0f, 0.0f);
    mCameraUp       = glm::vec3(0.0f, 0.0f,1.0f);
    mViewMatrix = glm::lookAtRH(mCameraPosition, mCameraLookAt,  mCameraUp);

    // invert the view Matrix
    mInvViewMatrix = glm::inverse(mViewMatrix);
    VkResult ret;

    InitParticles();

    InitUniformBuffers();
    InitVertexBuffers();
    InitTextures();
    InitLayouts();
    InitDescriptorSets();
    InitRenderPass();
    InitPipelines();
    InitFrameBuffers();

    // Create a fences to use when submitting the primary command buffers, one per swap chain image.
    for (uint32_t i = 0; i < mSwapchainImageCount; i++)
    {
        VkFenceCreateInfo fenceCreateInfo = {};
        fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        ret = vkCreateFence(mDevice, &fenceCreateInfo, nullptr, &mSubmitFences[i]);
        mFenceSubmitted[i] = false;
    }

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
    err = vkQueueSubmit(mQueue, 1, &submitInfo,  mSubmitFences[mSwapchainCurrentIdx]);
    mFenceSubmitted[mSwapchainCurrentIdx] = true;

    VK_CHECK(!err);
}

///////////////////////////////////////////////////////////////////////////////

bool VkSample::Update()
{
    // Wait for the previous submission of the swapchain image to finish
    if (mFenceSubmitted[mSwapchainCurrentIdx])
    {
        vkWaitForFences(mDevice, 1, &mSubmitFences[mSwapchainCurrentIdx], true, WAIT_FOREVER);
        mFenceSubmitted[mSwapchainCurrentIdx] = false;
        vkResetFences(mDevice,1,&mSubmitFences[mSwapchainCurrentIdx]);
    }

    // Get the currentframe begin and delta times in seconds..
    mUpdateTimeSeconds = (float)mFrameTimeBegin * MICRO_TO_UNIT;
    mDeltaTimeSeconds  = (float)mFrameTimeDelta * MICRO_TO_UNIT;

    // Restart the particles periodically.
    if (mFrameIdx%RESTART_FRAMES==0)
    {
        StartParticles();
    }

    // Update uniforms, and particles,
    UpdateUniforms();
    UpdateParticles();
}

///////////////////////////////////////////////////////////////////////////////

void VkSample::UpdateUniforms()
{
    VkResult ret = VK_SUCCESS;
    uint8_t *pData;

    Uniform* pUniform = &mUniform;
    ret = vkMapMemory(mDevice, pUniform->mem, 0, pUniform->allocSize, 0, (void **) &pData);
    assert(!ret);

    glm::mat4 model = glm::mat4();
    glm::mat4 modelView = mViewMatrix * model;
    glm::mat4 modelViewProjection = mProjectionMatrix * modelView;

    UniformData udata;
    udata.modelviewprojectionMatrix = modelViewProjection;
      udata.inverseCameraMatrix       = mInvViewMatrix;
    udata.tileCols                  = mTileNumCols;
    udata.tileRows                  = mTileNumRows;

    // copy uniform data to the buffer
    memcpy(pData,  (const void*) &udata, sizeof(UniformData));

    vkUnmapMemory(mDevice, mUniform.mem);
}

///////////////////////////////////////////////////////////////////////////////

bool VkSample::DestroySample()
{
    vkDestroyPipeline(mDevice, mParticlePipeline, nullptr);
    vkDestroyPipeline(mDevice, mFloorPipeline, nullptr);

    vkDestroyPipelineLayout(mDevice, mParticlePipelineLayout, nullptr);
    vkDestroyPipelineLayout(mDevice, mFloorPipelineLayout, nullptr);

    vkDestroyRenderPass(mDevice, mRenderPass, nullptr);
    vkDestroyDescriptorSetLayout(mDevice, mParticleDescriptorLayout, nullptr);
    vkDestroyDescriptorSetLayout(mDevice, mFloorDescriptorLayout, nullptr);

    // Destroy pool and all sets allocated from it.
    vkDestroyDescriptorPool(mDevice, mDescriptorPool,nullptr);

    // Destroy the fences
    for (uint32_t i = 0; i < mSwapchainImageCount; i++)
    {
        vkDestroyFence(mDevice, mSubmitFences[i], nullptr);
    }

    // Destroy the textures
    delete mpParticleTextureObject;
    delete mpFloorTextureObject;

    // Free the particles
    free(mpParticles);
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

///////////////////////////////////////////////////////////////////////////////

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