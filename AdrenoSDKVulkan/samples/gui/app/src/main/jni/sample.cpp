//                  Copyright (c) 2016 QUALCOMM Technologies Inc.
//                              All Rights Reserved.

#include "sample.h"

// Sample Name
#define SAMPLE_NAME "Vulkan Sample: Gui"
#define SAMPLE_VERSION 1

VkSample::VkSample() : VkSampleFramework(SAMPLE_NAME, SAMPLE_VERSION)
{
    SetUseValidation(true);

    // Create a Gui object
    mGui = new Gui(this);
}

///////////////////////////////////////////////////////////////////////////////

VkSample::~VkSample()
{
    delete mGui;
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

void VkSample::BuildCmdBuffer()
{
    // We rebuild the command buffer every frame
    // Make sure there are no fences that we are waiting for when building this frame's command buffer
    if (mFenceSubmitted[mSwapchainCurrentIdx])
    {
        vkWaitForFences(mDevice, 1, &mSubmitFences[mSwapchainCurrentIdx], true, 0xFFFFFFFFFFFFFFFF);
        mFenceSubmitted[mSwapchainCurrentIdx] = false;
        vkResetFences(mDevice, 1, &mSubmitFences[mSwapchainCurrentIdx]);
    }

    VkCommandBuffer &cmdBuffer = mSwapchainBuffers[mSwapchainCurrentIdx].cmdBuffer;

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
    cmd_buf_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmd_buf_info.pNext = nullptr;
    cmd_buf_info.flags = 0;
    cmd_buf_info.pInheritanceInfo = &cmd_buf_hinfo;

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
    clear_values[0].color.float32[0]        = 0.0f;
    clear_values[0].color.float32[1]        = 1.0f;
    clear_values[0].color.float32[2]        = 1.0f;
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

    // Run the empty render pass to fill in the framebuffer
    vkCmdBeginRenderPass(cmdBuffer, &rp_begin, VK_SUBPASS_CONTENTS_INLINE);

    // Draw the Gui
    mGui->Draw(cmdBuffer);

    vkCmdEndRenderPass(cmdBuffer);

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

bool VkSample::InitSample()
{
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

    // Initialize the Gui using our renderpass
    mGui->Initialize(mRenderPass);

    return true;
}

///////////////////////////////////////////////////////////////////////////////

bool VkSample::Draw()
{
    // Build the command buffers every frame
    BuildCmdBuffer();

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
    err = vkQueueSubmit(mQueue, 1, &submitInfo, mSubmitFences[mSwapchainCurrentIdx]);
    mFenceSubmitted[mSwapchainCurrentIdx] = true;
    VK_CHECK(!err);
}

///////////////////////////////////////////////////////////////////////////////

bool VkSample::Update()
{
    // Update imgui with mouse state
    ImGuiIO &io = ImGui::GetIO();
    io.MousePos.x   = mTouchX;
    io.MousePos.y   = mTouchY;
    io.MouseDown[0] = mTouchDown;

    // Increase the font size a bit to make the UI more readable on a mobile device..
    io.FontGlobalScale = 2.0;

    // Draw the GUI using ImGui
    ImGui::NewFrame();
    ImGui::ShowTestWindow();
    ImGui::Render();

    // Update the GUI object
    mGui->Update();
}

///////////////////////////////////////////////////////////////////////////////

bool VkSample::DestroySample()
{
    mGui->Destroy();

    vkDestroyRenderPass(mDevice, mRenderPass, nullptr);

    for (uint32_t i = 0; i < mSwapchainImageCount; i++)
    {
        vkDestroyFramebuffer(mDevice, mFrameBuffers[i], nullptr);

        // Destroy the fences used when submitting the primary command buffers
        vkDestroyFence(mDevice, mSubmitFences[i], nullptr);
    }
}

///////////////////////////////////////////////////////////////////////////////
// Begin Android Glue entry point

/**
 * Shared state for our app.
 */
struct engine {
    struct android_app* app;
    int animating;
    VkSampleFramework* sample;
};

/**
 * Process the next main command.
 */
static void engine_handle_cmd(struct android_app* app, int32_t cmd) {
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

static int engine_handle_input(struct android_app* app, AInputEvent* event)
{
    struct engine* engine = (struct engine*)app->userData;
    float fltWidth  = (float)ANativeWindow_getWidth(app->window);
    float fltHeight = (float)ANativeWindow_getHeight(app->window);

    // Touch the screen
    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION)
    {
        float fltOneX = 0.0f;
        float fltOneY = 0.0f;

        int iPointerAction = AMotionEvent_getAction(event);
        int iPointerIndx = (iPointerAction & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
        int iPointerID = AMotionEvent_getPointerId(event, iPointerIndx);
        int iAction = (iPointerAction & AMOTION_EVENT_ACTION_MASK);
        switch( iAction )
        {
            case AMOTION_EVENT_ACTION_POINTER_DOWN:
            case AMOTION_EVENT_ACTION_DOWN:
                fltOneX = AMotionEvent_getX(event, iPointerIndx);
                fltOneY = AMotionEvent_getY(event, iPointerIndx);
                engine->sample->PointerDownEvent(iPointerID, fltOneX / fltWidth , fltOneY / fltHeight);
                break;

            case AMOTION_EVENT_ACTION_POINTER_UP:
            case AMOTION_EVENT_ACTION_UP:
                //LogInfo("AMOTION_EVENT_ACTION_[POINTER]_UP: 0x%x", iPointerID);
                fltOneX = AMotionEvent_getX(event, iPointerIndx);
                fltOneY = AMotionEvent_getY(event, iPointerIndx);
                engine->sample->PointerUpEvent(iPointerID, fltOneX / fltWidth , fltOneY / fltHeight);
                break;

            case AMOTION_EVENT_ACTION_MOVE: {
                int iHistorySize = AMotionEvent_getHistorySize(event);
                int iPointerCount = AMotionEvent_getPointerCount(event);
                for (int iHistoryIndx = 0; iHistoryIndx < iHistorySize; iHistoryIndx++) {
                    for (int iPointerIndx = 0; iPointerIndx < iPointerCount; iPointerIndx++) {
                        iPointerID = AMotionEvent_getPointerId(event, iPointerIndx);
                        fltOneX = AMotionEvent_getHistoricalX(event, iPointerIndx, iHistoryIndx);
                        fltOneY = AMotionEvent_getHistoricalY(event, iPointerIndx, iHistoryIndx);
                        // LogInfo("    PointerID %d => (%0.2f, %0.2f)", iPointerID, fltOneX, fltOneY);
                    }
                }

                for (int iPointerIndx = 0; iPointerIndx < iPointerCount; iPointerIndx++) {
                    iPointerID = AMotionEvent_getPointerId(event, iPointerIndx);
                    fltOneX = AMotionEvent_getX(event, iPointerIndx);
                    fltOneY = AMotionEvent_getY(event, iPointerIndx);
                    // LogInfo("    PointerID %d => (%0.2f, %0.2f)", iPointerID, fltOneX, fltOneY);
                    engine->sample->PointerMoveEvent(iPointerID, fltOneX / fltWidth, fltOneY / fltHeight);
                }
                }
                break;

            case AMOTION_EVENT_ACTION_CANCEL:
                //LogInfo(0, "AMOTION_EVENT_ACTION_CANCEL: 0x%x", iPointerID);
                break;

            case AMOTION_EVENT_ACTION_OUTSIDE:
                //LogInfo(0, "AMOTION_EVENT_ACTION_OUTSIDE: 0x%x", iPointerID);
                break;
        }

        return 1;


    }
}

/**
 * This is the main entry point of a native application that is using
 * android_native_app_glue.  It runs in its own thread, with its own
 * event loop for receiving input events and doing other things.
 */
void android_main(struct android_app* state) {
    struct engine engine;
    memset(&engine, 0, sizeof(engine));

    // Make sure glue isn't stripped.
    app_dummy();

    engine.sample = new VkSample();

    assert(engine.sample);

    state->userData = &engine;
    state->onAppCmd = engine_handle_cmd;
    state->onInputEvent = engine_handle_input;
    engine.app = state;
    // Give the assetManager instance to the sample, as to allow it
    // to load assets such as shaders and images from our project.
    engine.sample->SetAssetManager(state->activity->assetManager);

    // loop waiting for stuff to do.
    while (1) {
        int ident;
        int events;
        struct android_poll_source* source;

        // If not animating, we will block forever waiting for events.
        // If animating, we loop until all events are read, then continue
        // to draw the next frame of animation.
        while ((ident=ALooper_pollAll(engine.animating ? 0 : -1, nullptr, &events,
                                      (void**)&source)) >= 0) {

            // Process this event.
            if (source != nullptr) {
                source->process(state, source);
            }

            // Check if we are exiting.
            if (state->destroyRequested != 0) {
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
