//                  Copyright (c) 2016 QUALCOMM Technologies Inc.
//                              All Rights Reserved.
#pragma once

// This definition enables the Android extensions
#define VK_USE_PLATFORM_ANDROID_KHR

// This definition allows prototypes of Vulkan API functions,
// rather than dynamically loading entrypoints to the API manually.
#define VK_PROTOTYPES

#include <assert.h>
#include <stdlib.h>
#include <vulkan/vulkan.h>
#include <android/log.h>
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "VkSample", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "VkSample", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "VkSample", __VA_ARGS__))


// Helper define for tracking error locations in code
#define VK_CHECK(X) if (!(X)) { LOGE("VK_CHECK Failure"); assert((X));}

struct DepthBuffer
{
    VkFormat format;
    VkImage image;
    VkDeviceMemory mem;
    VkImageView view;
};

struct SwapchainBuffer
{
    VkImage image;
    VkCommandBuffer cmdBuffer;
    VkImageView view;
};

struct Vertices
{
    VkBuffer buf;
    VkDeviceMemory mem;
    VkPipelineVertexInputStateCreateInfo vi;
    VkVertexInputBindingDescription      vi_bindings[1];
    VkVertexInputAttributeDescription    vi_attrs[2];
};

class VkSample
{
public:
    VkSample();
    ~VkSample();

    bool Initialize(ANativeWindow* window);

    void DrawFrame();
    
    bool IsInitialized() { return mInitialized; }
    
    bool TearDown();
    void ShutDown();
protected:

    bool CreateInstance();
    bool GetPhysicalDevices();
    void InitSurface();
    void InitDevice();
    void InitSwapchain();
    void InitCommandbuffers();
    void InitSync();
    void InitVertexBuffers();
    void InitLayouts();
    void InitRenderPass();
    void InitPipeline();
    void InitFrameBuffers();
    void InitSwapchainLayout();

    void BuildCmdBuffer();
    void SetNextBackBuffer();
    void PresentBackBuffer();
    
    VkShaderModule CreateShaderModule(const uint32_t* code, uint32_t size);
    bool GetMemoryTypeFromProperties( uint32_t typeBits, VkFlags requirements_mask, uint32_t *typeIndex);
    void SetImageLayout(VkImage image, VkCommandBuffer cmdBuffer, VkImageAspectFlags aspect, VkImageLayout oldLayout, VkImageLayout newLayout, VkPipelineStageFlags srcMask, VkPipelineStageFlags dstMask, uint32_t mipLevel=0, uint32_t mipLevelCount=1);

    ANativeWindow* mAndroidWindow;

    VkInstance mInstance;
    VkPhysicalDevice* mpPhysicalDevices;
    VkPhysicalDevice mPhysicalDevice;
    VkPhysicalDeviceProperties mPhysicalDeviceProperties;
    VkPhysicalDeviceMemoryProperties mPhysicalDeviceMemoryProperties;
    VkDevice mDevice;
    uint32_t mPhysicalDeviceCount;
    uint32_t mQueueFamilyIndex;
    VkQueue mQueue;
    VkSurfaceKHR mSurface;
    VkSurfaceFormatKHR mSurfaceFormat;

    VkSwapchainKHR mSwapchain;
    SwapchainBuffer* mSwapchainBuffers;
    uint32_t mSwapchainCurrentIdx;
    uint32_t mHeight;
    uint32_t mWidth;
    uint32_t mSwapchainImageCount;
    VkSemaphore mBackBufferSemaphore;
    VkSemaphore mRenderCompleteSemaphore;
    VkFence mFence;
    VkFramebuffer* mFrameBuffers;

    VkCommandPool mCommandPool;

    DepthBuffer* mDepthBuffers;

    Vertices mVertices;
    
    VkDescriptorSetLayout mDescriptorLayout;
    VkPipelineLayout mPipelineLayout;
    VkRenderPass mRenderPass;
    VkPipeline mPipeline;
    
    uint64_t GetTime();
    uint64_t mFrameTimeBegin;
    uint64_t mFrameTimeEnd;
    uint64_t mFrameIdx;
    
    bool mInitialized;
    bool mInitBegun;

    // debug
    bool CheckInstanceLayerValidation();
    uint32_t mEnabledInstanceLayerCount;
    static const char*  mInstanceLayers[];
    void CreateValidationCallbacks();
    PFN_vkCreateDebugReportCallbackEXT  mCreateDebugReportCallbackEXT;
    PFN_vkDestroyDebugReportCallbackEXT mDestroyDebugReportCallbackEXT;
    PFN_vkDebugReportMessageEXT         mDebugReportMessageCallback;
    VkDebugReportCallbackEXT            mDebugReportCallback;
};