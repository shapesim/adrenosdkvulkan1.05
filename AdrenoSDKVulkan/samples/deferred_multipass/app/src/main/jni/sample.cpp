
#include <jni.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#include "sample.h"

// The vertex buffer bind id, used as a constant in various places in the sample
#define VERTEX_BUFFER_BIND_ID 0

// Sample Name
#define SAMPLE_NAME "Vulkan Sample: Multipass Deferred Rendering"
#define SAMPLE_VERSION 1


VkSample::VkSample() : VkSampleFramework(SAMPLE_NAME, SAMPLE_VERSION)
{
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

    mDeferredWidth = mWidth;
    mDeferredHeight = mHeight;
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
            {  -1.0f, -1.0f,  1.0f,    1.0f, 1.0f, 1.0f, 1.0f,   0.0f, 1.0f }, // 1 | ---- 2
            {   1.0f, -1.0f,  1.0f,    1.0f, 1.0f, 1.0f, 1.0f,   1.0f, 1.0f }, //   |   /
            {  -1.0f,  1.0f,  1.0f,    1.0f, 1.0f, 1.0f, 1.0f,   0.0f, 0.0f }, // 3 | /

            {  -1.0f,  1.0f,  1.0f,    1.0f, 1.0f, 1.0f, 1.0f,   0.0f, 0.0f }, //       /  2
            {   1.0f, -1.0f,  1.0f,    1.0f, 1.0f, 1.0f, 1.0f,   1.0f, 1.0f }, //     /  |
            {   1.0f,  1.0f,  1.0f,    1.0f, 1.0f, 1.0f, 1.0f,   1.0f, 0.0f }, // 1 /____| 3
    };

    // Quad for final render pass
    mQuadVertices.InitBuffer(this, sizeof(quad_verts), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, (const char*)&quad_verts[0]);
    mQuadVertices.AddBinding(VERTEX_BUFFER_BIND_ID, sizeof(quad_verts[0]), VK_VERTEX_INPUT_RATE_VERTEX);
    mQuadVertices.AddAttribute(VERTEX_BUFFER_BIND_ID, 0, 0, VK_FORMAT_R32G32B32_SFLOAT);    // float3 position
    mQuadVertices.AddAttribute(VERTEX_BUFFER_BIND_ID, 1, sizeof(float) * 3, VK_FORMAT_R32G32B32A32_SFLOAT); // float4 color
    mQuadVertices.AddAttribute(VERTEX_BUFFER_BIND_ID, 2, sizeof(float) * 7, VK_FORMAT_R32G32_SFLOAT);       // float2 uv

    const float inspect_verts[][9] = {
            {  0.0f,  -1.0f,  1.0f,    0.0f, 1.0f, 0.0f, 1.0f,   0.0f, 1.0f }, // 1 | ---- 2
            {  0.75f, -1.0f,  1.0f,    0.0f, 1.0f, 0.0f, 1.0f,   1.0f, 1.0f }, //   |   /
            {  0.0f,   0.0f,  1.0f,    0.0f, 1.0f, 0.0f, 1.0f,   0.0f, 0.0f }, // 3 | /
            {  0.0f,   0.0f,  1.0f,    0.3f, 1.0f, 0.3f, 1.0f,   0.0f, 0.0f }, //       /  2
            {  0.75f, -1.0f,  1.0f,    0.3f, 1.0f, 0.3f, 1.0f,   1.0f, 1.0f }, //     /  |
            {  0.75f,  0.0f,  1.0f,    0.3f, 1.0f, 0.3f, 1.0f,   1.0f, 0.0f }, // 1 /____| 3

            { -0.75f,  0.0f,  1.0f,    0.0f, 0.0f, 1.0f, 1.0f,   0.0f, 1.0f }, // 1 | ---- 2
            {  0.0f,   0.0f,  1.0f,    0.0f, 0.0f, 1.0f, 1.0f,   1.0f, 1.0f }, //   |   /
            { -0.75f,  1.0f,  1.0f,    0.0f, 0.0f, 1.0f, 1.0f,   0.0f, 0.0f }, // 3 | /
            { -0.75f,  1.0f,  1.0f,    0.2f, 0.2f, 1.0f, 1.0f,   0.0f, 0.0f }, //       /  2
            {  0.0f,   0.0f,  1.0f,    0.2f, 0.2f, 1.0f, 1.0f,   1.0f, 1.0f }, //     /  |
            {  0.0f,   1.0f,  1.0f,    0.2f, 0.2f, 1.0f, 1.0f,   1.0f, 0.0f }, // 1 /____| 3

            {  0.0f,   0.0f,  1.0f,    1.0f, 1.0f, 1.0f, 1.0f,   0.0f, 1.0f }, // 1 | ---- 2
            {  0.75f,  0.0f,  1.0f,    1.0f, 1.0f, 1.0f, 1.0f,   1.0f, 1.0f }, //   |   /
            {  0.0f,   1.0f,  1.0f,    1.0f, 1.0f, 1.0f, 1.0f,   0.0f, 0.0f }, // 3 | /
            {  0.0f,   1.0f,  1.0f,    1.0f, 1.0f, 1.0f, 1.0f,   0.0f, 0.0f }, //       /  2
            {  0.75f,  0.0f,  1.0f,    1.0f, 1.0f, 1.0f, 1.0f,   1.0f, 1.0f }, //     /  |
            {  0.75f,  1.0f,  1.0f,    1.0f, 1.0f, 1.0f, 1.0f,   1.0f, 0.0f }, // 1 /____| 3
    };

    // 3 Quads for inspecting g-buffers
    mInspectVertices.InitBuffer(this, sizeof(inspect_verts), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, (const char*)&inspect_verts[0]);
    mInspectVertices.AddBinding(VERTEX_BUFFER_BIND_ID, sizeof(inspect_verts[0]), VK_VERTEX_INPUT_RATE_VERTEX);
    mInspectVertices.AddAttribute(VERTEX_BUFFER_BIND_ID, 0, 0, VK_FORMAT_R32G32B32_SFLOAT);    // float3 position
    mInspectVertices.AddAttribute(VERTEX_BUFFER_BIND_ID, 1, sizeof(float) * 3, VK_FORMAT_R32G32B32A32_SFLOAT); // float4 color
    mInspectVertices.AddAttribute(VERTEX_BUFFER_BIND_ID, 2, sizeof(float) * 7, VK_FORMAT_R32G32_SFLOAT);       // float2 uv
}

///////////////////////////////////////////////////////////////////////////////

void VkSample::InitUniformBuffers()
{
    // Create our initial MVP matrix
    glm::mat4 modelViewProjection = mProjectionMatrix * mViewMatrix;

    mVertUniformData.modelViewProjection    = modelViewProjection;
    mVertUniformData.view                   = mViewMatrix;
    mVertUniformData.model                  = mModelMatrix;
    mVertUniformData.camPos                 = glm::vec4(mCameraPos.x, mCameraPos.y, mCameraPos.z, 1.0f);

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

    // This sample has a uniform binding for vertex shader, and 2 samplers in the fragment shader
    VkDescriptorSetLayoutBinding subpass1Bindings[3] = {};
    // Our MVP matrix
    subpass1Bindings[0].binding = 0;
    subpass1Bindings[0].descriptorCount     = 1;
    subpass1Bindings[0].descriptorType      = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    subpass1Bindings[0].stageFlags          = VK_SHADER_STAGE_VERTEX_BIT;
    subpass1Bindings[0].pImmutableSamplers  = nullptr;

    // Our texture sampler
    subpass1Bindings[1].binding             = 1;
    subpass1Bindings[1].descriptorCount     = 1;
    subpass1Bindings[1].descriptorType      = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    subpass1Bindings[1].stageFlags          = VK_SHADER_STAGE_FRAGMENT_BIT;
    subpass1Bindings[1].pImmutableSamplers  = nullptr;

    // Another texture sampler
    subpass1Bindings[2].binding             = 2;
    subpass1Bindings[2].descriptorCount     = 1;
    subpass1Bindings[2].descriptorType      = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    subpass1Bindings[2].stageFlags          = VK_SHADER_STAGE_FRAGMENT_BIT;
    subpass1Bindings[2].pImmutableSamplers  = nullptr;

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
    descriptorSetLayoutCreateInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutCreateInfo.pNext         = nullptr;
    descriptorSetLayoutCreateInfo.bindingCount  = 3;
    descriptorSetLayoutCreateInfo.pBindings     = &subpass1Bindings[0];

    ret = vkCreateDescriptorSetLayout(mDevice, &descriptorSetLayoutCreateInfo, nullptr, &mDescriptorLayoutSubpass1);
    VK_CHECK(!ret);

    // Our pipeline layout simply points to the empty descriptor layout.
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
    pipelineLayoutCreateInfo.sType              = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.pNext              = nullptr;
    pipelineLayoutCreateInfo.setLayoutCount     = 1;
    pipelineLayoutCreateInfo.pSetLayouts        = &mDescriptorLayoutSubpass1;
    ret = vkCreatePipelineLayout(mDevice, &pipelineLayoutCreateInfo, nullptr, &mPipelineLayoutSubpass1);
    VK_CHECK(!ret);

    // This is the layout for the final draw, to a quad with the G-buffers as input
    // No vertex uniforms, but lights available as fragment uniforms
    VkDescriptorSetLayoutBinding subpass2Bindings[4] = {};
    // Our texture sampler - positions
    subpass2Bindings[0].binding                 = 0;
    subpass2Bindings[0].descriptorCount         = 1;
    subpass2Bindings[0].descriptorType          = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
    subpass2Bindings[0].stageFlags              = VK_SHADER_STAGE_FRAGMENT_BIT;
    subpass2Bindings[0].pImmutableSamplers      = nullptr;

    // Another texture sampler - normals
    subpass2Bindings[1].binding                 = 1;
    subpass2Bindings[1].descriptorCount         = 1;
    subpass2Bindings[1].descriptorType          = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
    subpass2Bindings[1].stageFlags              = VK_SHADER_STAGE_FRAGMENT_BIT;
    subpass2Bindings[1].pImmutableSamplers      = nullptr;

    // Another texture sampler - color
    subpass2Bindings[2].binding                 = 2;
    subpass2Bindings[2].descriptorCount         = 1;
    subpass2Bindings[2].descriptorType          = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
    subpass2Bindings[2].stageFlags              = VK_SHADER_STAGE_FRAGMENT_BIT;
    subpass2Bindings[2].pImmutableSamplers      = nullptr;

    //light positions
    subpass2Bindings[3].binding                 = 3;
    subpass2Bindings[3].descriptorCount         = 1;
    subpass2Bindings[3].descriptorType          = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    subpass2Bindings[3].stageFlags              = VK_SHADER_STAGE_FRAGMENT_BIT;
    subpass2Bindings[3].pImmutableSamplers      = nullptr;

    descriptorSetLayoutCreateInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutCreateInfo.pNext         = nullptr;
    descriptorSetLayoutCreateInfo.bindingCount  = 4;
    descriptorSetLayoutCreateInfo.pBindings     = &subpass2Bindings[0];

    ret = vkCreateDescriptorSetLayout(mDevice, &descriptorSetLayoutCreateInfo, nullptr, &mDescriptorLayoutSubpass2);
    VK_CHECK(!ret);

    // Our pipeline layout simply points to the empty descriptor layout.
    pipelineLayoutCreateInfo.sType              = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.pNext              = nullptr;
    pipelineLayoutCreateInfo.setLayoutCount     = 1;
    pipelineLayoutCreateInfo.pSetLayouts        = &mDescriptorLayoutSubpass2;
    ret = vkCreatePipelineLayout(mDevice, &pipelineLayoutCreateInfo, nullptr, &mPipelineLayoutSubpass2);
    VK_CHECK(!ret);
}

///////////////////////////////////////////////////////////////////////////////

void VkSample::InitRenderPass()
{
    // Create images which will be the targets for G-buffer data
    ImageViewObject::CreateImageView(this, mDeferredWidth, mDeferredHeight, VK_FORMAT_R32G32B32A32_SFLOAT,
                                     VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                     VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT,
                                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &mPositionTarget);

    ImageViewObject::CreateImageView(this, mDeferredWidth, mDeferredHeight, VK_FORMAT_R8G8B8A8_UNORM,
                                     VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                     VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT,
                                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &mNormalTarget);

    ImageViewObject::CreateImageView(this, mDeferredWidth, mDeferredHeight, VK_FORMAT_R8G8B8A8_UNORM,
                                     VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                     VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT,
                                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &mColorTarget);

    // The renderpass defines the attachments to the framebuffer object that gets
    // used in the pipeline. We have two attachments, the colour buffer, and the
    // depth buffer. The operations and layouts are set to defaults for this type
    // of attachment.

    // The multipass attachments are:
    // 0: Final surface
    // 1: Positions
    // 2: Normals
    // 3: Color
    // 4: Depth

    VkAttachmentDescription attachmentDescriptions[5] = {};
    attachmentDescriptions[0].flags             = 0;
    attachmentDescriptions[0].format            = mSurfaceFormat.format;
    attachmentDescriptions[0].samples           = VK_SAMPLE_COUNT_1_BIT;
    attachmentDescriptions[0].loadOp            = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachmentDescriptions[0].storeOp           = VK_ATTACHMENT_STORE_OP_STORE;
    attachmentDescriptions[0].stencilLoadOp     = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachmentDescriptions[0].stencilStoreOp    = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachmentDescriptions[0].initialLayout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    attachmentDescriptions[0].finalLayout       = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    attachmentDescriptions[1].flags             = 0;
    attachmentDescriptions[1].format            = mPositionTarget.GetFormat();
    attachmentDescriptions[1].samples           = VK_SAMPLE_COUNT_1_BIT;
    attachmentDescriptions[1].loadOp            = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachmentDescriptions[1].storeOp           = VK_ATTACHMENT_STORE_OP_STORE;
    attachmentDescriptions[1].stencilLoadOp     = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachmentDescriptions[1].stencilStoreOp    = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachmentDescriptions[1].initialLayout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    attachmentDescriptions[1].finalLayout       = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    attachmentDescriptions[2].flags             = 0;
    attachmentDescriptions[2].format            = mNormalTarget.GetFormat();
    attachmentDescriptions[2].samples           = VK_SAMPLE_COUNT_1_BIT;
    attachmentDescriptions[2].loadOp            = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachmentDescriptions[2].storeOp           = VK_ATTACHMENT_STORE_OP_STORE;
    attachmentDescriptions[2].stencilLoadOp     = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachmentDescriptions[2].stencilStoreOp    = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachmentDescriptions[2].initialLayout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    attachmentDescriptions[2].finalLayout       = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    attachmentDescriptions[3].flags             = 0;
    attachmentDescriptions[3].format            = mColorTarget.GetFormat();
    attachmentDescriptions[3].samples           = VK_SAMPLE_COUNT_1_BIT;
    attachmentDescriptions[3].loadOp            = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachmentDescriptions[3].storeOp           = VK_ATTACHMENT_STORE_OP_STORE;
    attachmentDescriptions[3].stencilLoadOp     = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachmentDescriptions[3].stencilStoreOp    = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachmentDescriptions[3].initialLayout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    attachmentDescriptions[3].finalLayout       = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    attachmentDescriptions[4].flags             = 0;
    attachmentDescriptions[4].format            = mDepthBuffers[0].format;
    attachmentDescriptions[4].samples           = VK_SAMPLE_COUNT_1_BIT;
    attachmentDescriptions[4].loadOp            = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachmentDescriptions[4].storeOp           = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachmentDescriptions[4].stencilLoadOp     = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachmentDescriptions[4].stencilStoreOp    = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachmentDescriptions[4].initialLayout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    attachmentDescriptions[4].finalLayout       = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthReference = {};
    depthReference.attachment           = 4;
    depthReference.layout               = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    // Render Subpass 1 - G-buffer generation
    VkAttachmentReference colorReferencesPass1[3] = {};
    colorReferencesPass1[0].attachment              = 1;
    colorReferencesPass1[0].layout                  = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    colorReferencesPass1[1].attachment              = 2;
    colorReferencesPass1[1].layout                  = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    colorReferencesPass1[2].attachment              = 3;
    colorReferencesPass1[2].layout                  = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpassDescriptions[2] = {};
    subpassDescriptions[0].pipelineBindPoint        = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescriptions[0].flags                    = 0;
    subpassDescriptions[0].inputAttachmentCount     = 0;
    subpassDescriptions[0].pInputAttachments        = nullptr;
    subpassDescriptions[0].colorAttachmentCount     = 3;
    subpassDescriptions[0].pColorAttachments        = &colorReferencesPass1[0];
    subpassDescriptions[0].pResolveAttachments      = nullptr;
    subpassDescriptions[0].pDepthStencilAttachment  = &depthReference;
    subpassDescriptions[0].preserveAttachmentCount  = 0;
    subpassDescriptions[0].pPreserveAttachments     = nullptr;

    // Render Subpass 2 - G-buffer input, final surface output
    VkAttachmentReference colorReferencesPass2[1] = {};
    colorReferencesPass2[0].attachment              = 0;
    colorReferencesPass2[0].layout                  = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference inputReferencesPass2[3] = {};
    inputReferencesPass2[0].attachment              = 1;
    inputReferencesPass2[0].layout                  = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    inputReferencesPass2[1].attachment              = 2;
    inputReferencesPass2[1].layout                  = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    inputReferencesPass2[2].attachment              = 3;
    inputReferencesPass2[2].layout                  = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    subpassDescriptions[1].pipelineBindPoint        = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescriptions[1].flags                    = 0;
    subpassDescriptions[1].inputAttachmentCount     = 3;
    subpassDescriptions[1].pInputAttachments        = &inputReferencesPass2[0];
    subpassDescriptions[1].colorAttachmentCount     = 1;
    subpassDescriptions[1].pColorAttachments        = &colorReferencesPass2[0];
    subpassDescriptions[1].pResolveAttachments      = nullptr;
    subpassDescriptions[1].pDepthStencilAttachment  = &depthReference;
    subpassDescriptions[1].preserveAttachmentCount  = 0;
    subpassDescriptions[1].pPreserveAttachments     = nullptr;

    VkSubpassDependency dependencies[1] = {};
    dependencies[0].srcSubpass                      = 0;
    dependencies[0].dstSubpass                      = 1;
    dependencies[0].srcAccessMask                   = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[0].dstAccessMask                   = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
    dependencies[0].srcStageMask                    = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    dependencies[0].dstStageMask                    = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

    // The renderpass itself is created with the number of subpasses, and the
    // list of attachments which those subpasses can reference.
    VkRenderPassCreateInfo renderPassCreateInfo = {};
    renderPassCreateInfo.sType                      = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.pNext                      = nullptr;
    renderPassCreateInfo.attachmentCount            = 5;
    renderPassCreateInfo.pAttachments               = attachmentDescriptions;
    renderPassCreateInfo.subpassCount               = 2;
    renderPassCreateInfo.pSubpasses                 = &subpassDescriptions[0];
    renderPassCreateInfo.dependencyCount            = 1;
    renderPassCreateInfo.pDependencies              = &dependencies[0];

    VkResult ret;
    ret = vkCreateRenderPass(mDevice, &renderPassCreateInfo, nullptr, &mRenderPass);
    VK_CHECK(!ret);
}

///////////////////////////////////////////////////////////////////////////////

void VkSample::InitPipelines()
{

    VkShaderModule sh_def_vert = CreateShaderModuleFromAsset("deferred_vert.spv");
    VkShaderModule sh_def_frag = CreateShaderModuleFromAsset("deferred_frag.spv");

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

    // We define a dynamic viewport and scissor.
    VkPipelineViewportStateCreateInfo  vp = {};
    vp.sType                = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    vp.viewportCount        = 1;
    vp.scissorCount         = 1;
    VkViewport viewport = {};
    viewport.height         = (float) mHeight;
    viewport.width          = (float) mWidth;
    viewport.minDepth       = (float) 0.0f;
    viewport.maxDepth       = (float) 1.0f;
    vp.pViewports = &viewport;

    VkRect2D scissor = {};
    scissor.extent.width    = mWidth;
    scissor.extent.height   = mHeight;
    scissor.offset.x        = 0;
    scissor.offset.y        = 0;
    vp.pScissors = &scissor;

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

    VkPipelineDepthStencilStateCreateInfo  ds = {};
    ds.sType                    = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    ds.depthTestEnable          = VK_TRUE;
    ds.depthWriteEnable         = VK_TRUE;
    ds.depthCompareOp           = VK_COMPARE_OP_LESS_OR_EQUAL;
    ds.depthBoundsTestEnable    = VK_FALSE;
    ds.back.failOp              = VK_STENCIL_OP_KEEP;
    ds.back.passOp              = VK_STENCIL_OP_KEEP;
    ds.back.compareOp           = VK_COMPARE_OP_ALWAYS;
    ds.stencilTestEnable        = VK_FALSE;
    ds.front                    = ds.back;

    // G-buffer creation
    InitPipeline(VK_NULL_HANDLE, 2, &shaderStages[0], mPipelineLayoutSubpass1, mRenderPass, 0, &visci, nullptr,
                 nullptr, &vp, nullptr, nullptr, &ds, &cb, nullptr, false, VK_NULL_HANDLE, &mPipelineSubpass1);

    vkDestroyShaderModule(mDevice, sh_def_vert, nullptr);
    vkDestroyShaderModule(mDevice, sh_def_frag, nullptr);

    VkShaderModule sh_final_vert = CreateShaderModuleFromAsset("final_vert.spv");
    VkShaderModule sh_final_frag = CreateShaderModuleFromAsset("final_frag.spv");

    visci = mQuadVertices.CreatePipelineState();

    shaderStages[0].sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[0].stage  = VK_SHADER_STAGE_VERTEX_BIT;
    shaderStages[0].module = sh_final_vert;
    shaderStages[0].pName  = "main";
    shaderStages[1].sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[1].stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
    shaderStages[1].module = sh_final_frag;
    shaderStages[1].pName  = "main";

    cb.attachmentCount = 1;

    ds.depthTestEnable = VK_FALSE;
    ds.depthWriteEnable = VK_FALSE;

    // This is the pipeline for drawing the final output.
    InitPipeline(VK_NULL_HANDLE, 2, &shaderStages[0], mPipelineLayoutSubpass2, mRenderPass, 1, &visci, nullptr,
                 nullptr, &vp, nullptr, nullptr, &ds, &cb, nullptr, false, VK_NULL_HANDLE, &mPipelineSubpass2);

    vkDestroyShaderModule(mDevice, sh_final_vert, nullptr);
    vkDestroyShaderModule(mDevice, sh_final_frag, nullptr);
}

///////////////////////////////////////////////////////////////////////////////

void VkSample::InitDescriptorPool() {
    //Create a pool with the amount of descriptors we require
    VkDescriptorPoolSize poolSize[3] = {};

    poolSize[0].type            = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    poolSize[0].descriptorCount = 10;

    poolSize[1].type            = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSize[1].descriptorCount = 10;

    poolSize[2].type            = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
    poolSize[2].descriptorCount = 10;

    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
    descriptorPoolCreateInfo.sType          = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolCreateInfo.pNext          = nullptr;
    descriptorPoolCreateInfo.maxSets        = 2;
    descriptorPoolCreateInfo.poolSizeCount  = 3;
    descriptorPoolCreateInfo.pPoolSizes     = poolSize;

    VkResult err;
    err = vkCreateDescriptorPool(mDevice, &descriptorPoolCreateInfo, NULL, &mDescriptorPool);
    VK_CHECK(!err);
}

///////////////////////////////////////////////////////////////////////////////

void VkSample::InitDescriptorSet()
{
    VkResult err;
    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
    descriptorSetAllocateInfo.sType                 = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorSetAllocateInfo.pNext                 = nullptr;
    descriptorSetAllocateInfo.descriptorPool        = mDescriptorPool;
    descriptorSetAllocateInfo.descriptorSetCount    = 1;
    descriptorSetAllocateInfo.pSetLayouts = &mDescriptorLayoutSubpass1;

    err = vkAllocateDescriptorSets(mDevice, &descriptorSetAllocateInfo, &mDescriptorSetSubpass1);
    VK_CHECK(!err);

    VkDescriptorImageInfo descriptorImageInfoPass1[2] = {};

    descriptorImageInfoPass1[0].sampler     = mTex.GetSampler();
    descriptorImageInfoPass1[0].imageView   = mTex.GetView();
    descriptorImageInfoPass1[0].imageLayout = mTex.GetLayout();

    descriptorImageInfoPass1[1].sampler     = mNormalTex.GetSampler();
    descriptorImageInfoPass1[1].imageView   = mNormalTex.GetView();
    descriptorImageInfoPass1[1].imageLayout = mNormalTex.GetLayout();

    VkWriteDescriptorSet writesS1[3] = {};

    VkDescriptorBufferInfo vertUniform = mVertShaderUniformBuffer.GetDescriptorInfo();
    writesS1[0].sType               = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writesS1[0].dstBinding          = 0;
    writesS1[0].dstSet              = mDescriptorSetSubpass1;
    writesS1[0].descriptorCount     = 1;
    writesS1[0].descriptorType      = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    writesS1[0].pBufferInfo         = &vertUniform;

    // diffuse
    writesS1[1].sType               = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writesS1[1].dstBinding          = 1;
    writesS1[1].dstSet              = mDescriptorSetSubpass1;
    writesS1[1].descriptorCount     = 1;
    writesS1[1].descriptorType      = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writesS1[1].pImageInfo          = &descriptorImageInfoPass1[0];

    // Normal map
    writesS1[2].sType               = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writesS1[2].dstBinding          = 2;
    writesS1[2].dstSet              = mDescriptorSetSubpass1;
    writesS1[2].descriptorCount     = 1;
    writesS1[2].descriptorType      = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writesS1[2].pImageInfo          = &descriptorImageInfoPass1[1];

    vkUpdateDescriptorSets(mDevice, 3, &writesS1[0], 0, nullptr);

    // This is the descriptor for the final draw, to a quad with the G-buffers as input
    descriptorSetAllocateInfo.sType                 = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorSetAllocateInfo.pNext                 = nullptr;
    descriptorSetAllocateInfo.descriptorPool        = mDescriptorPool;
    descriptorSetAllocateInfo.descriptorSetCount    = 1;
    descriptorSetAllocateInfo.pSetLayouts           = &mDescriptorLayoutSubpass2;

    err = vkAllocateDescriptorSets(mDevice, &descriptorSetAllocateInfo, &mDescriptorSetSubpass2);
    VK_CHECK(!err);

    VkDescriptorImageInfo descriptorImageInfoPass2[3] = {};

    // Input Attachments do not have samplers
    descriptorImageInfoPass2[0].sampler             = VK_NULL_HANDLE;
    descriptorImageInfoPass2[0].imageView           = mPositionTarget.GetView();
    descriptorImageInfoPass2[0].imageLayout         = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    descriptorImageInfoPass2[1].sampler             = VK_NULL_HANDLE;
    descriptorImageInfoPass2[1].imageView           = mNormalTarget.GetView();
    descriptorImageInfoPass2[1].imageLayout         = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    descriptorImageInfoPass2[2].sampler = VK_NULL_HANDLE;
    descriptorImageInfoPass2[2].imageView = mColorTarget.GetView();
    descriptorImageInfoPass2[2].imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkWriteDescriptorSet writes[4] = {};

    // position
    writes[0].sType             = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[0].dstBinding        = 0;
    writes[0].dstSet            = mDescriptorSetSubpass2;
    writes[0].descriptorCount   = 1;
    writes[0].descriptorType    = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
    writes[0].pImageInfo        = &descriptorImageInfoPass2[0];

    // Normal map
    writes[1].sType             = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[1].dstBinding        = 1;
    writes[1].dstSet            = mDescriptorSetSubpass2;
    writes[1].descriptorCount   = 1;
    writes[1].descriptorType    = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
    writes[1].pImageInfo        = &descriptorImageInfoPass2[1];

    // color map
    writes[2].sType             = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[2].dstBinding        = 2;
    writes[2].dstSet            = mDescriptorSetSubpass2;
    writes[2].descriptorCount   = 1;
    writes[2].descriptorType    = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
    writes[2].pImageInfo        = &descriptorImageInfoPass2[2];

    // lights
    VkDescriptorBufferInfo lightsInfo = mLightsUniformBuffer.GetDescriptorInfo();
    writes[3].sType             = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[3].dstBinding        = 3;
    writes[3].dstSet            = mDescriptorSetSubpass2;
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
        VkClearValue clear_values[5] = {};
        clear_values[0].color.float32[0] = 0.0f;
        clear_values[0].color.float32[1] = 0.0f;
        clear_values[0].color.float32[2] = 0.0f;
        clear_values[0].color.float32[3] = 1.0f;

        clear_values[1].color.float32[0] = 0.0f;
        clear_values[1].color.float32[1] = 0.0f;
        clear_values[1].color.float32[2] = 0.0f;
        clear_values[1].color.float32[3] = 1.0f;

        clear_values[2].color.float32[0] = 0.0f;
        clear_values[2].color.float32[1] = 0.0f;
        clear_values[2].color.float32[2] = -1.0f;
        clear_values[2].color.float32[3] = 1.0f;

        clear_values[3].color.float32[0] = 0.25f;
        clear_values[3].color.float32[1] = 0.25f;
        clear_values[3].color.float32[2] = 0.25f;
        clear_values[3].color.float32[3] = 1.0f;

        clear_values[4].depthStencil.depth      = 1.0f;
        clear_values[4].depthStencil.stencil    = 0;

        VkRenderPassBeginInfo rp_begin = {};
        rp_begin.sType                      = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        rp_begin.pNext                      = nullptr;
        rp_begin.renderPass                 = mRenderPass;
        rp_begin.framebuffer                = mFrameBuffers[i];
        rp_begin.renderArea.offset.x        = 0;
        rp_begin.renderArea.offset.y        = 0;
        rp_begin.renderArea.extent.width    = mWidth;
        rp_begin.renderArea.extent.height   = mHeight;
        rp_begin.clearValueCount            = 5;
        rp_begin.pClearValues               = clear_values;

        vkCmdBeginRenderPass(cmdBuffer, &rp_begin, VK_SUBPASS_CONTENTS_INLINE);

        // Set our pipeline. This holds all major state
        // the pipeline defines, for example, that the vertex buffer is a triangle list.
        vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineSubpass1);

        //bind out descriptor set, which handles our uniforms and samplers
        uint32_t dynamicOffsets[1]={0};
        vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayoutSubpass1, 0, 1, &mDescriptorSetSubpass1, 1, dynamicOffsets);

        // Bind our vertex buffer, with a 0 offset.
        VkDeviceSize offsets[1] = {0};
        vkCmdBindVertexBuffers(cmdBuffer, VERTEX_BUFFER_BIND_ID, 1, &mMesh.Buffer().GetBuffer(), offsets);

        // Issue a draw command
        vkCmdDraw(cmdBuffer, mMesh.GetNumVertices(), 1, 0, 0);

        vkCmdNextSubpass(cmdBuffer, VK_SUBPASS_CONTENTS_INLINE);

        // Set our pipeline. This holds all major state
        // the pipeline defines, for example, that the vertex buffer is a triangle list.
        vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineSubpass2);

        //bind out descriptor set, which handles our uniforms and samplers
        vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                mPipelineLayoutSubpass2, 0, 1, &mDescriptorSetSubpass2, 1, dynamicOffsets);

        // Bind our vertex buffer, with a 0 offset.
        vkCmdBindVertexBuffers(cmdBuffer, VERTEX_BUFFER_BIND_ID, 1, &mQuadVertices.GetBuffer(), offsets);

        // Issue a draw command, with our vertices. Full screen quad
        vkCmdDraw(cmdBuffer, 3*2, 1, 0, 0);

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
    VkImageView attachments[5] = {};
    VkFramebufferCreateInfo framebufferCreateInfo = {};
    framebufferCreateInfo.sType             = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferCreateInfo.pNext             = nullptr;
    framebufferCreateInfo.renderPass        = mRenderPass;
    framebufferCreateInfo.attachmentCount   = 5;
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
        attachments[1] = mPositionTarget.GetView();
        attachments[2] = mNormalTarget.GetView();
        attachments[3] = mColorTarget.GetView();
        attachments[4] = mDepthBuffers[i].view;

        ret = vkCreateFramebuffer(mDevice, &framebufferCreateInfo, nullptr, &mFrameBuffers[i]);
        VK_CHECK(!ret);
    }
}

bool VkSample::InitSample()
{
    // Initialize our matrices
    float aspect = (float)mWidth/(float)mHeight;
    mProjectionMatrix = glm::perspectiveRH(glm::radians(45.0f), aspect, 0.1f, 1000.0f );

    mCameraPos = glm::vec3(0.0f, 0.0f, -14.0f);
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

    InitLayouts();
    InitRenderPass();
    InitFrameBuffers();
    InitDescriptorSet();
    InitPipelines();

    BuildCmdBuffer();
}

///////////////////////////////////////////////////////////////////////////////

bool VkSample::Draw()
{
    VkResult err;

    const VkPipelineStageFlags WaitDstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    VkCommandBuffer drawbuffers[1] =
    {
            mSwapchainBuffers[mSwapchainCurrentIdx].cmdBuffer,
    };

    VkSubmitInfo submitInfo = {};
    submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext                = nullptr;
    submitInfo.waitSemaphoreCount   = 1;
    submitInfo.pWaitSemaphores      = &mBackBufferSemaphore;
    submitInfo.pWaitDstStageMask    = &WaitDstStageMask;
    submitInfo.commandBufferCount   = 1;
    submitInfo.pCommandBuffers      = &drawbuffers[0];
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores    = &mRenderCompleteSemaphore;

    err = vkQueueSubmit(mQueue, 1, &submitInfo,  VK_NULL_HANDLE);
    VK_CHECK(!err);
}

///////////////////////////////////////////////////////////////////////////////

bool VkSample::Update()
{
    static float spinAngle = 0.4f;

    glm::mat4 rotation = glm::rotate(glm::mat4(), glm::radians(spinAngle*10.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 model = mModelMatrix * rotation ;
    glm::mat4 modelView = mViewMatrix * model;
    glm::mat4 modelViewProjection = mProjectionMatrix * modelView;

    VkResult ret = VK_SUCCESS;
    uint8_t *pData;

    mVertUniformData.modelViewProjection    = modelViewProjection;
    mVertUniformData.view                   = mViewMatrix;
    mVertUniformData.model                  = model;
    mVertUniformData.camPos                 = glm::vec4(mCameraPos.x, mCameraPos.y, mCameraPos.z, 1.0f);

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

    spinAngle += 0.09f;
}

///////////////////////////////////////////////////////////////////////////////

bool VkSample::DestroySample()
{
    for (uint32_t i = 0; i < mSwapchainImageCount; i++)
    {
        vkDestroyFramebuffer(mDevice, mFrameBuffers[i], nullptr);
    }

    delete [] mFrameBuffers;

    // Destroy pipeline resources
    vkDestroyPipeline(mDevice, mPipelineSubpass1, nullptr);
    vkDestroyRenderPass(mDevice, mRenderPass, nullptr);
    vkDestroyPipelineLayout(mDevice, mPipelineLayoutSubpass2, nullptr);
    vkDestroyDescriptorSetLayout(mDevice, mDescriptorLayoutSubpass2, nullptr);

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
                LOGE("VkSample::Initialize Success");
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