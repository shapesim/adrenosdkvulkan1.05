//                  Copyright (c) 2016 QUALCOMM Technologies Inc.
//                              All Rights Reserved.

#include "sample.h"

// Sample Name
#define SAMPLE_NAME "Vulkan Sample: Compute Shader"
#define SAMPLE_VERSION 1

// The vertex buffer bind id, used as a constant in various places in the sample
#define VERTEX_BUFFER_BIND_ID 0

VkSample::VkSample()
    : VkSampleFramework(SAMPLE_NAME, SAMPLE_VERSION)
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
    // Generate a grid of vertices, with index buffer
    const int gridSize = 64;
    const float scale = 0.2f;

    std::vector<vertex_layout> vertices;
    vertices.reserve(gridSize*gridSize);

    for (uint32_t y = 0; y < gridSize; y++)
    {
        for (uint32_t x = 0; x< gridSize; x++)
        {
            vertex_layout v;
            v.pos = glm::vec4( x * scale, 1.0f , y * scale, 1.0);
            float size = gridSize*scale;
            v.color = v.pos / glm::vec4(size, size, size, 1.0);
            v.uv.x = 1.0f - v.color.x;
            v.uv.y = v.color.z;
            vertices.push_back(v);
        }
    }

    m_surfaceVertices.InitBuffer(this, sizeof(vertex_layout) * vertices.size(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, (const char*)&vertices[0]);
    m_surfaceVertices.AddBinding(VERTEX_BUFFER_BIND_ID, sizeof(vertex_layout), VK_VERTEX_INPUT_RATE_VERTEX);
    m_surfaceVertices.AddAttribute(VERTEX_BUFFER_BIND_ID, 0, 0, VK_FORMAT_R32G32B32_SFLOAT);                    // float3 position
    m_surfaceVertices.AddAttribute(VERTEX_BUFFER_BIND_ID, 1, sizeof(glm::vec4) , VK_FORMAT_R32G32B32A32_SFLOAT); // float4 color
    m_surfaceVertices.AddAttribute(VERTEX_BUFFER_BIND_ID, 2, sizeof(glm::vec4) * 2, VK_FORMAT_R32G32_SFLOAT);       // float2 uv

    std::vector<uint32_t> indices;

    // mesh of gridSize-1 quads
    for (uint32_t y = 0; y < gridSize-1; y++)
    {
        for (uint32_t x = 0; x < gridSize-1; x++)
        {
            // triangle 1
            indices.push_back(y * gridSize + x);
            indices.push_back((y+1)*gridSize + x);
            indices.push_back(y * gridSize + x + 1);
            // triangle 2
            indices.push_back((y+1)*gridSize + x);
            indices.push_back(y * gridSize + x + 1);
            indices.push_back((y+1)*gridSize + x + 1);
        }
    }

    m_surfaceIndexCount = indices.size();
    m_surfaceIndices.InitBuffer(this, sizeof(uint32_t) * indices.size(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT, (const char*)&indices[0]);
}

///////////////////////////////////////////////////////////////////////////////

void VkSample::InitUniformBuffers()
{
    m_vertexUniformBuffer.InitBuffer(this, sizeof(m_vertexUniform), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, &m_vertexUniform);

    m_storageBuffer.InitBuffer(this, sizeof(m_storageData), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, &m_storageData);
}

///////////////////////////////////////////////////////////////////////////////

void VkSample::InitTextures()
{
    bool success = true;
    success |= TextureObject::FromKTXFile(this, "vkbox.ktx", &m_texVkLogo);
    assert(success);
}

///////////////////////////////////////////////////////////////////////////////

void VkSample::InitLayouts()
{
    VkResult ret = VK_SUCCESS;

    // This sample has 2 bindings
    VkDescriptorSetLayoutBinding uniformAndSamplerBinding[2] = {};
    //mvp
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

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
    descriptorSetLayoutCreateInfo.sType             = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutCreateInfo.pNext             = nullptr;
    descriptorSetLayoutCreateInfo.bindingCount      = 2;
    descriptorSetLayoutCreateInfo.pBindings         = &uniformAndSamplerBinding[0];

    ret = vkCreateDescriptorSetLayout(mDevice, &descriptorSetLayoutCreateInfo, nullptr, &mDescriptorLayout);
    VK_CHECK(!ret);

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
    pipelineLayoutCreateInfo.sType              = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.pNext              = nullptr;
    pipelineLayoutCreateInfo.setLayoutCount     = 1;
    pipelineLayoutCreateInfo.pSetLayouts        = &mDescriptorLayout;
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
    attachmentDescriptions[0].flags = 0;
    attachmentDescriptions[0].format = mSurfaceFormat.format;
    attachmentDescriptions[0].samples = VK_SAMPLE_COUNT_1_BIT;
    attachmentDescriptions[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachmentDescriptions[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachmentDescriptions[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachmentDescriptions[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachmentDescriptions[0].initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    attachmentDescriptions[0].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    attachmentDescriptions[1].flags = 0;
    attachmentDescriptions[1].format = mDepthBuffers[0].format;
    attachmentDescriptions[1].samples = VK_SAMPLE_COUNT_1_BIT;
    attachmentDescriptions[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachmentDescriptions[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachmentDescriptions[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachmentDescriptions[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachmentDescriptions[1].initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    attachmentDescriptions[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    // We have references to the attachment offsets, stating the layout type.
    VkAttachmentReference colorReference = {};
    colorReference.attachment = 0;
    colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthReference = {};
    depthReference.attachment = 1;
    depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    // There can be multiple subpasses in a renderpass, but this example has only one.
    // We set the color and depth references at the grahics bind point in the pipeline.
    VkSubpassDescription subpassDescription = {};
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.flags = 0;
    subpassDescription.inputAttachmentCount = 0;
    subpassDescription.pInputAttachments = nullptr;
    subpassDescription.colorAttachmentCount = 1;
    subpassDescription.pColorAttachments = &colorReference;
    subpassDescription.pResolveAttachments = nullptr;
    subpassDescription.pDepthStencilAttachment = &depthReference;
    subpassDescription.preserveAttachmentCount = 0;
    subpassDescription.pPreserveAttachments = nullptr;

    // The renderpass itself is created with the number of subpasses, and the
    // list of attachments which those subpasses can reference.
    VkRenderPassCreateInfo renderPassCreateInfo = {};
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.pNext = nullptr;
    renderPassCreateInfo.attachmentCount = 2;
    renderPassCreateInfo.pAttachments = attachmentDescriptions;
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pSubpasses = &subpassDescription;
    renderPassCreateInfo.dependencyCount = 0;
    renderPassCreateInfo.pDependencies = nullptr;

    VkResult ret;
    ret = vkCreateRenderPass(mDevice, &renderPassCreateInfo, nullptr, &mRenderPass);
    VK_CHECK(!ret);
}

///////////////////////////////////////////////////////////////////////////////

void VkSample::InitPipelines()
{

    // Init the pipeline containing our cube vertex and fragment shaders
    VkShaderModule sh_vert = CreateShaderModuleFromAsset("shaders/simple.vert.spv");
    VkShaderModule sh_frag = CreateShaderModuleFromAsset("shaders/simple.frag.spv");

    VkPipelineVertexInputStateCreateInfo visci = m_surfaceVertices.CreatePipelineState();

    VkPipelineShaderStageCreateInfo shaderStages[2] = {};
    shaderStages[0].sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[0].stage  = VK_SHADER_STAGE_VERTEX_BIT;
    shaderStages[0].module = sh_vert;
    shaderStages[0].pName  = "main";
    shaderStages[1].sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[1].stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
    shaderStages[1].module = sh_frag;
    shaderStages[1].pName  = "main";

    VkPipelineRasterizationStateCreateInfo rs = {};
    rs.sType                    = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rs.polygonMode              = VK_POLYGON_MODE_LINE;
    rs.cullMode                 = VK_CULL_MODE_NONE;
    rs.frontFace                = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rs.depthClampEnable         = VK_FALSE;
    rs.rasterizerDiscardEnable  = VK_FALSE;
    rs.depthBiasEnable          = VK_FALSE;
    rs.lineWidth                = 1.0f;

    InitPipeline(VK_NULL_HANDLE, 2, &shaderStages[0], mPipelineLayout, mRenderPass, 0, &visci, nullptr,
                 nullptr, nullptr, &rs, nullptr, nullptr, nullptr, nullptr, false, VK_NULL_HANDLE, &mPipeline);

    vkDestroyShaderModule(mDevice, sh_frag, nullptr);
    vkDestroyShaderModule(mDevice, sh_vert, nullptr);
}
///////////////////////////////////////////////////////////////////////////////

void VkSample::InitDescriptorSet()
{
    //Create a pool with the amount of descriptors we require
    VkDescriptorPoolSize poolSize[2] = {};

    poolSize[0].type                        = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    poolSize[0].descriptorCount             = 1;
    poolSize[1].type                        = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSize[1].descriptorCount             = 1;

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

    descriptorImageInfo.sampler                     = m_texVkLogo.GetSampler();
    descriptorImageInfo.imageView                   = m_texVkLogo.GetView();
    descriptorImageInfo.imageLayout                 = VK_IMAGE_LAYOUT_GENERAL;

    VkWriteDescriptorSet writes[2] = {};
    //mvp
    VkDescriptorBufferInfo bufferInfo = m_vertexUniformBuffer.GetDescriptorInfo();
    writes[0].sType             = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[0].dstBinding        = 0;
    writes[0].dstSet            = mDescriptorSet;
    writes[0].descriptorCount   = 1;
    writes[0].descriptorType    = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    writes[0].pBufferInfo       = &bufferInfo;
    // texture
    writes[1].sType             = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[1].dstBinding        = 1;
    writes[1].dstSet            = mDescriptorSet;
    writes[1].descriptorCount   = 1;
    writes[1].descriptorType    = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writes[1].pImageInfo        = &descriptorImageInfo;

    vkUpdateDescriptorSets(mDevice, 2, &writes[0], 0, nullptr);
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

        // Bind our vertex buffer, with a 0 offset.
        VkDeviceSize offsets[1] = {0};
        vkCmdBindVertexBuffers(cmdBuffer, VERTEX_BUFFER_BIND_ID, 1, &m_surfaceVertices.GetBuffer(), offsets);

        vkCmdBindIndexBuffer(cmdBuffer, m_surfaceIndices.GetBuffer(), 0, VK_INDEX_TYPE_UINT32);

        // Issue a draw command
        vkCmdDrawIndexed(cmdBuffer, m_surfaceIndexCount, 1, 0, 0, 0);

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

bool VkSample::InitSample()
{
    InitTextures();

    // Initialize our matrices
    float aspect = (float)mWidth/(float)mHeight;
    m_projectionMatrix = glm::perspectiveRH(glm::radians(45.0f), aspect, 0.1f, 1000.0f );

    glm::vec3 camera(6.0f, -8.0f, -4.5f);
    glm::vec3 lookat(6.0f, 0.0f, 3.5f);
    glm::vec3 up(0.0f, 1.0f, 0.0f);
    m_modelViewMatrix = glm::lookAtRH( camera, lookat, up);
    m_vertexUniform.mvp = m_projectionMatrix * m_modelViewMatrix;

    InitUniformBuffers();
    InitVertexBuffers();
    InitLayouts();
    InitDescriptorSet();

    InitRenderPass();
    InitPipelines();
    InitFrameBuffers();

    InitComputeCmdBuffer();

    InitComputeLayouts();
    InitComputeDescriptorSet();
    InitComputePipeline();

    BuildComputeCmdBuffer();
    BuildCmdBuffer();
}

///////////////////////////////////////////////////////////////////////////////

void VkSample::InitComputeCmdBuffer()
{
    // Check if the current queue family supports compute operations.
    // This sample assumes this is the case - if the current queue did not support compute,
    // we'd need to create a new queue and submit our compute command buffers to that queue.

    uint32_t numQueueFamilies = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &numQueueFamilies, nullptr);

    VkQueueFamilyProperties* properties = new VkQueueFamilyProperties[numQueueFamilies];
    assert(properties);
    vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &numQueueFamilies, properties);

    // Sample assumes the current queue has compute capability
    assert(properties[mQueueFamilyIndex].queueFlags & VK_QUEUE_COMPUTE_BIT);

    delete [] properties;

    // Command buffers are allocated from a pool; we define that pool here and create it.
    VkCommandPoolCreateInfo commandPoolCreateInfo = {};
    commandPoolCreateInfo.sType                     = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.pNext                     = nullptr;
    commandPoolCreateInfo.queueFamilyIndex          = mQueueFamilyIndex;
    commandPoolCreateInfo.flags                     = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    VkResult ret = vkCreateCommandPool(mDevice, &commandPoolCreateInfo, nullptr, &mComputeCommandPool);
    VK_CHECK(!ret);

    VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
    commandBufferAllocateInfo.sType                 = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.pNext                 = nullptr;
    commandBufferAllocateInfo.commandPool           = mComputeCommandPool;
    commandBufferAllocateInfo.level                 = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandBufferCount    = 1;

    ret = vkAllocateCommandBuffers(mDevice, &commandBufferAllocateInfo, &mComputeCmdBuffer);
    VK_CHECK(!ret);
}

///////////////////////////////////////////////////////////////////////////////

void VkSample::InitComputeLayouts()
{
    VkResult ret = VK_SUCCESS;

    // Compute stage has a image input and image output
    VkDescriptorSetLayoutBinding computeBindings[2] = {};

    computeBindings[0].binding                  = 0;
    computeBindings[0].descriptorCount          = 1;
    computeBindings[0].descriptorType           = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    computeBindings[0].stageFlags               = VK_SHADER_STAGE_COMPUTE_BIT;
    computeBindings[0].pImmutableSamplers       = nullptr;

    computeBindings[1].binding                  = 1;
    computeBindings[1].descriptorCount          = 1;
    computeBindings[1].descriptorType           = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    computeBindings[1].stageFlags               = VK_SHADER_STAGE_COMPUTE_BIT;
    computeBindings[1].pImmutableSamplers       = nullptr;

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
    descriptorSetLayoutCreateInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutCreateInfo.pNext         = nullptr;
    descriptorSetLayoutCreateInfo.bindingCount  = 2;
    descriptorSetLayoutCreateInfo.pBindings     = &computeBindings[0];

    ret = vkCreateDescriptorSetLayout(mDevice, &descriptorSetLayoutCreateInfo, nullptr, &mComputeDescLayout);
    VK_CHECK(!ret);

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
    pipelineLayoutCreateInfo.sType              = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.pNext              = nullptr;
    pipelineLayoutCreateInfo.setLayoutCount     = 1;
    pipelineLayoutCreateInfo.pSetLayouts        = &mComputeDescLayout;
    ret = vkCreatePipelineLayout(mDevice, &pipelineLayoutCreateInfo, nullptr, &mComputePipelineLayout);
    VK_CHECK(!ret);
}

///////////////////////////////////////////////////////////////////////////////

void VkSample::InitComputeDescriptorSet()
{
    //Create a pool with the amount of descriptors we require
    VkDescriptorPoolSize poolSize[2] = {};
    poolSize[0].type                                = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    poolSize[0].descriptorCount                     = 2;
    poolSize[1].type                                = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    poolSize[1].descriptorCount                     = 2;

    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
    descriptorPoolCreateInfo.sType                  = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolCreateInfo.pNext                  = nullptr;
    descriptorPoolCreateInfo.maxSets                = 1;
    descriptorPoolCreateInfo.poolSizeCount          = 2;
    descriptorPoolCreateInfo.pPoolSizes             = poolSize;

    VkResult  err;
    err = vkCreateDescriptorPool(mDevice, &descriptorPoolCreateInfo, NULL, &mComputeDescriptorPool);
    VK_CHECK(!err);

    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
    descriptorSetAllocateInfo.sType                 = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorSetAllocateInfo.pNext                 = nullptr;
    descriptorSetAllocateInfo.descriptorPool        = mComputeDescriptorPool;
    descriptorSetAllocateInfo.descriptorSetCount    = 1;
    descriptorSetAllocateInfo.pSetLayouts           = &mComputeDescLayout;

    err = vkAllocateDescriptorSets(mDevice, &descriptorSetAllocateInfo, &mComputeDescriptorSet);
    VK_CHECK(!err);

    VkWriteDescriptorSet writes[2] = {};

    VkDescriptorBufferInfo vertBufferInfo =  m_surfaceVertices.GetDescriptorInfo();
    writes[0].sType                                 = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[0].dstBinding                            = 0;
    writes[0].dstSet                                = mComputeDescriptorSet;
    writes[0].descriptorCount                       = 1;
    writes[0].descriptorType                        = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    writes[0].pBufferInfo                           = &vertBufferInfo;

    VkDescriptorBufferInfo detailBufferInfo =  m_storageBuffer.GetDescriptorInfo();
    writes[1].sType                                 = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[1].dstBinding                            = 1;
    writes[1].dstSet                                = mComputeDescriptorSet;
    writes[1].descriptorCount                       = 1;
    writes[1].descriptorType                        = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    writes[1].pBufferInfo                           = &detailBufferInfo;

    vkUpdateDescriptorSets(mDevice, 2, &writes[0], 0, nullptr);
}

///////////////////////////////////////////////////////////////////////////////

void VkSample::InitComputePipeline()
{
    VkShaderModule sh_compute = CreateShaderModuleFromAsset("shaders/compute.comp.spv");

    VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfo = {};
    pipelineShaderStageCreateInfo.sType             = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    pipelineShaderStageCreateInfo.stage             = VK_SHADER_STAGE_COMPUTE_BIT;
    pipelineShaderStageCreateInfo.module            = sh_compute;
    pipelineShaderStageCreateInfo.pName             = "main";

    VkComputePipelineCreateInfo computePipelineCreateInfo = {};
    computePipelineCreateInfo.sType                 = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    computePipelineCreateInfo.layout                = mComputePipelineLayout;
    computePipelineCreateInfo.stage                 = pipelineShaderStageCreateInfo;

    VkResult ret = vkCreateComputePipelines(mDevice, VK_NULL_HANDLE, 1, &computePipelineCreateInfo,
                                            nullptr, &mComputePipeline);
    VK_CHECK(!ret);

    vkDestroyShaderModule(mDevice, sh_compute, nullptr);
}

///////////////////////////////////////////////////////////////////////////////

void VkSample::BuildComputeCmdBuffer()
{
    VkResult ret = VK_SUCCESS;
    ret = vkResetCommandBuffer(mComputeCmdBuffer, 0);
    VK_CHECK(!ret);

    VkCommandBufferInheritanceInfo cmd_buf_hinfo = {};
    cmd_buf_hinfo.sType                         = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
    cmd_buf_hinfo.pNext                         = nullptr;
    cmd_buf_hinfo.renderPass                    = VK_NULL_HANDLE;
    cmd_buf_hinfo.subpass                       = 0;
    cmd_buf_hinfo.framebuffer                   = VK_NULL_HANDLE;
    cmd_buf_hinfo.occlusionQueryEnable          = VK_FALSE;
    cmd_buf_hinfo.queryFlags                    = 0;
    cmd_buf_hinfo.pipelineStatistics            = 0;

    VkCommandBufferBeginInfo cmd_buf_info = {};
    cmd_buf_info.sType                          = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmd_buf_info.pNext                          = nullptr;
    cmd_buf_info.flags                          = 0;
    cmd_buf_info.pInheritanceInfo               = &cmd_buf_hinfo;

    ret = vkBeginCommandBuffer(mComputeCmdBuffer, &cmd_buf_info);
    VK_CHECK(!ret);

    vkCmdBindPipeline(mComputeCmdBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, mComputePipeline);

    vkCmdBindDescriptorSets(mComputeCmdBuffer, VK_PIPELINE_BIND_POINT_COMPUTE,
                            mComputePipelineLayout, 0, 1, &mComputeDescriptorSet, 0, nullptr);

    vkCmdDispatch(mComputeCmdBuffer, 16, 16, 1);

    ret = vkEndCommandBuffer(mComputeCmdBuffer);
    VK_CHECK(!ret);
}

///////////////////////////////////////////////////////////////////////////////

bool VkSample::Draw()
{
    VkCommandBuffer drawbuffers[2] = {
            mComputeCmdBuffer,
            mSwapchainBuffers[mSwapchainCurrentIdx].cmdBuffer,
    };

    const VkPipelineStageFlags WaitDstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    VkSubmitInfo submitInfo = {};
    submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext                = nullptr;
    submitInfo.waitSemaphoreCount   = 1;
    submitInfo.pWaitSemaphores      = &mBackBufferSemaphore;
    submitInfo.pWaitDstStageMask    = &WaitDstStageMask;
    submitInfo.commandBufferCount   = 2;
    submitInfo.pCommandBuffers      = &drawbuffers[0];
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores    = &mRenderCompleteSemaphore;

    VkResult err;
    err = vkQueueSubmit(mQueue, 1, &submitInfo,  VK_NULL_HANDLE);
    VK_CHECK(!err);
}

///////////////////////////////////////////////////////////////////////////////

bool VkSample::Update()
{
    vkDeviceWaitIdle(mDevice);

    static float time = 0.0f;

    VkResult ret = VK_SUCCESS;
    uint8_t *pData;

    ret = vkMapMemory(mDevice, m_storageBuffer.GetDeviceMemory(), 0, m_storageBuffer.GetAllocationSize(), 0, (void **) &pData);
    assert(!ret);

    m_storageData.time = time;

    memcpy(pData, &m_storageData, sizeof(m_storageData));

    vkUnmapMemory(mDevice, m_storageBuffer.GetDeviceMemory());

    time += 1.5f;
}

///////////////////////////////////////////////////////////////////////////////

bool VkSample::DestroySample()
{
    // Destroy pipeline resources
    vkDestroyPipeline(mDevice, mPipeline, nullptr);
    vkDestroyPipeline(mDevice, mComputePipeline, nullptr);
    vkDestroyPipelineLayout(mDevice, mComputePipelineLayout, nullptr);
    vkDestroyPipelineLayout(mDevice, mPipelineLayout, nullptr);

    vkDestroyRenderPass(mDevice, mRenderPass, nullptr);
    vkDestroyDescriptorSetLayout(mDevice, mDescriptorLayout, nullptr);
    vkDestroyDescriptorPool(mDevice,mDescriptorPool, nullptr);

    vkDestroyCommandPool(mDevice, mComputeCommandPool, nullptr);

    // Destroy verts
    m_surfaceVertices.Destroy();
    m_surfaceIndices.Destroy();

    // Destroy uniform
    m_vertexUniformBuffer.Destroy();

    m_storageBuffer.Destroy();

    // Destroy Textures
    m_texVkLogo.Destroy();
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
