
#include <jni.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#include "sample.h"

// The vertex buffer bind id, used as a constant in various places in the sample
#define VERTEX_BUFFER_BIND_ID 0

// Sample Name
#define SAMPLE_NAME "Vulkan Sample: Deferred Rendering"
#define SAMPLE_VERSION 1


VkSample::VkSample()
    : VkSampleFramework(SAMPLE_NAME, SAMPLE_VERSION)
{
    mDeferredWidth = 1280;
    mDeferredHeight = 720;

    this->SetUseValidation(false);
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
    if (!MeshObject::LoadObj(this, "spheres1.obj", "spheres1.mtl", 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, false, VERTEX_BUFFER_BIND_ID, &mMesh))
    {
        LOGE("Error loading sample mesh file");
        return;
    }

    const float quad_verts[][9] = {
            // Quad 1
            { -1.0f, -1.0f,  1.0f,     1.0f, 1.0f, 1.0f, 1.0f,   0.0f, 1.0f }, // 1 | ---- 2
            {  1.0f, -1.0f,  1.0f,     1.0f, 1.0f, 1.0f, 1.0f,   1.0f, 1.0f }, //   |   /
            {  -1.0f,  1.0f,  1.0f,    1.0f, 1.0f, 1.0f, 1.0f,   0.0f, 0.0f }, // 3 | /

            {  -1.0f,  1.0f,  1.0f,    1.0f, 1.0f, 1.0f, 1.0f,   0.0f, 0.0f }, //       /  2
            {  1.0f, -1.0f,  1.0f,     1.0f, 1.0f, 1.0f, 1.0f,   1.0f, 1.0f }, //     /  |
            {  1.0f,  1.0f,  1.0f,     1.0f, 1.0f, 1.0f, 1.0f,   1.0f, 0.0f }, // 1 /____| 3
    };

    // Quad for final render pass
    mQuadVertices.InitBuffer(this, sizeof(quad_verts), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, (const char*)&quad_verts[0]);
    mQuadVertices.AddBinding(VERTEX_BUFFER_BIND_ID, sizeof(quad_verts[0]), VK_VERTEX_INPUT_RATE_VERTEX);
    mQuadVertices.AddAttribute(VERTEX_BUFFER_BIND_ID, 0, 0, VK_FORMAT_R32G32B32_SFLOAT);                    // float3 position
    mQuadVertices.AddAttribute(VERTEX_BUFFER_BIND_ID, 1, sizeof(float) * 3, VK_FORMAT_R32G32B32A32_SFLOAT); // float4 color
    mQuadVertices.AddAttribute(VERTEX_BUFFER_BIND_ID, 2, sizeof(float) * 7, VK_FORMAT_R32G32_SFLOAT);       // float2 uv

    const float inspect_verts[][9] = {
            {  0.0f, -1.0f,  1.0f,     0.0f, 1.0f, 0.0f, 1.0f,   0.0f, 1.0f }, // 1 | ---- 2
            {  0.75f, -1.0f,  1.0f,    0.0f, 1.0f, 0.0f, 1.0f,   1.0f, 1.0f }, //   |   /
            {  0.0f,  0.0f,  1.0f,     0.0f, 1.0f, 0.0f, 1.0f,   0.0f, 0.0f }, // 3 | /
            {  0.0f,  0.0f,  1.0f,     0.3f, 1.0f, 0.3f, 1.0f,   0.0f, 0.0f }, //       /  2
            {  0.75f, -1.0f,  1.0f,    0.3f, 1.0f, 0.3f, 1.0f,   1.0f, 1.0f }, //     /  |
            {  0.75f,  0.0f,  1.0f,    0.3f, 1.0f, 0.3f, 1.0f,   1.0f, 0.0f }, // 1 /____| 3

            { -0.75f, 0.0f,  1.0f,     0.0f, 0.0f, 1.0f, 1.0f,   0.0f, 1.0f }, // 1 | ---- 2
            {  0.0f, 0.0f,  1.0f,      0.0f, 0.0f, 1.0f, 1.0f,   1.0f, 1.0f }, //   |   /
            { -0.75f,  1.0f,  1.0f,    0.0f, 0.0f, 1.0f, 1.0f,   0.0f, 0.0f }, // 3 | /
            { -0.75f,  1.0f,  1.0f,    0.2f, 0.2f, 1.0f, 1.0f,   0.0f, 0.0f }, //       /  2
            {  0.0f, 0.0f,  1.0f,      0.2f, 0.2f, 1.0f, 1.0f,   1.0f, 1.0f }, //     /  |
            {  0.0f,  1.0f,  1.0f,     0.2f, 0.2f, 1.0f, 1.0f,   1.0f, 0.0f }, // 1 /____| 3

            {  0.0f, 0.0f,  1.0f,      1.0f, 1.0f, 1.0f, 1.0f,   0.0f, 1.0f }, // 1 | ---- 2
            {  0.75f, 0.0f,  1.0f,     1.0f, 1.0f, 1.0f, 1.0f,   1.0f, 1.0f }, //   |   /
            {  0.0f,  1.0f,  1.0f,     1.0f, 1.0f, 1.0f, 1.0f,   0.0f, 0.0f }, // 3 | /
            {  0.0f,  1.0f,  1.0f,     1.0f, 1.0f, 1.0f, 1.0f,   0.0f, 0.0f }, //       /  2
            {  0.75f, 0.0f,  1.0f,     1.0f, 1.0f, 1.0f, 1.0f,   1.0f, 1.0f }, //     /  |
            {  0.75f,  1.0f,  1.0f,    1.0f, 1.0f, 1.0f, 1.0f,   1.0f, 0.0f }, // 1 /____| 3
    };

    // 3 Quads for inspecting g-buffers
    mInspectVertices.InitBuffer(this, sizeof(inspect_verts), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, (const char*)&inspect_verts[0]);
    mInspectVertices.AddBinding(VERTEX_BUFFER_BIND_ID, sizeof(inspect_verts[0]), VK_VERTEX_INPUT_RATE_VERTEX);
    mInspectVertices.AddAttribute(VERTEX_BUFFER_BIND_ID, 0, 0,                   VK_FORMAT_R32G32B32_SFLOAT);    // float3 position
    mInspectVertices.AddAttribute(VERTEX_BUFFER_BIND_ID, 1, sizeof(float) * 3,   VK_FORMAT_R32G32B32A32_SFLOAT); // float4 color
    mInspectVertices.AddAttribute(VERTEX_BUFFER_BIND_ID, 2, sizeof(float) * 7,   VK_FORMAT_R32G32_SFLOAT);       // float2 uv
}

///////////////////////////////////////////////////////////////////////////////

void VkSample::InitUniformBuffers()
{
    // Create our initial MVP matrix
    glm::mat4 modelViewProjection = mProjectionMatrix * mViewMatrix;

    mVertUniformData.modelViewProjection = modelViewProjection;
    mVertUniformData.view = mViewMatrix;
    mVertUniformData.model = mModelMatrix;
    mVertUniformData.camPos = glm::vec4(mCameraPos.x, mCameraPos.y, mCameraPos.z, 1.0f);

    mLightsUniformBuffer.InitBuffer(this, sizeof(mLights), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, (const char*)&mLights);
    mVertShaderUniformBuffer.InitBuffer(this, sizeof(mVertUniformData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, &mVertUniformData);
}

///////////////////////////////////////////////////////////////////////////////

void VkSample::InitTextures()
{
    bool success = true;
    success |= TextureObject::FromKTXFile(this, "rock.ktx", &mTex);
    success |= TextureObject::FromKTXFile(this, "normals.ktx", &mNormalTex);
    assert(success);
}

///////////////////////////////////////////////////////////////////////////////

void VkSample::InitLayouts()
{
    VkResult ret = VK_SUCCESS;

    // This is the layout for the final draw, to a quad with the G-buffers as input
    // No vertex uniforms, but lights available as fragment uniforms
    VkDescriptorSetLayoutBinding uniformAndSamplerBinding[4] = {};

    // Our texture sampler - positions
    uniformAndSamplerBinding[0].binding = 0;
    uniformAndSamplerBinding[0].descriptorCount     = 1;
    uniformAndSamplerBinding[0].descriptorType      = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    uniformAndSamplerBinding[0].stageFlags          = VK_SHADER_STAGE_FRAGMENT_BIT;
    uniformAndSamplerBinding[0].pImmutableSamplers  = nullptr;

    // Another texture sampler - normals
    uniformAndSamplerBinding[1].binding             = 1;
    uniformAndSamplerBinding[1].descriptorCount     = 1;
    uniformAndSamplerBinding[1].descriptorType      = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    uniformAndSamplerBinding[1].stageFlags          = VK_SHADER_STAGE_FRAGMENT_BIT;
    uniformAndSamplerBinding[1].pImmutableSamplers  = nullptr;

    // Another texture sampler - color
    uniformAndSamplerBinding[2].binding             = 2;
    uniformAndSamplerBinding[2].descriptorCount     = 1;
    uniformAndSamplerBinding[2].descriptorType      = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    uniformAndSamplerBinding[2].stageFlags          = VK_SHADER_STAGE_FRAGMENT_BIT;
    uniformAndSamplerBinding[2].pImmutableSamplers  = nullptr;

    //light positions
    uniformAndSamplerBinding[3].binding             = 3;
    uniformAndSamplerBinding[3].descriptorCount     = 1;
    uniformAndSamplerBinding[3].descriptorType      = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    uniformAndSamplerBinding[3].stageFlags          = VK_SHADER_STAGE_FRAGMENT_BIT;
    uniformAndSamplerBinding[3].pImmutableSamplers  = nullptr;

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
    descriptorSetLayoutCreateInfo.sType             = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutCreateInfo.pNext             = nullptr;
    descriptorSetLayoutCreateInfo.bindingCount      = 4;
    descriptorSetLayoutCreateInfo.pBindings         = &uniformAndSamplerBinding[0];

    ret = vkCreateDescriptorSetLayout(mDevice, &descriptorSetLayoutCreateInfo, nullptr, &mDescriptorLayout);
    VK_CHECK(!ret);

    // Our pipeline layout simply points to the empty descriptor layout.
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
    pipelineLayoutCreateInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.pNext                  = nullptr;
    pipelineLayoutCreateInfo.setLayoutCount         = 1;
    pipelineLayoutCreateInfo.pSetLayouts            = &mDescriptorLayout;
    ret = vkCreatePipelineLayout(mDevice, &pipelineLayoutCreateInfo, nullptr, &mPipelineLayout);
    VK_CHECK(!ret);
}

///////////////////////////////////////////////////////////////////////////////

void VkSample::InitDeferredLayouts()
{
    VkResult ret = VK_SUCCESS;

    // This sample has a uniform binding for vertex shader, and 2 samplers in the fragment shader
    VkDescriptorSetLayoutBinding uniformAndSamplerBinding[3] = {};

    // Our MVP matrix
    uniformAndSamplerBinding[0].binding             = 0;
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

    // Another texture sampler
    uniformAndSamplerBinding[2].binding             = 2;
    uniformAndSamplerBinding[2].descriptorCount     = 1;
    uniformAndSamplerBinding[2].descriptorType      = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    uniformAndSamplerBinding[2].stageFlags          = VK_SHADER_STAGE_FRAGMENT_BIT;
    uniformAndSamplerBinding[2].pImmutableSamplers  = nullptr;

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
    descriptorSetLayoutCreateInfo.sType             = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutCreateInfo.pNext             = nullptr;
    descriptorSetLayoutCreateInfo.bindingCount      = 3;
    descriptorSetLayoutCreateInfo.pBindings         = &uniformAndSamplerBinding[0];

    ret = vkCreateDescriptorSetLayout(mDevice, &descriptorSetLayoutCreateInfo, nullptr, &mDeferredDescriptorLayout);
    VK_CHECK(!ret);

    // Our pipeline layout simply points to the empty descriptor layout.
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
    pipelineLayoutCreateInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.pNext                  = nullptr;
    pipelineLayoutCreateInfo.setLayoutCount         = 1;
    pipelineLayoutCreateInfo.pSetLayouts            = &mDeferredDescriptorLayout;
    ret = vkCreatePipelineLayout(mDevice, &pipelineLayoutCreateInfo, nullptr, &mDeferredPipelineLayout);
    VK_CHECK(!ret);
}

///////////////////////////////////////////////////////////////////////////////

void VkSample::InitDeferredDescriptorSet()
{
    // Set up the deferred descriptor set
    VkResult err;
    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
    descriptorSetAllocateInfo.sType                 = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorSetAllocateInfo.pNext                 = nullptr;
    descriptorSetAllocateInfo.descriptorPool        = mDescriptorPool;
    descriptorSetAllocateInfo.descriptorSetCount    = 1;
    descriptorSetAllocateInfo.pSetLayouts = &mDeferredDescriptorLayout;

    err = vkAllocateDescriptorSets(mDevice, &descriptorSetAllocateInfo, &mDeferredDescriptorSet);
    VK_CHECK(!err);

    VkDescriptorImageInfo descriptorImageInfo[2] = {};

    descriptorImageInfo[0].sampler                  = mTex.GetSampler();
    descriptorImageInfo[0].imageView                = mTex.GetView();
    descriptorImageInfo[0].imageLayout              = mTex.GetLayout();

    descriptorImageInfo[1].sampler                  = mNormalTex.GetSampler();
    descriptorImageInfo[1].imageView                = mNormalTex.GetView();
    descriptorImageInfo[1].imageLayout              = mNormalTex.GetLayout();

    VkWriteDescriptorSet writes[3] = {};

    VkDescriptorBufferInfo vertUniform = mVertShaderUniformBuffer.GetDescriptorInfo();
    writes[0].sType                                 = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[0].dstBinding                            = 0;
    writes[0].dstSet                                = mDeferredDescriptorSet;
    writes[0].descriptorCount                       = 1;
    writes[0].descriptorType                        = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    writes[0].pBufferInfo                           = &vertUniform;

    // diffuse
    writes[1].sType                                 = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[1].dstBinding                            = 1;
    writes[1].dstSet                                = mDeferredDescriptorSet;
    writes[1].descriptorCount                       = 1;
    writes[1].descriptorType                        = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writes[1].pImageInfo                            = &descriptorImageInfo[0];

    // Normal map
    writes[2].sType                                 = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[2].dstBinding                            = 2;
    writes[2].dstSet                                = mDeferredDescriptorSet;
    writes[2].descriptorCount                       = 1;
    writes[2].descriptorType                        = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writes[2].pImageInfo                            = &descriptorImageInfo[1];

    vkUpdateDescriptorSets(mDevice, 3, &writes[0], 0, nullptr);
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
    colorReference.attachment                       = 0;
    colorReference.layout                           = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthReference = {};
    depthReference.attachment                       = 1;
    depthReference.layout                           = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

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
    VkShaderModule sh_final_vert = CreateShaderModuleFromAsset("final.vert.spv");
    VkShaderModule sh_final_frag = CreateShaderModuleFromAsset("final.frag.spv");

    VkPipelineVertexInputStateCreateInfo visci = mQuadVertices.CreatePipelineState();

    VkPipelineShaderStageCreateInfo shaderStages[2] = {};
    shaderStages[0].sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[0].stage  = VK_SHADER_STAGE_VERTEX_BIT;
    shaderStages[0].module = sh_final_vert;
    shaderStages[0].pName  = "main";
    shaderStages[1].sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[1].stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
    shaderStages[1].module = sh_final_frag;
    shaderStages[1].pName  = "main";

    // We define a dynamic viewport and scissor.
    VkPipelineViewportStateCreateInfo      vp = {};
    vp.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    vp.viewportCount = 1;
    vp.scissorCount = 1;

    // Instead of statically definding our scissor and viewport as part of the
    // pipeline, we can state they are dynamic, providing them as part of the
    // command stream via command buffer recording operations.
    VkDynamicState dynamicStateList[2] = {};
    dynamicStateList[0] = VK_DYNAMIC_STATE_VIEWPORT;
    dynamicStateList[1] = VK_DYNAMIC_STATE_SCISSOR;

    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {};
    dynamicStateCreateInfo.sType                = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicStateCreateInfo.pNext                = nullptr;
    dynamicStateCreateInfo.dynamicStateCount    = 2;
    dynamicStateCreateInfo.pDynamicStates       = &dynamicStateList[0];

    // This is the pipeline for drawing the final output.
    InitPipeline(VK_NULL_HANDLE, 2, &shaderStages[0], mPipelineLayout, mRenderPass, 0, &visci, nullptr,
                 nullptr, &vp, nullptr, nullptr, nullptr, nullptr, &dynamicStateCreateInfo, false, VK_NULL_HANDLE, &mPipeline);

    // This is a pipeline for inspecting the G-buffers.
    VkShaderModule sh_inspect_vert = CreateShaderModuleFromAsset("inspect.vert.spv");
    VkShaderModule sh_inspect_frag = CreateShaderModuleFromAsset("inspect.frag.spv");

    visci = mInspectVertices.CreatePipelineState();

    shaderStages[0].module = sh_inspect_vert;
    shaderStages[1].module = sh_inspect_frag;

    InitPipeline(VK_NULL_HANDLE, 2, &shaderStages[0], mPipelineLayout, mRenderPass, 0, &visci, nullptr,
                 nullptr, &vp, nullptr, nullptr, nullptr, nullptr, &dynamicStateCreateInfo, false, VK_NULL_HANDLE, &mInspectPipeline);


    vkDestroyShaderModule(mDevice, sh_inspect_vert, nullptr);
    vkDestroyShaderModule(mDevice, sh_inspect_frag, nullptr);

    vkDestroyShaderModule(mDevice, sh_final_vert, nullptr);
    vkDestroyShaderModule(mDevice, sh_final_frag, nullptr);
}

///////////////////////////////////////////////////////////////////////////////

void VkSample::InitDescriptorPool()
{
    //Create a pool with the amount of descriptors we require
    VkDescriptorPoolSize poolSize[2] = {};

    // First set has
    // 1x - VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC
    // 2x - VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER

    // Second set has
    // 1x - VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC
    // 3x - VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER

    // Total allocations from the pool
    // 2 sets
    // 2x - VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC
    // 5x - VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER

    poolSize[0].type                        = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    poolSize[0].descriptorCount             = 2;

    poolSize[1].type                        = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSize[1].descriptorCount             = 5;

    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
    descriptorPoolCreateInfo.sType          = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolCreateInfo.pNext          = nullptr;
    descriptorPoolCreateInfo.maxSets        = 2;
    descriptorPoolCreateInfo.poolSizeCount  = 2;
    descriptorPoolCreateInfo.pPoolSizes     = poolSize;

    VkResult err;
    err = vkCreateDescriptorPool(mDevice, &descriptorPoolCreateInfo, NULL, &mDescriptorPool);
    VK_CHECK(!err);
}
///////////////////////////////////////////////////////////////////////////////

void VkSample::InitDescriptorSet()
{
    // This is the descriptor for the final draw, to a quad with the G-buffers as input
    VkResult err;
    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
    descriptorSetAllocateInfo.sType                 = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorSetAllocateInfo.pNext                 = nullptr;
    descriptorSetAllocateInfo.descriptorPool        = mDescriptorPool;
    descriptorSetAllocateInfo.descriptorSetCount    = 1;
    descriptorSetAllocateInfo.pSetLayouts           = &mDescriptorLayout;

    err = vkAllocateDescriptorSets(mDevice, &descriptorSetAllocateInfo, &mDescriptorSet);
    VK_CHECK(!err);

    VkDescriptorImageInfo descriptorImageInfo[3] = {};

    descriptorImageInfo[0].sampler                  = mPositionTexture.GetSampler();
    descriptorImageInfo[0].imageView                = mPositionTexture.GetView();
    descriptorImageInfo[0].imageLayout              = VK_IMAGE_LAYOUT_GENERAL;

    descriptorImageInfo[1].sampler                  = mNormalTexture.GetSampler();
    descriptorImageInfo[1].imageView                = mNormalTexture.GetView();
    descriptorImageInfo[1].imageLayout              = VK_IMAGE_LAYOUT_GENERAL;

    descriptorImageInfo[2].sampler                  = mColorTexture.GetSampler();
    descriptorImageInfo[2].imageView                = mColorTexture.GetView();
    descriptorImageInfo[2].imageLayout              = VK_IMAGE_LAYOUT_GENERAL;

    VkWriteDescriptorSet writes[4] = {};

    // diffuse
    writes[0].sType             = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[0].dstBinding        = 0;
    writes[0].dstSet            = mDescriptorSet;
    writes[0].descriptorCount   = 1;
    writes[0].descriptorType    = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writes[0].pImageInfo        = &descriptorImageInfo[0];

    // Normal map
    writes[1].sType             = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[1].dstBinding        = 1;
    writes[1].dstSet            = mDescriptorSet;
    writes[1].descriptorCount   = 1;
    writes[1].descriptorType    = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writes[1].pImageInfo        = &descriptorImageInfo[1];

    // color map
    writes[2].sType             = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[2].dstBinding        = 2;
    writes[2].dstSet            = mDescriptorSet;
    writes[2].descriptorCount   = 1;
    writes[2].descriptorType    = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writes[2].pImageInfo        = &descriptorImageInfo[2];

    // lights
    VkDescriptorBufferInfo lightsInfo = mLightsUniformBuffer.GetDescriptorInfo();
    writes[3].sType             = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[3].dstBinding        = 3;
    writes[3].dstSet            = mDescriptorSet;
    writes[3].descriptorCount   = 1;
    writes[3].descriptorType    = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    writes[3].pBufferInfo       = &lightsInfo;

    vkUpdateDescriptorSets(mDevice, 4, &writes[0], 0, nullptr);
}

///////////////////////////////////////////////////////////////////////////////

void VkSample::BuildCmdBuffer()
{
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
        rp_begin.sType                      = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        rp_begin.pNext                      = nullptr;
        rp_begin.renderPass                 = mRenderPass;
        rp_begin.framebuffer                = mFrameBuffers[i];
        rp_begin.renderArea.offset.x        = 0;
        rp_begin.renderArea.offset.y        = 0;
        rp_begin.renderArea.extent.width    = mWidth;
        rp_begin.renderArea.extent.height   = mHeight;
        rp_begin.clearValueCount            = 2;
        rp_begin.pClearValues               = clear_values;

        vkCmdBeginRenderPass(cmdBuffer, &rp_begin, VK_SUBPASS_CONTENTS_INLINE);

        // Set our pipeline. This holds all major state
        // the pipeline defines, for example, that the vertex buffer is a triangle list.
        vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline);

        //bind out descriptor set, which handles our uniforms and samplers
        uint32_t dynamicOffsets[1]={0};
        vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout, 0, 1, &mDescriptorSet, 1, dynamicOffsets);

        // Provide the dynamic state missing from our pipeline
        VkViewport viewport;
        memset(&viewport, 0, sizeof(viewport));
        viewport.width          = mWidth;
        viewport.height         = mHeight;
        viewport.minDepth       = 0.0f;
        viewport.maxDepth       = 1.0f;
        vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);

        VkRect2D scissor;
        memset(&scissor, 0, sizeof(scissor));
        scissor.extent.width    = mWidth;
        scissor.extent.height   = mHeight;
        scissor.offset.x        = 0;
        scissor.offset.y        = 0;
        vkCmdSetScissor(cmdBuffer, 0, 1, &scissor);

        // Bind our vertex buffer, with a 0 offset.
        VkDeviceSize offsets[1] = {0};
        vkCmdBindVertexBuffers(cmdBuffer, VERTEX_BUFFER_BIND_ID, 1, &mQuadVertices.GetBuffer(), offsets);

        // Issue a draw command, with our vertices. Full screen quad
        vkCmdDraw(cmdBuffer, 3*2, 1, 0, 0);

        vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mInspectPipeline);
        vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout, 0, 1, &mDescriptorSet, 1, dynamicOffsets);

        memset(&viewport, 0, sizeof(viewport));
        viewport.x              = mWidth/2;
        viewport.y              = mHeight/2;
        viewport.width          = mWidth/2;
        viewport.height         = mHeight/2;
        viewport.minDepth       = 0.0f;
        viewport.maxDepth       = 1.0f;
        vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);

        vkCmdSetScissor(cmdBuffer, 0, 1, &scissor);

        offsets[1] = {0};
        vkCmdBindVertexBuffers(cmdBuffer, VERTEX_BUFFER_BIND_ID, 1, &mInspectVertices.GetBuffer(), offsets);

        // Issue a draw command, with our vertices.
        vkCmdDraw(cmdBuffer, 3*6, 1, 0, 0);

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

void VkSample::InitFrameBuffers()
{

    //The framebuffer objects reference the renderpass, and allow
    // the references defined in that renderpass to now attach to views.
    // The views in this example are the colour view, which is our swapchain image,
    // and the depth buffer created manually earlier.
    VkImageView attachments[2] = {};
    VkFramebufferCreateInfo framebufferCreateInfo = {};
    framebufferCreateInfo.sType             = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferCreateInfo.pNext             = nullptr;
    framebufferCreateInfo.renderPass        = mRenderPass;
    framebufferCreateInfo.attachmentCount   = 2;
    framebufferCreateInfo.pAttachments      = attachments;
    framebufferCreateInfo.width             = mWidth;
    framebufferCreateInfo.height            = mHeight;
    framebufferCreateInfo.layers            = 1;

    VkResult ret;

    mFrameBuffers = new VkFramebuffer[mSwapchainImageCount];
    // Reusing the framebufferCreateInfo to create m_swapchainImageCount framebuffers,
    // only the attachments to the relevent image views change each time.
    for (uint32_t i = 0; i < mSwapchainImageCount; i++)
    {
        attachments[0] = mSwapchainBuffers[i].view;
        attachments[1] = mDepthBuffers[i].view;

        ret = vkCreateFramebuffer(mDevice, &framebufferCreateInfo, nullptr, &mFrameBuffers[i]);
        VK_CHECK(!ret);
    }
}

void VkSample::InitDeferredFramebuffers()
{
    // Create images which will be the targets for G-buffer data
    ImageViewObject::CreateImageView(this, mDeferredWidth, mDeferredHeight, VK_FORMAT_R32G32B32A32_SFLOAT,
                                     VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                     VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
                                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &mPositionTarget);

    ImageViewObject::CreateImageView(this, mDeferredWidth, mDeferredHeight, VK_FORMAT_R32G32B32A32_SFLOAT,
                                     VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                     VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
                                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &mNormalTarget);

    ImageViewObject::CreateImageView(this, mDeferredWidth, mDeferredHeight, VK_FORMAT_R8G8B8A8_UNORM,
                                     VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                     VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
                                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &mColorTarget);

    ImageViewObject::CreateImageView(this, mDeferredWidth, mDeferredHeight, VK_FORMAT_D16_UNORM,
                                     VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                                     VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
                                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &mDepthTarget);

    // Create a renderpass for deferred rendering, then create a frame buffer for it
    // worldPos, normal, texture, depth
    VkAttachmentDescription attachmentDescriptions[4] = {};
    attachmentDescriptions[0].flags             = 0;
    attachmentDescriptions[0].format            = mPositionTarget.GetFormat();
    attachmentDescriptions[0].samples           = VK_SAMPLE_COUNT_1_BIT;
    attachmentDescriptions[0].loadOp            = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachmentDescriptions[0].storeOp           = VK_ATTACHMENT_STORE_OP_STORE;
    attachmentDescriptions[0].stencilLoadOp     = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachmentDescriptions[0].stencilStoreOp    = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachmentDescriptions[0].initialLayout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    attachmentDescriptions[0].finalLayout       = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    attachmentDescriptions[1].flags             = 0;
    attachmentDescriptions[1].format            = mNormalTarget.GetFormat();
    attachmentDescriptions[1].samples           = VK_SAMPLE_COUNT_1_BIT;
    attachmentDescriptions[1].loadOp            = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachmentDescriptions[1].storeOp           = VK_ATTACHMENT_STORE_OP_STORE;
    attachmentDescriptions[1].stencilLoadOp     = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachmentDescriptions[1].stencilStoreOp    = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachmentDescriptions[1].initialLayout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    attachmentDescriptions[1].finalLayout       = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    attachmentDescriptions[2].flags             = 0;
    attachmentDescriptions[2].format            = mColorTarget.GetFormat();
    attachmentDescriptions[2].samples           = VK_SAMPLE_COUNT_1_BIT;
    attachmentDescriptions[2].loadOp            = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachmentDescriptions[2].storeOp           = VK_ATTACHMENT_STORE_OP_STORE;
    attachmentDescriptions[2].stencilLoadOp     = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachmentDescriptions[2].stencilStoreOp    = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachmentDescriptions[2].initialLayout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    attachmentDescriptions[2].finalLayout       = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    attachmentDescriptions[3].flags             = 0;
    attachmentDescriptions[3].format            = mDepthBuffers[0].format;
    attachmentDescriptions[3].samples           = VK_SAMPLE_COUNT_1_BIT;
    attachmentDescriptions[3].loadOp            = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachmentDescriptions[3].storeOp           = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachmentDescriptions[3].stencilLoadOp     = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachmentDescriptions[3].stencilStoreOp    = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachmentDescriptions[3].initialLayout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    attachmentDescriptions[3].finalLayout       = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    // We have references to the attachment offsets, stating the layout type.
    VkAttachmentReference colorReference[3] = {};
    colorReference[0].attachment                = 0;
    colorReference[0].layout                    = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    colorReference[1].attachment                = 1;
    colorReference[1].layout                    = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    colorReference[2].attachment                = 2;
    colorReference[2].layout                    = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthReference = {};
    depthReference.attachment                   = 3;
    depthReference.layout                       = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpassDescription = {};
    subpassDescription.pipelineBindPoint        = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.flags                    = 0;
    subpassDescription.inputAttachmentCount     = 0;
    subpassDescription.pInputAttachments        = nullptr;
    subpassDescription.colorAttachmentCount     = 3;
    subpassDescription.pColorAttachments        = &colorReference[0];
    subpassDescription.pResolveAttachments      = nullptr;
    subpassDescription.pDepthStencilAttachment  = &depthReference;
    subpassDescription.preserveAttachmentCount  = 0;
    subpassDescription.pPreserveAttachments     = nullptr;

    // Crate a renderpass for deferred rendering
    VkRenderPassCreateInfo renderPassCreateInfo = {};
    renderPassCreateInfo.sType                  = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.pNext                  = nullptr;
    renderPassCreateInfo.attachmentCount        = 4;
    renderPassCreateInfo.pAttachments           = attachmentDescriptions;
    renderPassCreateInfo.subpassCount           = 1;
    renderPassCreateInfo.pSubpasses             = &subpassDescription;
    renderPassCreateInfo.dependencyCount        = 0;
    renderPassCreateInfo.pDependencies          = nullptr;

    VkResult ret;
    ret = vkCreateRenderPass(mDevice, &renderPassCreateInfo, nullptr, &mDeferredRenderPass);
    VK_CHECK(!ret);

    VkImageView attachments[4] = {};
    VkFramebufferCreateInfo framebufferCreateInfo = {};
    framebufferCreateInfo.sType                 = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferCreateInfo.pNext                 = nullptr;
    framebufferCreateInfo.renderPass            = mDeferredRenderPass;
    framebufferCreateInfo.attachmentCount       = 4;
    framebufferCreateInfo.pAttachments          = attachments;
    framebufferCreateInfo.width                 = mDeferredWidth;
    framebufferCreateInfo.height                = mDeferredHeight;
    framebufferCreateInfo.layers                = 1;

    attachments[0] = mPositionTarget.GetView();
    attachments[1] = mNormalTarget.GetView();
    attachments[2] = mColorTarget.GetView();
    attachments[3] = mDepthTarget.GetView();

    ret = vkCreateFramebuffer(mDevice, &framebufferCreateInfo, nullptr, &mDeferredFrameBuffer);
    VK_CHECK(!ret);
}

void VkSample::InitDeferredTextures()
{
    // These textures are the input to the final render pass, where G-buffers are input.
    // The targets from the deferred rendering pass are transferred to these textures so they can
    // be sampled as fragment shader inputs.
    TextureObject::CreateTexture(this, mDeferredWidth, mDeferredHeight, mPositionTarget.GetFormat(),
                                 VK_IMAGE_USAGE_TRANSFER_DST_BIT, &mPositionTexture);

    TextureObject::CreateTexture(this, mDeferredWidth, mDeferredHeight, mNormalTarget.GetFormat(),
                                 VK_IMAGE_USAGE_TRANSFER_DST_BIT, &mNormalTexture);

    TextureObject::CreateTexture(this, mDeferredWidth, mDeferredHeight, mColorTarget.GetFormat(),
                                 VK_IMAGE_USAGE_TRANSFER_DST_BIT, &mColorTexture);
}

void VkSample::InitDeferredPipelines()
{
    //This pipeline is the one which collects and outputs G-Buffer data
    VkShaderModule sh_def_vert = CreateShaderModuleFromAsset("deferred.vert.spv");
    VkShaderModule sh_def_frag = CreateShaderModuleFromAsset("deferred.frag.spv");

    VkPipelineVertexInputStateCreateInfo visci = mMesh.Buffer().CreatePipelineState();

    VkPipelineShaderStageCreateInfo shaderStages[2] = {};
    shaderStages[0].sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[0].stage  = VK_SHADER_STAGE_VERTEX_BIT;
    shaderStages[0].module = sh_def_vert;
    shaderStages[0].pName  = "main";
    shaderStages[1].sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[1].stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
    shaderStages[1].module = sh_def_frag;
    shaderStages[1].pName  = "main";

    // We define a simple viewport and scissor.
    VkPipelineViewportStateCreateInfo      vp = {};
    vp.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    vp.viewportCount = 1;
    vp.scissorCount = 1;

    // Instead of statically defining our scissor and viewport as part of the
    // pipeline, we can state they are dynamic, providing them as part of the
    // command stream via command buffer recording operations.
    VkDynamicState dynamicStateList[2] = {};
    dynamicStateList[0] = VK_DYNAMIC_STATE_VIEWPORT;
    dynamicStateList[1] = VK_DYNAMIC_STATE_SCISSOR;

    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {};
    dynamicStateCreateInfo.sType                = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicStateCreateInfo.pNext                = nullptr;
    dynamicStateCreateInfo.dynamicStateCount    = 2;
    dynamicStateCreateInfo.pDynamicStates       = &dynamicStateList[0];

    // This pipeline writes our G-buffers. Ensure the colorWriteMask is valid for each
    // attachment.
    VkPipelineColorBlendAttachmentState att_state[3] = {};
    att_state[0].colorWriteMask = 0xf;
    att_state[0].blendEnable    = VK_FALSE;
    att_state[1].colorWriteMask = 0xf;
    att_state[1].blendEnable    = VK_FALSE;
    att_state[2].colorWriteMask = 0xf;
    att_state[2].blendEnable    = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo    cb = {};
    cb.sType                = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    cb.attachmentCount      = 3;
    cb.pAttachments         = &att_state[0];

    InitPipeline(VK_NULL_HANDLE, 2, &shaderStages[0], mDeferredPipelineLayout, mDeferredRenderPass, 0, &visci, nullptr,
                 nullptr, &vp, nullptr, nullptr, nullptr, &cb, &dynamicStateCreateInfo, false, VK_NULL_HANDLE, &mDeferredPipeline);

    vkDestroyShaderModule(mDevice, sh_def_frag, nullptr);
    vkDestroyShaderModule(mDevice, sh_def_vert, nullptr);
}

void VkSample::InitDeferredCommandBuffers()
{
    VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
    commandBufferAllocateInfo.sType                 = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.pNext                 = nullptr;
    commandBufferAllocateInfo.commandPool           = mCommandPool;
    commandBufferAllocateInfo.level                 = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandBufferCount    = 1;

    // Allocate a shared buffer for use in setup operations.
    VkResult ret = vkAllocateCommandBuffers(mDevice, &commandBufferAllocateInfo,
                                            &mDeferredCommandBuffer);
    VK_CHECK(!ret);

    // vkBeginCommandBuffer should reset the command buffer, but Reset can be called
    // to make it more explicit.
    VkResult err;
    err = vkResetCommandBuffer(mDeferredCommandBuffer, 0);
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
    err = vkBeginCommandBuffer(mDeferredCommandBuffer, &cmd_buf_info);
    VK_CHECK(!err);

    // When starting the render pass, we can set clear values.
    VkClearValue clear_values[4] = {};
    clear_values[0].color.float32[0]        = 0.0f;
    clear_values[0].color.float32[1]        = 0.0f;
    clear_values[0].color.float32[2]        = 0.0f;
    clear_values[0].color.float32[3]        = 0.0f;
    clear_values[1].color.float32[0]        = 0.0f;
    clear_values[1].color.float32[1]        = 0.0f;
    clear_values[1].color.float32[2]        = 0.0f;
    clear_values[1].color.float32[3]        = 0.0f;
    clear_values[2].color.float32[0]        = 0.0f;
    clear_values[2].color.float32[1]        = 0.0f;
    clear_values[2].color.float32[2]        = 0.0f;
    clear_values[2].color.float32[3]        = 1.0f;
    clear_values[3].depthStencil.depth      = 1.0f;
    clear_values[3].depthStencil.stencil    = 0;

    VkRenderPassBeginInfo rp_begin = {};
    rp_begin.sType                      = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rp_begin.pNext                      = nullptr;
    rp_begin.renderPass                 = mDeferredRenderPass;
    rp_begin.framebuffer                = mDeferredFrameBuffer;
    rp_begin.renderArea.offset.x        = 0;
    rp_begin.renderArea.offset.y        = 0;
    rp_begin.renderArea.extent.width    = mPositionTexture.GetWidth();
    rp_begin.renderArea.extent.height   = mPositionTexture.GetHeight();
    rp_begin.clearValueCount            = 4;
    rp_begin.pClearValues               = clear_values;

    vkCmdBeginRenderPass(mDeferredCommandBuffer, &rp_begin, VK_SUBPASS_CONTENTS_INLINE);

    // Set our pipeline. This holds all major state
    // the pipeline defines, for example, that the vertex buffer is a triangle list.
    vkCmdBindPipeline(mDeferredCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mDeferredPipeline);

    //bind out descriptor set, which handles our uniforms and samplers
    uint32_t dynamicOffsets[1] = {0};
    vkCmdBindDescriptorSets(mDeferredCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            mDeferredPipelineLayout, 0, 1, &mDeferredDescriptorSet, 1, dynamicOffsets);

    // Provide the dynamic state missing from our pipeline
    VkViewport viewport;
    memset(&viewport, 0, sizeof(viewport));
    viewport.width      = mPositionTarget.GetWidth();
    viewport.height     = mPositionTarget.GetHeight();
    viewport.minDepth   = 0.0f;
    viewport.maxDepth   = 1.0f;
    vkCmdSetViewport(mDeferredCommandBuffer, 0, 1, &viewport);

    VkRect2D scissor;
    memset(&scissor, 0, sizeof(scissor));
    scissor.extent.width    = mPositionTarget.GetWidth();;
    scissor.extent.height   = mPositionTarget.GetHeight();
    scissor.offset.x        = 0;
    scissor.offset.y        = 0;
    vkCmdSetScissor(mDeferredCommandBuffer, 0, 1, &scissor);

    // Bind our vertex buffer, with a 0 offset.
    VkDeviceSize offsets[1] = {0};
    vkCmdBindVertexBuffers(mDeferredCommandBuffer, VERTEX_BUFFER_BIND_ID, 1, &mMesh.Buffer().GetBuffer(), offsets);

    // Issue a draw command
    vkCmdDraw(mDeferredCommandBuffer, mMesh.GetNumVertices(), 1, 0, 0);

    // Now our render pass has ended.
    vkCmdEndRenderPass(mDeferredCommandBuffer);

    TargetToTexture(mDeferredCommandBuffer, mPositionTarget,mPositionTexture);
    TargetToTexture(mDeferredCommandBuffer, mNormalTarget,  mNormalTexture);
    TargetToTexture(mDeferredCommandBuffer, mColorTarget,   mColorTexture);

    // By ending the command buffer, it is put out of record mode.
    err = vkEndCommandBuffer(mDeferredCommandBuffer);
    VK_CHECK(!err);
}

bool VkSample::InitSample()
{
    // Initialize our matrices
    float aspect = (float)mWidth/(float)mHeight;
    mProjectionMatrix = glm::perspectiveRH(glm::radians(45.0f), aspect, 0.1f, 1000.0f );

    mCameraPos = glm::vec3(0.0f, 0.0f, -12.0f);
    glm::vec3 lookat(0.0f, 0.0f, 0.0f);
    glm::vec3 up(0.0f, 1.0f, 0.0f);
    mViewMatrix =  glm::lookAtRH(mCameraPos, lookat, up);
    mModelMatrix = glm::translate(glm::mat4(), glm::vec3(-2.0f, 0, 0));

    mLights.color[0].r = 1.0f;
    mLights.color[0].g = 1.0f;
    mLights.color[0].b = 1.0f;
    mLights.color[0].a = 1.0f;

    mLights.color[1].r = 0.0f;
    mLights.color[1].g = 0.0f;
    mLights.color[1].b = 1.0f;
    mLights.color[1].a = 1.0f;

    mLights.color[2].r = 1.0f;
    mLights.color[2].g = 1.0f;
    mLights.color[2].b = 0.0f;
    mLights.color[2].a = 1.0f;

    mLights.color[3].r = 0.0f;
    mLights.color[3].g = 0.0f;
    mLights.color[3].b = 1.0f;
    mLights.color[3].a = 1.0f;

    InitTextures();
    InitUniformBuffers();
    InitVertexBuffers();
    InitDescriptorPool();

    InitDeferredLayouts();
    InitDeferredDescriptorSet();
    InitDeferredFramebuffers();
    InitDeferredTextures();
    InitDeferredPipelines();

    InitDeferredCommandBuffers();

    InitLayouts();
    InitDescriptorSet();
    InitRenderPass();
    InitPipelines();
    InitFrameBuffers();

    BuildCmdBuffer();
}

///////////////////////////////////////////////////////////////////////////////

bool VkSample::Draw()
{
    // There are two buffers to submit:
    //   The first buffer contains the commands that generate G-buffers, and copy them
    //    from target images to textures, which can be sampled.
    //   The second buffer contains the presentation pass, with a final full-screen quad
    //    drawn using our final shader which takes the g-buffer and generates the final image
    VkCommandBuffer drawbuffers[2] = {
            mDeferredCommandBuffer,
            mSwapchainBuffers[mSwapchainCurrentIdx].cmdBuffer,
    };

    const VkPipelineStageFlags WaitDstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    VkSubmitInfo submitInfo = {};
    submitInfo.sType                    = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext                    = nullptr;
    submitInfo.waitSemaphoreCount       = 1;
    submitInfo.pWaitSemaphores          = &mBackBufferSemaphore;
    submitInfo.pWaitDstStageMask        = &WaitDstStageMask;
    submitInfo.commandBufferCount       = 2;
    submitInfo.pCommandBuffers          = &drawbuffers[0];
    submitInfo.signalSemaphoreCount     = 1;
    submitInfo.pSignalSemaphores        = &mRenderCompleteSemaphore;

    VkResult err;
    err = vkQueueSubmit(mQueue, 1, &submitInfo,  VK_NULL_HANDLE);
    VK_CHECK(!err);
}


///////////////////////////////////////////////////////////////////////////////

bool VkSample::Update()
{
    // We want an explicit sync for this sample
    vkDeviceWaitIdle(mDevice);

    static float spinAngle = 0.1f;

    glm::mat4 rotation = glm::rotate(glm::mat4(), glm::radians(spinAngle), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 model = mModelMatrix * rotation ;
    glm::mat4 modelView = mViewMatrix * model;
    glm::mat4 modelViewProjection = mProjectionMatrix * modelView;

    VkResult ret = VK_SUCCESS;
    uint8_t *pData;

    mVertUniformData.modelViewProjection = modelViewProjection;
    mVertUniformData.view = mViewMatrix;
    mVertUniformData.model = model;
    mVertUniformData.camPos = glm::vec4(mCameraPos.x, mCameraPos.y, mCameraPos.z, 1.0f);

    ret = vkMapMemory(mDevice, mVertShaderUniformBuffer.GetDeviceMemory(), 0, mVertShaderUniformBuffer.GetAllocationSize(), 0, (void **) &pData);
    assert(!ret);

    memcpy(pData, &mVertUniformData, sizeof(mVertUniformData));

    vkUnmapMemory(mDevice, mVertShaderUniformBuffer.GetDeviceMemory());

    //fragment uniform - lights
    mLights.posAndSize[0].x = -1.5;
    mLights.posAndSize[0].y = -2.5;
    mLights.posAndSize[0].z = -14.5f;
    mLights.posAndSize[0].w = 20.0f;

    mLights.posAndSize[1].x = sinf(spinAngle * 0.2f) * 5.9f;
    mLights.posAndSize[1].y = cosf(spinAngle * 0.31f) * 5.5f;
    mLights.posAndSize[1].z = -5.0f;
    mLights.posAndSize[1].w = 50.5f;

    mLights.posAndSize[2].x = cosf(spinAngle * 0.3f) * 10.9f;
    mLights.posAndSize[2].y = sinf(2.0f + spinAngle * 0.21f) * 10.5f;
    mLights.posAndSize[2].z = 2.05f;
    mLights.posAndSize[2].w = 20.2f;

    mLights.posAndSize[3].x = cosf(1.2f + spinAngle * 0.1f) * 2.9f;
    mLights.posAndSize[3].y = -2.5f;
    mLights.posAndSize[3].z = sinf(3.2f + spinAngle * 0.23f) * 1.5 + 1.0f;
    mLights.posAndSize[3].w = 10.5f;

    ret = vkMapMemory(mDevice, mLightsUniformBuffer.GetDeviceMemory(), 0, mLightsUniformBuffer.GetAllocationSize(), 0, (void**)&pData);

    memcpy(pData, &mLights, sizeof(mLights));

    vkUnmapMemory(mDevice, mLightsUniformBuffer.GetDeviceMemory());

    spinAngle += 0.4f;
}

///////////////////////////////////////////////////////////////////////////////

bool VkSample::DestroySample()
{
    // destroy framebuffers
    for (uint32_t i = 0; i < mSwapchainImageCount; i++)
    {
        vkDestroyFramebuffer(mDevice, mFrameBuffers[i], nullptr);
    }
    delete [] mFrameBuffers;

    // Destroy pipeline resources
    vkDestroyPipeline(mDevice, mPipeline, nullptr);
    vkDestroyRenderPass(mDevice, mRenderPass, nullptr);
    vkDestroyPipelineLayout(mDevice, mPipelineLayout, nullptr);
    vkDestroyDescriptorSetLayout(mDevice, mDescriptorLayout, nullptr);

    // Destroy deferred pipeline resources
    vkDestroyPipeline(mDevice, mDeferredPipeline, nullptr);
    vkDestroyRenderPass(mDevice, mDeferredRenderPass, nullptr);
    vkDestroyPipelineLayout(mDevice, mDeferredPipelineLayout, nullptr);
    vkDestroyDescriptorSetLayout(mDevice, mDeferredDescriptorLayout, nullptr);
    vkDestroyFramebuffer(mDevice, mDeferredFrameBuffer, nullptr);

    // Destroy inspection pipeline
    vkDestroyPipeline(mDevice, mInspectPipeline, nullptr);

    // Destroy descriptor pool
    vkDestroyDescriptorPool(mDevice, mDescriptorPool, nullptr);

    // Destroy vertices
    mVertexBuff.Destroy();
    mQuadVertices.Destroy();

    mTex.Destroy();
    mNormalTex.Destroy();

    mNormalTarget.Destroy();
    mPositionTarget.Destroy();
    mColorTarget.Destroy();
    mNormalTexture.Destroy();
    mPositionTexture.Destroy();
    mColorTexture.Destroy();

    // Destroy uniform
    mVertShaderUniformBuffer.Destroy();
    mLightsUniformBuffer.Destroy();
}

///////////////////////////////////////////////////////////////////////////////
// Begin Android Glue entry point
#include <android_native_app_glue.h>

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
            break;}
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