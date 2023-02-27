//                  Copyright (c) 2016 QUALCOMM Technologies Inc.
//                              All Rights Reserved.

#include "sample.h"

// Sample Name
#define SAMPLE_NAME "Vulkan Sample: Multitheading"
#define SAMPLE_VERSION 1


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

void VkSample::InitUniformBuffers()
{
    if (mModelViewMatrixUniform.buf != VK_NULL_HANDLE)
    {
        return;
    }

    // the uniform in this example is a matrix in the vertex stage
    memset(&mModelViewMatrixUniform, 0, sizeof(Uniform));

    VkResult err = VK_SUCCESS;

    // Create our buffer object
    VkBufferCreateInfo bufferCreateInfo = {};
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

    //UpdateUniforms(pThreadData);

    // Bind our buffer to the memory
    err = vkBindBufferMemory(mDevice, mModelViewMatrixUniform.buf, mModelViewMatrixUniform.mem, 0);
    assert(!err);

    mModelViewMatrixUniform.bufferInfo.buffer = mModelViewMatrixUniform.buf;
    mModelViewMatrixUniform.bufferInfo.offset = 0;
    mModelViewMatrixUniform.bufferInfo.range = sizeof(glm::mat4);
}

///////////////////////////////////////////////////////////////////////////////

void VkSample::BuildCmdBuffer()
{
    // We rebuild the command buffer every frame
    // Make sure there are no fences that we are waiting for when building this frame's command buffer
    if (mFenceSubmitted[mSwapchainCurrentIdx])
    {
        vkWaitForFences(mDevice, 1, &mSubmitFences[mSwapchainCurrentIdx], true, 0xFFFFFFFFFFFFFFFF);
        mFenceSubmitted[mSwapchainCurrentIdx] = false;
        vkResetFences(mDevice,1,&mSubmitFences[mSwapchainCurrentIdx]);
    }

    VkCommandBuffer &cmdBuffer = mSwapchainBuffers[mSwapchainCurrentIdx].cmdBuffer;

    // vkBeginCommandBuffer should reset the command buffer, but Reset can be called
    // to make it more explicit.
    VkResult err;
    err = vkResetCommandBuffer(cmdBuffer, 0);
    VK_CHECK(!err);

    // Create the inheritance structure for this primary command buffer
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
    SetImageLayout(mSwapchainBuffers[mSwapchainCurrentIdx].image,
                   cmdBuffer,
                   VK_IMAGE_ASPECT_COLOR_BIT,
                   VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                   VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                   VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                   VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT);
    SetImageLayout(mDepthBuffers[mSwapchainCurrentIdx].image,
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
    rp_begin.framebuffer                    = mFrameBuffers[mSwapchainCurrentIdx];
    rp_begin.renderArea.offset.x            = 0;
    rp_begin.renderArea.offset.y            = 0;
    rp_begin.renderArea.extent.width        = mWidth;
    rp_begin.renderArea.extent.height       = mHeight;
    rp_begin.clearValueCount                = 2;
    rp_begin.pClearValues                   = clear_values;

    vkCmdBeginRenderPass(cmdBuffer, &rp_begin, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS /*VK_SUBPASS_CONTENTS_INLINE*/);

    for (uint32_t t = 0; t < mNumThreadsPerGroup; t++)
    {
        // Call each of the thread's secondary command buffers
        vkCmdExecuteCommands(cmdBuffer, 1, &mThreadData[mRenderThreadGroup][t].commandBuffer);
    }

    // Now our render pass has ended.
    vkCmdEndRenderPass(cmdBuffer);

    // As stated earlier, now transition the swapchain image to the PRESENT mode.
    SetImageLayout(mSwapchainBuffers[mSwapchainCurrentIdx].image,
                   cmdBuffer,
                   VK_IMAGE_ASPECT_COLOR_BIT,
                   VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                   VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                   VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                   VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);
    SetImageLayout(mDepthBuffers[mSwapchainCurrentIdx].image,
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

///////////////////////////////////////////////////////////////////////////////

void VkSample::InitTextures()
{
    // load the textures.  There are 3 textures used in this sample
    char* pFilenames[NUM_TEXTURES]={(char*)"bricks.ktx",(char*)"grass.ktx", (char*)"wood.ktx"};
    for (int32_t i=0; i<NUM_TEXTURES; i++)
    {
        mpTextureObjects[i] = new TextureObject();
        bool success = true;
        success = TextureObject::FromKTXFile(this, pFilenames[i],  mpTextureObjects[i]);
        assert(success);
    }
}

///////////////////////////////////////////////////////////////////////////////

void VkSample::InitThreading()
{
    // Assign the number of threads and thread groups.  Every frame a different thread group will be
    // launched to update command buffers.
    mNumThreadsPerGroup = 16;
    assert (mNumThreadsPerGroup <= MAX_NUM_THREADS_PER_GROUP);
    mNumThreadGroups    = 2;
    assert(mNumThreadGroups <= MAX_NUM_THREAD_GROUPS);

    // Set the number for frames the command buffers from each group is rendered.  When the group is
    // finished rendering, this group will start building the next set of command buffers, and the next
    // group's command buffers will be used for rendering.
    mFramesBetweenThreadRecreation = 400;

    // Set up the per thread data
    for (uint32_t g=0; g < mNumThreadGroups; g++ )
    {
        for (uint32_t t = 0; t < mNumThreadsPerGroup; t++)
        {
            ThreadData* pThreadData = &mThreadData[g][t];

            // Let the thread know which group and index it is
            pThreadData->group = g;
            pThreadData->index = t;

            // Set the data used for this thread
            pThreadData->pMutex                         = &mMutex;
            pThreadData->device                         = mDevice;
            pThreadData->pSample                        = this;
            pThreadData->device                         = mDevice;
            pThreadData->renderPass                     = mRenderPass;
            pThreadData->pModelViewMatrixUniform        = &mModelViewMatrixUniform;

            // Initialize values that the thread will set
            pThreadData->descriptorSet                  = VK_NULL_HANDLE;
            pThreadData->descriptorPool                 = VK_NULL_HANDLE;
            pThreadData->descriptorLayout               = VK_NULL_HANDLE;
            pThreadData->pipelineLayout                 = VK_NULL_HANDLE;

            pThreadData->vertices.buf                   = VK_NULL_HANDLE;
            pThreadData->textureIdx                     = t%3;
            pThreadData->positionIdx                    = t;

            // Create the thread
            mpThreads[g][t] = new Thread(ThreadFunc, pThreadData, t);
        }
    }
    mWorkingThreadGroup = 0;
    mRenderThreadGroup  = 0;

    // Launch the threads.  They will stop once they finish creating a secondary command buffer
    for (uint32_t t = 0; t < mNumThreadsPerGroup; t++)
    {
        mpThreads[mWorkingThreadGroup][t]->Run();
    }
}

///////////////////////////////////////////////////////////////////////////////

bool VkSample::InitSample()
{
    // Set up the projection matrix
    float aspect = (float) mWidth / (float) mHeight;
    mProjectionMatrix = glm::perspectiveRH(glm::radians(45.0f), aspect, 0.1f, 100.0f );
    mSpinAngle = 0.7f;

    // Initialize our camera matrices
    glm::vec3 position(0.0f, 2.5f, 40.0f);
    glm::vec3 lookat(0.0f, 0.0f, 0.0f);
    glm::vec3 up(0.0f, 1.0f, 0.0f);
    mViewMatrix = glm::lookAtRH(position, lookat, up);

    VkResult ret;

    // Create a fence to use when submitting the primary command buffers, one per swap chain image.
    for (uint32_t i = 0; i < mSwapchainImageCount; i++) {

        VkFenceCreateInfo fenceCreateInfo = {};
        fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        ret = vkCreateFence(mDevice, &fenceCreateInfo, nullptr, &mSubmitFences[i]);
        mFenceSubmitted[i] = false;
    }

    InitRenderPass();
    InitFrameBuffers();
    InitTextures();
    InitUniformBuffers();
    InitThreading();
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

void VkSample::PrepareThreads()
{
    VkResult err;

    if (mFrameIdx% mFramesBetweenThreadRecreation == 0)
    {
        // Each thread will render the cube in a different position, with a different color, and
        // texture.

        // Set the textures to be used for each thread's cube
        for (uint32_t t = 0; t < mNumThreadsPerGroup; t++)
        {
            int32_t newTextureIdx = rand() % NUM_TEXTURES;
            for (uint32_t g = 0; g < mNumThreadGroups; g++)
            {
                ThreadData *pThreadData = &mThreadData[g][t];
                pThreadData->textureIdx = newTextureIdx;
            }
        }

         // There are MAX_NUM_POSITIONS that the cubes can be placed in.  This logic
        // makes sure the cubes are placed in random and unique positions.

        // Initialize the thread positions for the first possible unique positions
        int32_t positions[MAX_NUM_POSITIONS];
        for (int32_t k = 0; k < MAX_NUM_POSITIONS; k++)
        {
            positions[k] = k < mNumThreadsPerGroup ? k : -1;
        }

        // Shuffle 100x so that the cube positions are randomized
        for (int32_t s = 0; s < 100; s++)
        {
            int32_t a = rand() % MAX_NUM_POSITIONS;
            int32_t b = rand() % MAX_NUM_POSITIONS;

            // swap a and b positions
            int32_t temp = positions[a];
            positions[a] = positions[b];
            positions[b] = temp;
        }

        // Set the final position index for each thread (same for both groups)
        for (int32_t k = 0; k < MAX_NUM_POSITIONS; k++)
        {
            if (positions[k] >= 0)
            {
                for (uint32_t g = 0; g < mNumThreadGroups; g++)
                {
                    ThreadData *pThreadData = &mThreadData[g][positions[k]];
                    pThreadData->positionIdx = k;
                }
            }
        }

        // Determine the next group of threads to put to work
        int32_t nextWorkingGroup = mWorkingThreadGroup + 1;
        if (nextWorkingGroup >= mNumThreadGroups)
        {
            nextWorkingGroup = 0;
        }

        for (uint32_t i = 0; i < mNumThreadsPerGroup; i++)
        {
            // Wait until worker thread is done
            mpThreads[mWorkingThreadGroup][i]->Join();

            // Launch each thread in the group
            mpThreads[nextWorkingGroup][i]->Run();
        }

        // Update the group values
        mRenderThreadGroup = mWorkingThreadGroup;
        mWorkingThreadGroup = nextWorkingGroup;
    }
}

///////////////////////////////////////////////////////////////////////////////

void VkSample::UpdateUniforms()
{
    VkResult ret = VK_SUCCESS;
    uint8_t *pData;

    Uniform* pUniform = &mModelViewMatrixUniform;

    ret = vkMapMemory(mDevice, pUniform->mem, 0, pUniform->allocSize, 0, (void **) &pData);
    assert(!ret);

    // Update the rotation matrices
    glm::mat4 rotationx = glm::rotate(glm::mat4(), glm::radians(mSpinAngle*3), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 rotationy = glm::rotate(glm::mat4(), glm::radians(mSpinAngle  ), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 rotationMatrix = rotationx * rotationy;
    mSpinAngle += 0.06f;

    // Build the mvp matrix
    glm::mat4 model = glm::mat4() * rotationMatrix;
    glm::mat4 modelView = mViewMatrix * model;
    glm::mat4 modelViewProjection = mProjectionMatrix * modelView;

    // copy to the buffer
    memcpy(pData, (const void*) &modelViewProjection, sizeof(modelViewProjection));
    vkUnmapMemory(mDevice, pUniform->mem);
}

///////////////////////////////////////////////////////////////////////////////

bool VkSample::Update()
{
    UpdateUniforms();

    PrepareThreads();

    // Build the command buffers every frame
    BuildCmdBuffer();
}

///////////////////////////////////////////////////////////////////////////////

bool VkSample::DestroySample()
{
    // Destroy the threads
    for (uint32_t g=0; g<mNumThreadGroups; g++ )
    {
        for (uint32_t t = 0; t < mNumThreadsPerGroup; t++)
        {
            ThreadData *pThreadData = &mThreadData[g][t];

            // Destory the layouts
            vkDestroyPipelineLayout(mDevice, pThreadData->pipelineLayout, nullptr);
            vkDestroyDescriptorSetLayout(mDevice, pThreadData->descriptorLayout, nullptr);

            // Destroy the pipeline
            vkDestroyPipeline(mDevice, pThreadData->pipeline, nullptr);

            // Free descriptor sets allocated from the descriptor pool
            vkFreeDescriptorSets(mDevice, pThreadData->descriptorPool,1, &pThreadData->descriptorSet);

            // Destroy the descriptor pool
            vkDestroyDescriptorPool(mDevice, pThreadData->descriptorPool, nullptr);

            // Destroy the vertice buffer
            vkDestroyBuffer(mDevice, pThreadData->vertices.buf, nullptr);
        }
    }

    for (uint32_t i = 0; i < mSwapchainImageCount; i++)
    {
        // Destroy the fences used when submitting the primary command buffers
        vkDestroyFence(mDevice, mSubmitFences[i], nullptr);
    }
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