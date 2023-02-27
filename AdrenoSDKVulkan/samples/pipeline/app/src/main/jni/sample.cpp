//                  Copyright (c) 2016 QUALCOMM Technologies Inc.
//                              All Rights Reserved.

#include "sample.h"

// Sample Name
#define SAMPLE_NAME "Vulkan Sample: Pipeline"
#define SAMPLE_VERSION 1

// The vertex buffer bind id, used as a constant in various places in the sample
#define VERTEX_BUFFER_BIND_ID 0

const char* VkSample::CACHE_FILE_NAME = "/data/data/com.qualcomm.vulkan.pipeline/files/vulkan_pipelineCache.bin";

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
    if (!MeshObject::LoadObj(this, "pin.obj", "pin.mtl", 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, false, VERTEX_BUFFER_BIND_ID, &mMesh))
    {
        LOGE("Error loading mesh");
        return;
    }
}

///////////////////////////////////////////////////////////////////////////////

void VkSample::InitUniformBuffers()
{
    mVertShaderUniformBuffer.InitBuffer(this, sizeof(mVertUniformData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, &mVertUniformData);
}

///////////////////////////////////////////////////////////////////////////////

void VkSample::InitLayouts()
{
    VkResult ret = VK_SUCCESS;

    VkDescriptorSetLayoutBinding uniformAndSamplerBinding[1] = {};

    // Our MVP matrix buffer
    uniformAndSamplerBinding[0].binding             = 0;
    uniformAndSamplerBinding[0].descriptorCount     = 1;
    uniformAndSamplerBinding[0].descriptorType      = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    uniformAndSamplerBinding[0].stageFlags          = VK_SHADER_STAGE_VERTEX_BIT;
    uniformAndSamplerBinding[0].pImmutableSamplers  = nullptr;

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
    descriptorSetLayoutCreateInfo.sType             = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutCreateInfo.pNext             = nullptr;
    descriptorSetLayoutCreateInfo.bindingCount      = 1;
    descriptorSetLayoutCreateInfo.pBindings         = &uniformAndSamplerBinding[0];

    ret = vkCreateDescriptorSetLayout(mDevice, &descriptorSetLayoutCreateInfo, nullptr, &mDescriptorLayout);
    VK_CHECK(!ret);

    // Our pipeline layout simply points to the descriptor layout.
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
    attachmentDescriptions[1].format            = mDepthBuffers[0].format;
    attachmentDescriptions[1].samples           = VK_SAMPLE_COUNT_1_BIT;
    attachmentDescriptions[1].loadOp            = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachmentDescriptions[1].storeOp           = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachmentDescriptions[1].stencilLoadOp     = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachmentDescriptions[1].stencilStoreOp    = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachmentDescriptions[1].initialLayout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    attachmentDescriptions[1].finalLayout       = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    // We have references to the attachment offsets, stating the layout type.
    VkAttachmentReference colorReference = {};
    colorReference.attachment                   = 0;
    colorReference.layout                       = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthReference = {};
    depthReference.attachment                   = 1;
    depthReference.layout                       = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    // There can be multiple subpasses in a renderpass, but this example has only one.
    // We set the color and depth references at the grahics bind point in the pipeline.
    VkSubpassDescription subpassDescription = {};
    subpassDescription.pipelineBindPoint        = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.flags                    = 0;
    subpassDescription.inputAttachmentCount     = 0;
    subpassDescription.pInputAttachments        = nullptr;
    subpassDescription.colorAttachmentCount     = 1;
    subpassDescription.pColorAttachments        = &colorReference;
    subpassDescription.pResolveAttachments      = nullptr;
    subpassDescription.pDepthStencilAttachment  = &depthReference;
    subpassDescription.preserveAttachmentCount  = 0;
    subpassDescription.pPreserveAttachments     = nullptr;

    // The renderpass itself is created with the number of subpasses, and the
    // list of attachments which those subpasses can reference.
    VkRenderPassCreateInfo renderPassCreateInfo = {};
    renderPassCreateInfo.sType                  = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.pNext                  = nullptr;
    renderPassCreateInfo.attachmentCount        = 2;
    renderPassCreateInfo.pAttachments           = attachmentDescriptions;
    renderPassCreateInfo.subpassCount           = 1;
    renderPassCreateInfo.pSubpasses             = &subpassDescription;
    renderPassCreateInfo.dependencyCount        = 0;
    renderPassCreateInfo.pDependencies          = nullptr;

    VkResult ret;
    ret = vkCreateRenderPass(mDevice, &renderPassCreateInfo, nullptr, &mRenderPass);
    VK_CHECK(!ret);
}

///////////////////////////////////////////////////////////////////////////////

void VkSample::InitPipelines()
{
    VkResult ret;

    // Look for a pipeline cache file on disk and load it
    char* pCache = nullptr;
    uint32_t cacheSize = 0;
    bool bCacheLoaded = LoadFileBuffer(CACHE_FILE_NAME, (void**) &pCache, &cacheSize);

    // Create the pipeline cache.  Use the initial cache data if available.
    VkPipelineCacheCreateInfo           pipelineCreateInfo;
    pipelineCreateInfo.sType            = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    pipelineCreateInfo.flags            = 0;
    pipelineCreateInfo.pNext            = nullptr;
    pipelineCreateInfo.initialDataSize  =  bCacheLoaded ? cacheSize : 0;
    pipelineCreateInfo.pInitialData     =  bCacheLoaded ? pCache :nullptr;
    ret = vkCreatePipelineCache(mDevice, &pipelineCreateInfo, nullptr, &mPipelineCache);
    VK_CHECK(!ret);

    // For each pipeline
    for (int p=0; p<NUM_PIPELINES; p++)
    {
        // Init the pipeline containing our vertex and fragment shaders
        char fileName[250];
        sprintf(fileName, "shaders/simple%d.vert.spv", p);
        VkShaderModule sh_vert = CreateShaderModuleFromAsset(fileName);

        sprintf(fileName, "shaders/simple%d.frag.spv", p);
        VkShaderModule sh_frag = CreateShaderModuleFromAsset(fileName);

        VkPipelineVertexInputStateCreateInfo visci = mMesh.Buffer().CreatePipelineState();

        VkPipelineShaderStageCreateInfo shaderStages[2] = {};
        shaderStages[0].sType       = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[0].stage       = VK_SHADER_STAGE_VERTEX_BIT;
        shaderStages[0].module      = sh_vert;
        shaderStages[0].pName       = "main";

        shaderStages[1].sType       = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[1].stage       = VK_SHADER_STAGE_FRAGMENT_BIT;
        shaderStages[1].module      = sh_frag;
        shaderStages[1].pName       = "main";

        VkPipelineRasterizationStateCreateInfo rs = {};
        rs.sType                    = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rs.polygonMode              = VK_POLYGON_MODE_LINE;
        rs.cullMode                 = VK_CULL_MODE_NONE;
        rs.frontFace                = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rs.depthClampEnable         = VK_FALSE;
        rs.rasterizerDiscardEnable  = VK_FALSE;
        rs.depthBiasEnable          = VK_FALSE;
        rs.lineWidth                = 1.0f;

        // Initialize the pipeline.
        // The first pipeline is not derived from any other.
        // The second pipeline is derived from the first pipeline.
        // The third pipeline is derived from the second pipeline.
        InitPipeline(mPipelineCache, 2, &shaderStages[0], mPipelineLayout, mRenderPass, 0, &visci,
                     nullptr,
                     nullptr, nullptr, &rs, nullptr, nullptr, nullptr, nullptr, true, p==0 ? VK_NULL_HANDLE : mPipelines[p-1], &mPipelines[p]);

        vkDestroyShaderModule(mDevice, sh_frag, nullptr);
        vkDestroyShaderModule(mDevice, sh_vert, nullptr);
    }

    // If we were able to read the cache from disk, free the buffer
    if (bCacheLoaded)
    {
        free(pCache);
    }
    else
    {
        // create a new cache file on disk
        cacheSize = 0;
        ret = vkGetPipelineCacheData(mDevice, mPipelineCache, &cacheSize, nullptr);
        VK_CHECK(!ret);

        pCache = (char*)malloc(cacheSize);
        ret = vkGetPipelineCacheData(mDevice, mPipelineCache, &cacheSize, pCache);
        VK_CHECK(!ret);


        bool bCacheSaved = SaveFileBuffer(CACHE_FILE_NAME, pCache, cacheSize);
        free(pCache);
    }
}

///////////////////////////////////////////////////////////////////////////////

void VkSample::InitDescriptorSet()
{
    // Create a pool with the amount of descriptors we require
    // This pool will have just 1 dynamic uniform buffer
    VkDescriptorPoolSize poolSize[1] = {};

    poolSize[0].type                                = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    poolSize[0].descriptorCount                     = 1;

    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
    descriptorPoolCreateInfo.sType                  = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolCreateInfo.pNext                  = nullptr;
    descriptorPoolCreateInfo.maxSets                = 1;
    descriptorPoolCreateInfo.poolSizeCount          = 1;
    descriptorPoolCreateInfo.pPoolSizes             = poolSize;

    VkResult  err;
    err = vkCreateDescriptorPool(mDevice, &descriptorPoolCreateInfo, NULL, &mDescriptorPool);
    VK_CHECK(!err);

    // Allocate 1 set from the pool
    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
    descriptorSetAllocateInfo.sType                 = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorSetAllocateInfo.pNext                 = nullptr;
    descriptorSetAllocateInfo.descriptorPool        = mDescriptorPool;
    descriptorSetAllocateInfo.descriptorSetCount    = 1;
    descriptorSetAllocateInfo.pSetLayouts           = &mDescriptorLayout;
    err = vkAllocateDescriptorSets(mDevice, &descriptorSetAllocateInfo, &mDescriptorSet);
    VK_CHECK(!err);

    // Update the descriptor set with the vertex shader uniform buffer information
    VkWriteDescriptorSet writes[0] = {};
    VkDescriptorBufferInfo vertUniform = mVertShaderUniformBuffer.GetDescriptorInfo();
    writes[0].sType                                 = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[0].dstBinding                            = 0;
    writes[0].dstSet                                = mDescriptorSet;
    writes[0].descriptorCount                       = 1;
    writes[0].descriptorType                        = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    writes[0].pBufferInfo                           = &vertUniform;
    vkUpdateDescriptorSets(mDevice, 1, writes, 0, nullptr);
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

        // for each pipeline
        for (uint32_t p=0; p< NUM_PIPELINES; p++)
        {
            // Set our pipeline. This holds all major state
            // the pipeline defines, for example, that the vertex buffer is a triangle list.
            vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelines[p]);

            //bind out descriptor set, which handles our uniforms and samplers
            uint32_t dynOffsets[1]={0};
            vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout, 0, 1, &mDescriptorSet, 1, dynOffsets);

            // Bind our vertex buffer, with a 0 offset.
            VkDeviceSize offsets[1] = {0};
            vkCmdBindVertexBuffers(cmdBuffer, VERTEX_BUFFER_BIND_ID, 1, &mMesh.Buffer().GetBuffer(), offsets);

            // Issue a draw command, with our 3 vertices.
            vkCmdDraw(cmdBuffer, mMesh.GetNumVertices(), 1, 0, 0);
        }

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
    // The framebuffer objects reference the renderpass, and allow
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

///////////////////////////////////////////////////////////////////////////////

bool VkSample::InitSample()
{
    // Initialize our matrices
    float aspect = (float)mWidth/(float)mHeight;
    mProjectionMatrix = glm::perspectiveRH(glm::radians(45.0f), aspect, 0.1f, 1000.0f );

    // Set up our camera and vertex uniform data
    mVertUniformData.camPos = glm::vec3(0.0f, 20.0f, 100.0f);
    glm::vec3 lookat(0.0f,  20.0f, 0.0f);
    glm::vec3 up(0.0f, -1.0f, 0.0f);
    mVertUniformData.view = glm::lookAtRH(mVertUniformData.camPos, lookat, up);
    mVertUniformData.model = glm::mat4(1.0f);
    mVertUniformData.modelViewProjection = mProjectionMatrix * mVertUniformData.view * mVertUniformData.model;

    InitVertexBuffers();
    InitUniformBuffers();
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

}

///////////////////////////////////////////////////////////////////////////////

bool VkSample::DestroySample()
{
    // Destroy the pipeline cache
    vkDestroyPipelineCache(mDevice, mPipelineCache, nullptr);

    // Destroy pipeline resources
    for (uint_t p=0; p< NUM_PIPELINES; p++)
    {
        vkDestroyPipeline(mDevice, mPipelines[p], nullptr);
    }
    vkDestroyPipelineLayout(mDevice, mPipelineLayout, nullptr);

    vkDestroyRenderPass(mDevice, mRenderPass, nullptr);
    vkDestroyDescriptorSetLayout(mDevice, mDescriptorLayout, nullptr);

    // Destroy pool and all sets allocated from it.
    vkDestroyDescriptorPool(mDevice, mDescriptorPool,nullptr);

    // destroy mesh
    mMesh.Destroy();

    // Destroy uniform
    mVertShaderUniformBuffer.Destroy();

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
