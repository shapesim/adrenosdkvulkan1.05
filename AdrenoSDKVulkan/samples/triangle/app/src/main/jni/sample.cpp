//                  Copyright (c) 2016 QUALCOMM Technologies Inc.
//                              All Rights Reserved.

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#include "sample.h"
#include "shader.h"

// The vertex buffer bind id, used as a constant in various places in the sample
#define VERTEX_BUFFER_BIND_ID 0

// Sample Name
#define SAMPLE_NAME "Vulkan Sample: Triangle"

VkSample::VkSample()
{
    mInitialized = false;
    mInitBegun = false;
    mDebugReportCallback = NULL;
}
///////////////////////////////////////////////////////////////////////////////

VkSample::~VkSample()
{
    if (mInitBegun)
    {
        TearDown();
    }
}

///////////////////////////////////////////////////////////////////////////////

// VkSample::Initialize
//
// Initializes the Vulkan subsystem to a default sample state.
//
bool VkSample::Initialize(ANativeWindow* window)
{
    VkResult ret = VK_SUCCESS;

    if (mInitBegun)
    {
        TearDown();
    }

    //mInitBegun acts as a signal that a partial teardown may be
    //needed, regardless of mInitialized state.
    mInitBegun = true;

    //The android window to render to is passed, we must use this
    //later in the initialization sequence.
    mAndroidWindow = window;

    CreateInstance();
    GetPhysicalDevices();

    InitDevice();
    InitSwapchain();
    InitCommandbuffers();
    InitVertexBuffers();
    InitLayouts();
    InitRenderPass();
    InitPipeline();
    InitFrameBuffers();
    InitSync();
    InitSwapchainLayout();

    // Initialize our command buffers
    BuildCmdBuffer();

    // We acquire the next swap chain image, in preparation for the render loop
    SetNextBackBuffer();

    mFrameIdx = 0;
    mFrameTimeBegin = GetTime();

    mInitialized = true;
    return true;
}

//debug
const char*  VkSample::mInstanceLayers[] =
        {
                "VK_LAYER_GOOGLE_threading",
                "VK_LAYER_LUNARG_parameter_validation",
                "VK_LAYER_LUNARG_object_tracker",
                "VK_LAYER_LUNARG_core_validation",
                "VK_LAYER_LUNARG_device_limits",
                "VK_LAYER_LUNARG_image",
                "VK_LAYER_LUNARG_swapchain",
                "VK_LAYER_GOOGLE_unique_objects",
        };

bool VkSample::CheckInstanceLayerValidation()
{
    // Determine the number of instance layers that Vulkan reports
    uint32_t numInstanceLayers = 0;
    vkEnumerateInstanceLayerProperties(&numInstanceLayers, nullptr);

    // Enumerate instance layers with valid pointer in last parameter
    VkLayerProperties* layerProperties = (VkLayerProperties*)malloc(numInstanceLayers * sizeof(VkLayerProperties));
    vkEnumerateInstanceLayerProperties(&numInstanceLayers, layerProperties);

    // Make sure the desired instance validation layers are available
    // NOTE:  These are not listed in an arbitrary order.  Threading must be
    //        first, and unique_objects must be last.  This is the order they
    //        will be inserted by the loader.
    mEnabledInstanceLayerCount =  sizeof(mInstanceLayers) / sizeof(mInstanceLayers[0]);
    for (uint32_t i = 0; i < mEnabledInstanceLayerCount; i++)
    {
        bool found = false;
        for (uint32_t j = 0; j < numInstanceLayers; j++)
        {
            if (strcmp(mInstanceLayers[i], layerProperties[j].layerName) == 0)
            {
                found = true;
            }
        }
        if (!found)
        {
            LOGE("Instance Layer not found: %s", mInstanceLayers[i]);
            return false;
        }
    }

    return true;
}


//////////////////////////////////////////////////////////////////////////////
// Validation call back.  We Filter the message and log.
static VKAPI_ATTR VkBool32 VKAPI_CALL DebugReportCallback(
        VkDebugReportFlagsEXT msgFlags,
        VkDebugReportObjectTypeEXT objType,
        uint64_t srcObject, size_t location,
        int32_t msgCode, const char * pLayerPrefix,
        const char * pMsg, void * pUserData )
{
    // allocate a string to build up the validation message we'll report
    char *message = (char *)malloc(strlen(pMsg) + 100);
    assert(message);

    if (msgFlags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT)
    {
        sprintf(message, "INFORMATION: [%s] Code %d : %s\n", pLayerPrefix, msgCode, pMsg);
    }
    else if (msgFlags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
    {
        sprintf(message, "WARNING: [%s] Code %d : %s\n", pLayerPrefix, msgCode, pMsg);
    }
    else if (msgFlags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
    {
        sprintf(message, "PERFORMANCE WARNING: [%s] Code %d : %s\n", pLayerPrefix, msgCode, pMsg);
    }
    else if (msgFlags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
    {
        if (msgCode == 8)
        {
            // Ignore for now...vkCreateSwapChainKHR() called with pCreateInfo->imageExtent = (..,..), which is not equal to the currentExtent = (..,..) return by vkGetPhysicalDeviceSurfaceCapabilitiesKHR()
            return false;
        }
        else if (msgCode == 53)
        {
            // Ignore for now...Command Buffer 0x..... is already in use and not marked for simultaneous use
            return false;
        }
        else if (msgCode == 7)
        {
            // Ignore for now...Cannot clear attachment 1 with invalid first layout VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
            // http://stackoverflow.com/questions/38445001/layout-transition-between-multiple-subpasses-in-vulkan/38446072
            return false;
        }
        sprintf(message, "ERROR: [%s] Code %d : %s\n", pLayerPrefix, msgCode, pMsg);
    }
    else if (msgFlags & VK_DEBUG_REPORT_DEBUG_BIT_EXT)
    {
        sprintf(message, "DEBUG: [%s] Code %d : %s\n", pLayerPrefix, msgCode, pMsg);
    }
    else
    {
        // Not sure what else???
        return false;
    }
    // Log it!
    LOGI(message);

    free(message);

    // false indicates that layer should not bail-out of an API call that had validation failures.
    // This may mean that the app dies inside the driver due to invalid parameter(s).
    // That's what would happen without validation layers, so we'll keep that behavior here.
    return false;
}

///////////////////////////////////////////////////////////////////////////////
// Using the ReportCallback Extensions, we'll create validation callbacks.
void VkSample::CreateValidationCallbacks()
{
    mCreateDebugReportCallbackEXT   = (PFN_vkCreateDebugReportCallbackEXT)  vkGetInstanceProcAddr( mInstance, "vkCreateDebugReportCallbackEXT");
    mDestroyDebugReportCallbackEXT  = (PFN_vkDestroyDebugReportCallbackEXT) vkGetInstanceProcAddr( mInstance, "vkDestroyDebugReportCallbackEXT");
    mDebugReportMessageCallback     = (PFN_vkDebugReportMessageEXT)         vkGetInstanceProcAddr( mInstance, "vkDebugReportMessageEXT");

    VK_CHECK(mCreateDebugReportCallbackEXT);
    VK_CHECK(mDestroyDebugReportCallbackEXT);
    VK_CHECK(mDebugReportMessageCallback);

    // Create the debug report callback..
    VkDebugReportCallbackCreateInfoEXT dbgCreateInfo;
    dbgCreateInfo.sType         = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
    dbgCreateInfo.pNext         = NULL;
    dbgCreateInfo.pfnCallback   = DebugReportCallback;
    dbgCreateInfo.pUserData     = NULL;
    dbgCreateInfo.flags         =   VK_DEBUG_REPORT_ERROR_BIT_EXT               |
                                    VK_DEBUG_REPORT_WARNING_BIT_EXT             |
                                    VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT |
                                    // Uncomment this flag for verbose information logging
                                    //VK_DEBUG_REPORT_INFORMATION_BIT_EXT         |
                                    VK_DEBUG_REPORT_DEBUG_BIT_EXT;

    VkResult ret = mCreateDebugReportCallbackEXT(mInstance, &dbgCreateInfo, NULL, &mDebugReportCallback);
    VK_CHECK(!ret);
}

///////////////////////////////////////////////////////////////////////////////

bool VkSample::CreateInstance()
{
    VkResult ret = VK_SUCCESS;
    bool bUseValidation = false;

    // Discover the number of extensions listed in the instance properties in order to allocate
    // a buffer large enough to hold them.
    uint32_t instanceExtensionCount = 0;
    ret = vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, nullptr);
    VK_CHECK(!ret);

    VkBool32 surfaceExtFound            = 0;
    VkBool32 platformSurfaceExtFound    = 0;
    VkBool32 debugReportExtFound        = 0;

    VkExtensionProperties* instanceExtensions = nullptr;
    instanceExtensions = new VkExtensionProperties[instanceExtensionCount];

    // Now request instanceExtensionCount VkExtensionProperties elements be read into out buffer
    ret = vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, instanceExtensions);
    VK_CHECK(!ret);

    // We require two extensions, VK_KHR_surface and VK_KHR_android_surface. If they are found,
    // add them to the extensionNames list that we'll use to initialize our instance with later.
    uint32_t enabledExtensionCount = 0;
    const char* extensionNames[16];
    for (uint32_t i = 0; i < instanceExtensionCount; i++)
    {
        if (!strcmp(VK_KHR_SURFACE_EXTENSION_NAME, instanceExtensions[i].extensionName))
        {
            surfaceExtFound = 1;
            extensionNames[enabledExtensionCount++] = VK_KHR_SURFACE_EXTENSION_NAME;
        }

        if (!strcmp(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME, instanceExtensions[i].extensionName))
        {
            platformSurfaceExtFound = 1;
            extensionNames[enabledExtensionCount++] = VK_KHR_ANDROID_SURFACE_EXTENSION_NAME;
        }

        if (bUseValidation && !strcmp(VK_EXT_DEBUG_REPORT_EXTENSION_NAME, instanceExtensions[i].extensionName))
        {
            debugReportExtFound = 1;
            extensionNames[enabledExtensionCount++] = VK_EXT_DEBUG_REPORT_EXTENSION_NAME;
        }

        VK_CHECK(enabledExtensionCount < 16);
    }
    if (!surfaceExtFound)
    {
        LOGE("vkEnumerateInstanceExtensionProperties failed to find the " VK_KHR_SURFACE_EXTENSION_NAME" extension.");
        return false;
    }
    if (!platformSurfaceExtFound)
    {
        LOGE("vkEnumerateInstanceExtensionProperties failed to find the " VK_KHR_ANDROID_SURFACE_EXTENSION_NAME" extension.");
        return false;
    }

    if (bUseValidation && !debugReportExtFound)
    {
        LOGE("vkEnumerateInstanceExtensionProperties failed to find the " VK_EXT_DEBUG_REPORT_EXTENSION_NAME" extension.");
        return false;
    }

    // We specify the Vulkan version our application was built with,
    // as well as names and versions for our application and engine,
    // if applicable. This allows the driver to gain insight to what
    // is utilizing the vulkan driver, and serve appropriate versions.
    VkApplicationInfo applicationInfo = {};
    applicationInfo.sType                       = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pNext                       = nullptr;
    applicationInfo.pApplicationName            = SAMPLE_NAME;
    applicationInfo.applicationVersion          = 0;
    applicationInfo.pEngineName                 = "VkSample";
    applicationInfo.engineVersion               = 1;
    applicationInfo.apiVersion                  = VK_API_VERSION_1_0;

    if (bUseValidation && !CheckInstanceLayerValidation())
    {
        return false;
    }

    // Creation information for the instance points to details about
    // the application, and also the list of extensions to enable.
    VkInstanceCreateInfo instanceCreateInfo = {};
    instanceCreateInfo.sType                    = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pNext                    = nullptr;
    instanceCreateInfo.pApplicationInfo         = &applicationInfo;
    instanceCreateInfo.enabledLayerCount        = mEnabledInstanceLayerCount; //debug 0
    instanceCreateInfo.ppEnabledLayerNames      = mEnabledInstanceLayerCount==0 ? nullptr : mInstanceLayers; // debug nullptr;
    instanceCreateInfo.enabledExtensionCount    = enabledExtensionCount;
    instanceCreateInfo.ppEnabledExtensionNames  = extensionNames;

    // The main Vulkan instance is created with the creation infos above.
    // We do not specify a custom memory allocator for instance creation.
    ret = vkCreateInstance(&instanceCreateInfo, nullptr, &mInstance);

    // we can delete the list of extensions after calling vkCreateInstance
    delete[] instanceExtensions;

    // Vulkan API return values can expose further information on a failure.
    // For instance, INCOMPATIBLE_DRIVER may be returned if the API level
    // an application is built with, exposed through VkApplicationInfo, is 
    // newer than the driver present on a device.
    if (ret == VK_ERROR_INCOMPATIBLE_DRIVER)
    {
        LOGE("Cannot find a compatible Vulkan installable client driver: vkCreateInstance Failure");
        return false;
    }
    else if (ret == VK_ERROR_EXTENSION_NOT_PRESENT)
    {
        LOGE("Cannot find a specified extension library: vkCreateInstance Failure");
        return false;
    }
    else
    {
        VK_CHECK(!ret);
    }

    if (bUseValidation)
    {
        CreateValidationCallbacks();
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////

bool VkSample::GetPhysicalDevices()
{
    VkResult ret = VK_SUCCESS;

    // Query number of physical devices available
    ret = vkEnumeratePhysicalDevices(mInstance, &mPhysicalDeviceCount, nullptr);
    VK_CHECK(!ret);

    if (mPhysicalDeviceCount == 0)
    {
        LOGE("No physical devices detected.");
        return false;
    }

    // Allocate space the the correct number of devices, before requesting their data
    mpPhysicalDevices = new VkPhysicalDevice[mPhysicalDeviceCount];
    ret = vkEnumeratePhysicalDevices(mInstance, &mPhysicalDeviceCount, mpPhysicalDevices);
    VK_CHECK(!ret);

    
    // For purposes of this sample, we simply use the first device.
    mPhysicalDevice = mpPhysicalDevices[0];

    // By querying the device properties, we learn the device name, amongst
    // other details.
    vkGetPhysicalDeviceProperties(mPhysicalDevice, &mPhysicalDeviceProperties);

    LOGI("Vulkan Device: %s", mPhysicalDeviceProperties.deviceName);

    // Get Memory information and properties - this is required later, when we begin
    // allocating buffers to store data.
    vkGetPhysicalDeviceMemoryProperties(mPhysicalDevice, &mPhysicalDeviceMemoryProperties);

    return true;
}

///////////////////////////////////////////////////////////////////////////////

void VkSample::InitDevice()
{
    VkResult ret = VK_SUCCESS;
    // Akin to when creating the instance, we can query extensions supported by the physical device
    // that we have selected to use.
    uint32_t deviceExtensionCount = 0;
    VkExtensionProperties *device_extensions = nullptr;
    ret = vkEnumerateDeviceExtensionProperties(mPhysicalDevice, nullptr, &deviceExtensionCount, nullptr);
    VK_CHECK(!ret);

    VkBool32 swapchainExtFound = 0;
    VkExtensionProperties* deviceExtensions = new VkExtensionProperties[deviceExtensionCount];
    ret = vkEnumerateDeviceExtensionProperties(mPhysicalDevice, nullptr, &deviceExtensionCount, deviceExtensions);
    VK_CHECK(!ret);

    // For our example, we require the swapchain extension, which is used to present backbuffers efficiently
    // to the users screen.
    uint32_t enabledExtensionCount = 0;
    const char* extensionNames[16] = {0};
    for (uint32_t i = 0; i < deviceExtensionCount; i++) {
        if (!strcmp(VK_KHR_SWAPCHAIN_EXTENSION_NAME, deviceExtensions[i].extensionName))
        {
            swapchainExtFound = 1;
            extensionNames[enabledExtensionCount++] = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
        }
        VK_CHECK(enabledExtensionCount < 16);
    }
    if (!swapchainExtFound)
    {
        LOGE("vkEnumerateDeviceExtensionProperties failed to find the " VK_KHR_SWAPCHAIN_EXTENSION_NAME " extension: vkCreateInstance Failure");

        // Always attempt to enable the swapchain
        extensionNames[enabledExtensionCount++] = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
    }

    InitSurface();

    // Before we create our main Vulkan device, we must ensure our physical device
    // has queue families which can perform the actions we require. For this, we request
    // the number of queue families, and their properties.
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &queueFamilyCount, nullptr);

    VkQueueFamilyProperties* queueProperties = new VkQueueFamilyProperties[queueFamilyCount];
    vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &queueFamilyCount, queueProperties);
    VK_CHECK(queueFamilyCount >= 1);

    // We query each queue family in turn for the ability to support the android surface
    // that was created earlier. We need the device to be able to present its images to
    // this surface, so it is important to test for this.
    VkBool32* supportsPresent = new VkBool32[queueFamilyCount];
    for (uint32_t i = 0; i < queueFamilyCount; i++) {
        vkGetPhysicalDeviceSurfaceSupportKHR(mPhysicalDevice, i, mSurface, &supportsPresent[i]);
    }

    // Search for a graphics queue, and ensure it also supports our surface. We want a
    // queue which can be used for both, as to simplify operations.
    uint32_t queueIndex = UINT32_MAX;
    for (uint32_t i = 0; i < queueFamilyCount; i++)
    {
        if ((queueProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
        {
            if (supportsPresent[i] == VK_TRUE)
            {
                queueIndex = i;
                break;
            }
        }
    }

    delete [] supportsPresent;
    delete [] queueProperties;

    if (queueIndex == UINT32_MAX)
    {
        VK_CHECK("Could not obtain a queue family for both graphics and presentation." && 0);
    }

    // We have identified a queue family which both supports our android surface,
    // and can be used for graphics operations.
    mQueueFamilyIndex = queueIndex;

    // As we create the device, we state we will be creating a queue of the
    // family type required. 1.0 is the highest priority and we use that.
    float queuePriorities[1] = { 1.0 };
    VkDeviceQueueCreateInfo deviceQueueCreateInfo = {};
    deviceQueueCreateInfo.sType                 = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    deviceQueueCreateInfo.pNext                 = nullptr;
    deviceQueueCreateInfo.queueFamilyIndex      = mQueueFamilyIndex;
    deviceQueueCreateInfo.queueCount            = 1;
    deviceQueueCreateInfo.pQueuePriorities      = queuePriorities;

    // Now we pass the queue create info, as well as our requested extensions,
    // into our DeviceCreateInfo structure.
    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType                      = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pNext                      = nullptr;
    deviceCreateInfo.queueCreateInfoCount       = 1;
    deviceCreateInfo.pQueueCreateInfos          = &deviceQueueCreateInfo;
    deviceCreateInfo.enabledLayerCount          = 0;
    deviceCreateInfo.ppEnabledLayerNames        = nullptr;
    deviceCreateInfo.enabledExtensionCount      = enabledExtensionCount;
    deviceCreateInfo.ppEnabledExtensionNames    = extensionNames;

    // Create the device.
    ret = vkCreateDevice(mPhysicalDevice, &deviceCreateInfo, nullptr, &mDevice);
    VK_CHECK(!ret);

    // Obtain the device queue that we requested.
    vkGetDeviceQueue(mDevice, mQueueFamilyIndex, 0, &mQueue);
}

///////////////////////////////////////////////////////////////////////////////

void VkSample::InitSurface()
{
    VkResult ret = VK_SUCCESS;
    // At this point, we create the android surface. This is because we want to
    // ensure our device is capable of working with the created surface object.
    VkAndroidSurfaceCreateInfoKHR surfaceCreateInfo = {};
    surfaceCreateInfo.sType     = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
    surfaceCreateInfo.pNext     = nullptr;
    surfaceCreateInfo.flags     = 0;
    surfaceCreateInfo.window    = mAndroidWindow;

    ret = vkCreateAndroidSurfaceKHR(mInstance, &surfaceCreateInfo, nullptr, &mSurface);
    VK_CHECK(!ret);
}

///////////////////////////////////////////////////////////////////////////////

void VkSample::InitSwapchain()
{
    VkResult ret = VK_SUCCESS;
    // By querying the supported formats of our surface, we can ensure that
    // we use one that the device can work with.
    uint32_t formatCount;
    ret = vkGetPhysicalDeviceSurfaceFormatsKHR(mPhysicalDevice, mSurface, &formatCount, nullptr);
    VK_CHECK(!ret);

    VkSurfaceFormatKHR* surfFormats = new VkSurfaceFormatKHR[formatCount];
    ret = vkGetPhysicalDeviceSurfaceFormatsKHR(mPhysicalDevice, mSurface, &formatCount, surfFormats);
    VK_CHECK(!ret);

    // If the format list includes just one entry of VK_FORMAT_UNDEFINED,
    // the surface has no preferred format.  Otherwise, at least one
    // supported format will be returned. For the purposes of this sample, 
    // we use the first format returned.
    if (formatCount == 1 && surfFormats[0].format == VK_FORMAT_UNDEFINED)
    {
        mSurfaceFormat.format = VK_FORMAT_B8G8R8A8_UNORM;
        mSurfaceFormat.colorSpace = surfFormats[0].colorSpace;
    }
    else
    {
        mSurfaceFormat = surfFormats[0];
    }

    delete [] surfFormats;

    // Now we obtain the surface capabilities, which contains details such as width and height.
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    ret = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mPhysicalDevice, mSurface, &surfaceCapabilities);
    VK_CHECK(!ret);

    mWidth  = surfaceCapabilities.currentExtent.width;
    mHeight = surfaceCapabilities.currentExtent.height;

    // Now that we have selected formats and obtained ideal surface dimensions,
    // we create the swapchain. We use FIFO mode, which is always present. This
    // mode has a queue of images internally, that will be presented to the screen.
    // The swapchain will be created and expose the number of images created
    // in the queue, which will be at least the number specified in minImageCount.
    VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
    swapchainCreateInfo.sType                 = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfo.surface               = mSurface;
    swapchainCreateInfo.minImageCount         = surfaceCapabilities.minImageCount;
    swapchainCreateInfo.imageFormat           = mSurfaceFormat.format;
    swapchainCreateInfo.imageColorSpace       = mSurfaceFormat.colorSpace;
    swapchainCreateInfo.imageExtent.width     = mWidth;
    swapchainCreateInfo.imageExtent.height    = mHeight;
    swapchainCreateInfo.imageUsage            = surfaceCapabilities.supportedUsageFlags;
    swapchainCreateInfo.preTransform          = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    swapchainCreateInfo.imageArrayLayers      = 1;
    swapchainCreateInfo.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
    swapchainCreateInfo.compositeAlpha        = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;
    swapchainCreateInfo.presentMode           = VK_PRESENT_MODE_FIFO_KHR;
    swapchainCreateInfo.clipped               = VK_TRUE;

    ret = vkCreateSwapchainKHR(mDevice, &swapchainCreateInfo, nullptr, &mSwapchain);
    VK_CHECK(!ret);

    // Query the number of swapchain images. This is the number of images in the internal
    // queue.
    ret = vkGetSwapchainImagesKHR(mDevice, mSwapchain, &mSwapchainImageCount, nullptr);
    VK_CHECK(!ret);

    LOGI("Swapchain Image Count: %d\n", mSwapchainImageCount);

    // Now we can retrieve these images, as to use them in rendering as our framebuffers.
    VkImage* pSwapchainImages = new VkImage[mSwapchainImageCount];
    ret = vkGetSwapchainImagesKHR(mDevice, mSwapchain, &mSwapchainImageCount, pSwapchainImages);
    VK_CHECK(!ret);

    // We prepare our own representation of the swapchain buffers, for keeping track
    // of resources during rendering.
    mSwapchainBuffers = new SwapchainBuffer[mSwapchainImageCount];
    VK_CHECK(mSwapchainBuffers);

    // From the images obtained from the swapchain, we create image views.
    // This gives us context into the image.
    VkImageViewCreateInfo imageViewCreateInfo = {};
    imageViewCreateInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewCreateInfo.pNext                           = nullptr;
    imageViewCreateInfo.format                          = mSurfaceFormat.format;
    imageViewCreateInfo.components.r                    = VK_COMPONENT_SWIZZLE_R;
    imageViewCreateInfo.components.g                    = VK_COMPONENT_SWIZZLE_G;
    imageViewCreateInfo.components.b                    = VK_COMPONENT_SWIZZLE_B;
    imageViewCreateInfo.components.a                    = VK_COMPONENT_SWIZZLE_A;
    imageViewCreateInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    imageViewCreateInfo.subresourceRange.baseMipLevel   = 0;
    imageViewCreateInfo.subresourceRange.levelCount     = 1;
    imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    imageViewCreateInfo.subresourceRange.layerCount     = 1;
    imageViewCreateInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
    imageViewCreateInfo.flags                           = 0;

    for (uint32_t i = 0; i < mSwapchainImageCount; i++)
    {
        // We create an Imageview for each swapchain image, and track
        // the view and image in our swapchainBuffers object.
        mSwapchainBuffers[i].image = pSwapchainImages[i];
        imageViewCreateInfo.image  = pSwapchainImages[i];

        VkResult err = vkCreateImageView(mDevice, &imageViewCreateInfo, nullptr, &mSwapchainBuffers[i].view);
        VK_CHECK(!err);
    }

    // At this point, we have the references now in our swapchainBuffer object
    delete [] pSwapchainImages;

    // Now we create depth buffers for our swapchain images, which form part of our framebuffers later.
    mDepthBuffers = new DepthBuffer[mSwapchainImageCount];
    for (int i = 0; i < mSwapchainImageCount; i++)
    {
        const VkFormat depthFormat = VK_FORMAT_D16_UNORM;

        VkImageCreateInfo imageCreateInfo = {};
        imageCreateInfo.sType                               = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageCreateInfo.pNext                               = nullptr;
        imageCreateInfo.imageType                           = VK_IMAGE_TYPE_2D;
        imageCreateInfo.format                              = depthFormat;
        imageCreateInfo.extent                              = {mWidth, mHeight, 1};
        imageCreateInfo .mipLevels                          = 1;
        imageCreateInfo .arrayLayers                        = 1;
        imageCreateInfo .samples                            = VK_SAMPLE_COUNT_1_BIT;
        imageCreateInfo.tiling                              = VK_IMAGE_TILING_OPTIMAL;
        imageCreateInfo.usage                               = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        imageCreateInfo .flags                              = 0;

        VkImageViewCreateInfo imageViewCreateInfo = {};
        imageViewCreateInfo .sType                          = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo .pNext                          = nullptr;
        imageViewCreateInfo .image                          = VK_NULL_HANDLE;
        imageViewCreateInfo.format                          = depthFormat;
        imageViewCreateInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_DEPTH_BIT;
        imageViewCreateInfo.subresourceRange.baseMipLevel   = 0;
        imageViewCreateInfo.subresourceRange.levelCount     = 1;
        imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        imageViewCreateInfo.subresourceRange.layerCount     = 1;
        imageViewCreateInfo.flags                           = 0;
        imageViewCreateInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;

        VkMemoryRequirements mem_reqs;
        VkResult  err;
        bool  pass;

        mDepthBuffers[i].format = depthFormat;

        // Create the image with details as imageCreateInfo
        err = vkCreateImage(mDevice, &imageCreateInfo, nullptr, &mDepthBuffers[i].image);
        VK_CHECK(!err);

        // discover what memory requirements are for this image.
        vkGetImageMemoryRequirements(mDevice, mDepthBuffers[i].image, &mem_reqs);

        // Allocate memory according to requirements
        VkMemoryAllocateInfo memoryAllocateInfo = {};
        memoryAllocateInfo.sType                            = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memoryAllocateInfo.pNext                            = nullptr;
        memoryAllocateInfo.allocationSize                   = 0;
        memoryAllocateInfo.memoryTypeIndex                  = 0;
        memoryAllocateInfo.allocationSize                   = mem_reqs.size;
        pass = GetMemoryTypeFromProperties(mem_reqs.memoryTypeBits, 0, &memoryAllocateInfo.memoryTypeIndex);
        VK_CHECK(pass);

        err = vkAllocateMemory(mDevice, &memoryAllocateInfo, nullptr, &mDepthBuffers[i].mem);
        VK_CHECK(!err);

        // Bind memory to the image
        err = vkBindImageMemory(mDevice, mDepthBuffers[i].image, mDepthBuffers[i].mem, 0);
        VK_CHECK(!err);

        // Create the view for this image
        imageViewCreateInfo.image = mDepthBuffers[i].image;
        err = vkCreateImageView(mDevice, &imageViewCreateInfo, nullptr, &mDepthBuffers[i].view);
        VK_CHECK(!err);
    }
}

///////////////////////////////////////////////////////////////////////////////

void VkSample::InitSwapchainLayout()
{
    VkResult err;

    // Set the swapchain layout to present
    for (uint32_t i = 0; i < mSwapchainImageCount; i++)
    {
        VkCommandBuffer &cmdBuffer = mSwapchainBuffers[i].cmdBuffer;

        // vkBeginCommandBuffer should reset the command buffer, but Reset can be called
        // to make it more explicit.
        err = vkResetCommandBuffer(cmdBuffer, 0);
        VK_CHECK(!err);

        VkCommandBufferInheritanceInfo cmd_buf_hinfo = {};
        cmd_buf_hinfo.sType                                 = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
        cmd_buf_hinfo.pNext                                 = nullptr;
        cmd_buf_hinfo.renderPass                            = VK_NULL_HANDLE;
        cmd_buf_hinfo.subpass                               = 0;
        cmd_buf_hinfo.framebuffer                           = VK_NULL_HANDLE;
        cmd_buf_hinfo.occlusionQueryEnable                  = VK_FALSE;
        cmd_buf_hinfo.queryFlags                            = 0;
        cmd_buf_hinfo.pipelineStatistics                    = 0;

        VkCommandBufferBeginInfo cmd_buf_info = {};
        cmd_buf_info.sType                                  = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        cmd_buf_info.pNext                                  = nullptr;
        cmd_buf_info.flags                                  = 0;
        cmd_buf_info.pInheritanceInfo                       = &cmd_buf_hinfo;

        // By calling vkBeginCommandBuffer, cmdBuffer is put into the recording state.
        err = vkBeginCommandBuffer(cmdBuffer, &cmd_buf_info);
        VK_CHECK(!err);

        SetImageLayout(mSwapchainBuffers[i].image, cmdBuffer, VK_IMAGE_ASPECT_COLOR_BIT,                               VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT);
        SetImageLayout(mDepthBuffers[i].image,     cmdBuffer, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT);

        err = vkEndCommandBuffer(cmdBuffer);
        VK_CHECK(!err);

        // Submit the queue
        const VkPipelineStageFlags WaitDstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        VkSubmitInfo submitInfo = {};
        submitInfo.sType                                    = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.pNext                                    = nullptr;
        submitInfo.waitSemaphoreCount                       = 0;
        submitInfo.pWaitSemaphores                          = nullptr;
        submitInfo.pWaitDstStageMask                        = &WaitDstStageMask;
        submitInfo.commandBufferCount                       = 1;
        submitInfo.pCommandBuffers                          = &mSwapchainBuffers[i].cmdBuffer;
        submitInfo.signalSemaphoreCount                     = 0;
        submitInfo.pSignalSemaphores                        = nullptr;
        err = vkQueueSubmit(mQueue, 1, &submitInfo, mFence);
        VK_CHECK(!err);
        err = vkWaitForFences(mDevice, 1, &mFence, true, 0xFFFFFFFF);
        VK_CHECK(!err);
        err= vkResetFences(mDevice, 1, &mFence);
        VK_CHECK(!err);
    }
}

///////////////////////////////////////////////////////////////////////////////

void VkSample::InitCommandbuffers()
{
    VkResult ret = VK_SUCCESS;
    // Command buffers are allocated from a pool; we define that pool here and create it.
    VkCommandPoolCreateInfo commandPoolCreateInfo = {};
    commandPoolCreateInfo.sType                     = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.pNext                     = nullptr;
    commandPoolCreateInfo.queueFamilyIndex          = mQueueFamilyIndex;
    commandPoolCreateInfo.flags                     = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    ret = vkCreateCommandPool(mDevice, &commandPoolCreateInfo, nullptr, &mCommandPool);
    VK_CHECK(!ret);

    VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
    commandBufferAllocateInfo.sType                 = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.pNext                 = nullptr;
    commandBufferAllocateInfo.commandPool           = mCommandPool;
    commandBufferAllocateInfo.level                 = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandBufferCount    = 1;

    // Create render command buffers, one per swapchain image
    for (int i=0; i < mSwapchainImageCount; i++)
    {
        ret = vkAllocateCommandBuffers(mDevice, &commandBufferAllocateInfo, &mSwapchainBuffers[i].cmdBuffer);
        VK_CHECK(!ret);
    }
}

///////////////////////////////////////////////////////////////////////////////

void VkSample::InitVertexBuffers()
{
    // Our vertex buffer data is a simple triangle, with associated vertex colors.
    const float vb[3][7] = {
            //      position                   color
            { -0.9f, -0.9f,  0.9f,     1.0f, 0.0f, 0.0f, 1.0f },
            {  0.9f, -0.9f,  0.9f,     0.0f, 1.0f, 0.0f, 1.0f },
            {  0.0f,  0.9f,  0.9f,     0.0f, 0.0f, 1.0f, 1.0f },
    };

    VkResult   err;
    bool   pass;

    // Our mVertices member contains the types required for storing
    // and defining our vertex buffer within the graphics pipeline.
    memset(&mVertices, 0, sizeof(mVertices));

    // Create our buffer object.
    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.sType  = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.pNext  = nullptr;
    bufferCreateInfo.size   = sizeof(vb);
    bufferCreateInfo.usage  = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferCreateInfo.flags  = 0;
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
    mVertices.vi.pVertexBindingDescriptions         = mVertices.vi_bindings;
    mVertices.vi.vertexAttributeDescriptionCount    = 2;
    mVertices.vi.pVertexAttributeDescriptions       = mVertices.vi_attrs;

    // We bind the buffer as a whole, using the correct buffer ID.
    // This defines the stride for each element of the vertex array.
    mVertices.vi_bindings[0].binding                = VERTEX_BUFFER_BIND_ID;
    mVertices.vi_bindings[0].stride                 = sizeof(vb[0]);
    mVertices.vi_bindings[0].inputRate              = VK_VERTEX_INPUT_RATE_VERTEX;

    // Within each element, we define the attributes. At location 0,
    // the vertex positions, in float3 format, with offset 0 as they are
    // first in the array structure.
    mVertices.vi_attrs[0].binding                   = VERTEX_BUFFER_BIND_ID;
    mVertices.vi_attrs[0].location                  = 0;
    mVertices.vi_attrs[0].format                    = VK_FORMAT_R32G32B32_SFLOAT; //float3
    mVertices.vi_attrs[0].offset                    = 0;

    // The second location is the vertex colors, in RGBA float4 format.
    // These appear in each element in memory after the float3 vertex
    // positions, so the offset is set accordingly.
    mVertices.vi_attrs[1].binding                   = VERTEX_BUFFER_BIND_ID;
    mVertices.vi_attrs[1].location                  = 1;
    mVertices.vi_attrs[1].format                    = VK_FORMAT_R32G32B32A32_SFLOAT; //float4
    mVertices.vi_attrs[1].offset                    = sizeof(float) * 3;
}

///////////////////////////////////////////////////////////////////////////////

void VkSample::InitLayouts()
{
    VkResult ret = VK_SUCCESS;
    // This sample has no bindings, so the layout is empty.
    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
    descriptorSetLayoutCreateInfo.sType             = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutCreateInfo.pNext             = nullptr;
    descriptorSetLayoutCreateInfo.bindingCount      = 0;
    descriptorSetLayoutCreateInfo.pBindings         = nullptr;

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

    attachmentDescriptions[1].flags = 0;
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
    colorReference.attachment = 0;
    colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthReference = {};
    depthReference.attachment = 1;
    depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

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

void VkSample::InitPipeline()
{
    VkResult   err;

    // The pipeline contains all major state for rendering.

    // Our vertex input is a single vertex buffer, and its layout is defined
    // in our mVertices object already. Use this when creating the pipeline.
    VkPipelineVertexInputStateCreateInfo   vi = {};
    vi = mVertices.vi;

    // Our vertex buffer describes a triangle list.
    VkPipelineInputAssemblyStateCreateInfo ia = {}; 
    ia.sType    = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    ia.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    // State for rasterization, such as polygon fill mode is defined.
    VkPipelineRasterizationStateCreateInfo rs = {}; 
    rs.sType                    = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rs.polygonMode              = VK_POLYGON_MODE_FILL;
    rs.cullMode                 = VK_CULL_MODE_BACK_BIT;
    rs.frontFace                = VK_FRONT_FACE_CLOCKWISE;
    rs.depthClampEnable         = VK_FALSE;
    rs.rasterizerDiscardEnable  = VK_FALSE;
    rs.depthBiasEnable          = VK_FALSE;
    rs.lineWidth                = 1.0f;

    // For this example we do not do blending, so it is disabled.
    VkPipelineColorBlendAttachmentState att_state[1] = {}; 
    att_state[0].colorWriteMask = 0xf;
    att_state[0].blendEnable    = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo    cb = {}; 
    cb.sType            = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    cb.attachmentCount  = 1;
    cb.pAttachments     = &att_state[0];

    // We define a simple viewport and scissor. It does not change during rendering
    // in this sample.
    VkPipelineViewportStateCreateInfo      vp = {}; 
    vp.sType            = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    vp.viewportCount    = 1;
    vp.scissorCount     = 1;

    VkViewport viewport = {}; 
    viewport.height = (float) mHeight;
    viewport.width = (float) mWidth;
    viewport.minDepth = (float) 0.0f;
    viewport.maxDepth = (float) 1.0f;
    vp.pViewports = &viewport;

    VkRect2D scissor = {}; 
    scissor.extent.width    = mWidth;
    scissor.extent.height   = mHeight;
    scissor.offset.x        = 0;
    scissor.offset.y        = 0;
    vp.pScissors = &scissor;

    // Standard depth and stencil state is defined
    VkPipelineDepthStencilStateCreateInfo  ds = {}; 
    ds.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    ds.depthTestEnable          = VK_TRUE;
    ds.depthWriteEnable         = VK_TRUE;
    ds.depthCompareOp           = VK_COMPARE_OP_LESS_OR_EQUAL;
    ds.depthBoundsTestEnable    = VK_FALSE;
    ds.back.failOp              = VK_STENCIL_OP_KEEP;
    ds.back.passOp              = VK_STENCIL_OP_KEEP;
    ds.back.compareOp           = VK_COMPARE_OP_ALWAYS;
    ds.stencilTestEnable        = VK_FALSE;
    ds.front                    = ds.back;

    // We do not use multisample
    VkPipelineMultisampleStateCreateInfo   ms = {}; 
    ms.sType                = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    ms.pSampleMask          = nullptr;
    ms.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    // We define two shader stages: our vertex and fragment shader.
    // they are embedded as SPIR-V into a header file for ease of deployment.
    VkPipelineShaderStageCreateInfo shaderStages[2] = {};
    shaderStages[0].sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[0].stage  = VK_SHADER_STAGE_VERTEX_BIT;
    shaderStages[0].module = CreateShaderModule( (const uint32_t*)&shader_tri_vert[0], shader_tri_vert_size);
    shaderStages[0].pName  = "main";
    shaderStages[1].sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[1].stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
    shaderStages[1].module = CreateShaderModule( (const uint32_t*)&shader_tri_frag[0], shader_tri_frag_size);
    shaderStages[1].pName  = "main";

    // Pipelines are allocated from pipeline caches.
    VkPipelineCacheCreateInfo pipelineCache = {};
    pipelineCache.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    pipelineCache.pNext = nullptr;
    pipelineCache.flags = 0;

    VkPipelineCache piplineCache;
    err = vkCreatePipelineCache(mDevice, &pipelineCache, nullptr, &piplineCache);
    VK_CHECK(!err);

    // Out graphics pipeline records all state information, including our renderpass
    // and pipeline layout. We do not have any dynamic state in this example.
    VkGraphicsPipelineCreateInfo pipelineCreateInfo = {}; 
    pipelineCreateInfo.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.layout              = mPipelineLayout;
    pipelineCreateInfo.pVertexInputState   = &vi;
    pipelineCreateInfo.pInputAssemblyState = &ia;
    pipelineCreateInfo.pRasterizationState = &rs;
    pipelineCreateInfo.pColorBlendState    = &cb;
    pipelineCreateInfo.pMultisampleState   = &ms;
    pipelineCreateInfo.pViewportState      = &vp;
    pipelineCreateInfo.pDepthStencilState  = &ds;
    pipelineCreateInfo.pStages             = &shaderStages[0];
    pipelineCreateInfo.renderPass          = mRenderPass;
    pipelineCreateInfo.pDynamicState       = nullptr;
    pipelineCreateInfo.stageCount          = 2; //vertex and fragment

    err = vkCreateGraphicsPipelines(mDevice, piplineCache, 1, &pipelineCreateInfo, nullptr, &mPipeline);
    VK_CHECK(!err);

    // We can destroy the cache now as we do not need it. The shader modules also
    // can be destroyed after the pipeline is created.
    vkDestroyPipelineCache(mDevice, piplineCache, nullptr);

    vkDestroyShaderModule(mDevice, shaderStages[0].module, nullptr);
    vkDestroyShaderModule(mDevice, shaderStages[1].module, nullptr);
}
///////////////////////////////////////////////////////////////////////////////

VkShaderModule VkSample::CreateShaderModule(const uint32_t* code, uint32_t size)
{
    VkShaderModule module;
    VkResult  err;

    // Creating a shader is very simple once it's in memory as compiled SPIR-V.
    VkShaderModuleCreateInfo moduleCreateInfo = {};
    moduleCreateInfo.sType      = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    moduleCreateInfo.pNext      = nullptr;
    moduleCreateInfo.codeSize   = size;
    moduleCreateInfo.pCode      = code;
    moduleCreateInfo.flags      = 0;
    err = vkCreateShaderModule(mDevice, &moduleCreateInfo, nullptr, &module);
    VK_CHECK(!err);

    return module;
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

void VkSample::InitSync()
{
    VkResult ret = VK_SUCCESS;
    // For synchronization, we have semaphores for rendering and backbuffer signalling.
    VkSemaphoreCreateInfo semaphoreCreateInfo = {};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreCreateInfo.pNext = nullptr;
    semaphoreCreateInfo.flags = 0;
    ret = vkCreateSemaphore(mDevice, &semaphoreCreateInfo, nullptr, &mBackBufferSemaphore);
    VK_CHECK(!ret);

    ret = vkCreateSemaphore(mDevice, &semaphoreCreateInfo, nullptr, &mRenderCompleteSemaphore);
    VK_CHECK(!ret);

    // Create a fence to use for syncing the layout changes
    VkFenceCreateInfo fenceCreateInfo= {};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    ret = vkCreateFence(mDevice, &fenceCreateInfo, nullptr, &mFence);
}

///////////////////////////////////////////////////////////////////////////////

bool VkSample::GetMemoryTypeFromProperties( uint32_t typeBits, VkFlags requirements_mask, uint32_t* typeIndex)
{
    VK_CHECK(typeIndex != nullptr);
    // Search memtypes to find first index with those properties
    for (uint32_t i = 0; i < 32; i++)
    {
        if ((typeBits & 1) == 1)
        {
            // Type is available, does it match user properties?
            if ((mPhysicalDeviceMemoryProperties.memoryTypes[i].propertyFlags &
                 requirements_mask) == requirements_mask)
            {
                *typeIndex = i;
                return true;
            }
        }
        typeBits >>= 1;
    }
    // No memory types matched, return failure
    return false;
}

///////////////////////////////////////////////////////////////////////////////

void VkSample::SetNextBackBuffer()
{
    VkResult ret = VK_SUCCESS;

    // Get the next image to render to, then queue a wait until the image is ready
    ret  = vkAcquireNextImageKHR(mDevice, mSwapchain, UINT64_MAX, mBackBufferSemaphore, VK_NULL_HANDLE, &mSwapchainCurrentIdx);
    if (ret == VK_ERROR_OUT_OF_DATE_KHR)
    {
        LOGW("VK_ERROR_OUT_OF_DATE_KHR not handled in sample");
    } else if (ret == VK_SUBOPTIMAL_KHR)
    {
        LOGW("VK_SUBOPTIMAL_KHR not handled in sample");
    }
    VK_CHECK(!ret);
}

///////////////////////////////////////////////////////////////////////////////

void VkSample::PresentBackBuffer()
{
    VkResult ret = VK_SUCCESS;

    // Use WSI to present. The semaphore chain used to signal rendering
    // completion allows the operation to wait before the present is
    // completed.
    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType               = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.swapchainCount      = 1;
    presentInfo.pSwapchains         = &mSwapchain;
    presentInfo.pImageIndices       = &mSwapchainCurrentIdx;
    presentInfo.waitSemaphoreCount  = 1;
    presentInfo.pWaitSemaphores     = &mRenderCompleteSemaphore;

    ret = vkQueuePresentKHR(mQueue, &presentInfo);
    VK_CHECK(!ret);

    // Obtain the back buffer for the next frame.
    SetNextBackBuffer();
}

///////////////////////////////////////////////////////////////////////////////

bool VkSample::TearDown()
{
    if (!mInitBegun)
    {
        return false;
    }

    // Destroy all resources for framebuffers, swapchain images, and depth buffers
    for (uint32_t i = 0; i < mSwapchainImageCount; i++)
    {
        vkDestroyFramebuffer(mDevice, mFrameBuffers[i], nullptr);

        vkFreeCommandBuffers(mDevice, mCommandPool, 1, &mSwapchainBuffers[i].cmdBuffer);
        vkDestroyImageView(mDevice, mSwapchainBuffers[i].view, nullptr);
        vkDestroyImage(mDevice, mDepthBuffers[i].image, nullptr);
        vkDestroyImageView(mDevice, mDepthBuffers[i].view, nullptr);

        vkFreeMemory(mDevice, mDepthBuffers[i].mem, nullptr);
    }

    // Destroy the swapchain
    vkDestroySwapchainKHR(mDevice, mSwapchain, nullptr);

    delete [] mFrameBuffers;
    delete [] mSwapchainBuffers;
    delete [] mDepthBuffers;

    // Destroy pools
    vkDestroyCommandPool(mDevice, mCommandPool, nullptr);

    // Destroy pipeline resources
    vkDestroyPipeline(mDevice, mPipeline, nullptr);
    vkDestroyRenderPass(mDevice, mRenderPass, nullptr);
    vkDestroyPipelineLayout(mDevice, mPipelineLayout, nullptr);
    vkDestroyDescriptorSetLayout(mDevice, mDescriptorLayout, nullptr);

    // Destroy sync
    vkDestroySemaphore(mDevice, mBackBufferSemaphore, nullptr);
    vkDestroySemaphore(mDevice, mRenderCompleteSemaphore, nullptr);
    vkDestroyFence(mDevice, mFence, nullptr);

    // Destroy vertices
    vkDestroyBuffer(mDevice, mVertices.buf, nullptr);
    vkFreeMemory(mDevice, mVertices.mem, nullptr);

    // Destroy the device and surface
    vkDestroyDevice(mDevice, nullptr);
    vkDestroySurfaceKHR(mInstance, mSurface, nullptr);

    // Destroy the callback
    if (mDebugReportCallback)
    {
        mDestroyDebugReportCallbackEXT(mInstance, mDebugReportCallback, nullptr);
    }

    // Destroy the instance
    vkDestroyInstance(mInstance, nullptr);

    delete [] mpPhysicalDevices;

    mInitialized = false;
    mInitBegun = false;
    
    return true;
}

///////////////////////////////////////////////////////////////////////////////

void VkSample::SetImageLayout(VkImage image, VkCommandBuffer cmdBuffer, VkImageAspectFlags aspect, VkImageLayout oldLayout, VkImageLayout newLayout, VkPipelineStageFlags srcMask, VkPipelineStageFlags dstMask, uint32_t mipLevel, uint32_t mipLevelCount)
{
    VkImageMemoryBarrier imageMemoryBarrier = {};
    imageMemoryBarrier.sType                            = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageMemoryBarrier.pNext                            = nullptr;
    imageMemoryBarrier.oldLayout                        = oldLayout;
    imageMemoryBarrier.newLayout                        = newLayout;
    imageMemoryBarrier.image                            = image;
    imageMemoryBarrier.subresourceRange.aspectMask      = aspect;
    imageMemoryBarrier.subresourceRange.baseMipLevel    = mipLevel;
    imageMemoryBarrier.subresourceRange.levelCount      = mipLevelCount;
    imageMemoryBarrier.subresourceRange.baseArrayLayer  = 0;
    imageMemoryBarrier.subresourceRange.layerCount      = 1;
    imageMemoryBarrier.srcAccessMask                    = 0;
    imageMemoryBarrier.dstAccessMask                    = 0;
    imageMemoryBarrier.srcQueueFamilyIndex              = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier.dstQueueFamilyIndex              = VK_QUEUE_FAMILY_IGNORED;

    if (oldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
    {
        imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    }
    if (oldLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
    {
        imageMemoryBarrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    }
    if (oldLayout == VK_IMAGE_LAYOUT_PREINITIALIZED)
    {
        imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
    }
    if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    }

    if (newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
    {
        // Ensures reads can be made
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    }
    if (newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        // Ensures writes can be made
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    }
    if (newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
    {
        // Ensure writes have completed
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    }
    if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
    {
        // Ensure writes have completed
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    }
    if (newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        // Make sure any Copy or CPU writes to image are flushed
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
    }
    if (newLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
    {
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    }

    // Barrier on image memory, with correct layouts set.
    vkCmdPipelineBarrier(cmdBuffer, srcMask /*VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT*/, dstMask /*VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT*/, 0, 0, NULL, 0, NULL, 1, &imageMemoryBarrier);
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

        // Bind our vertex buffer, with a 0 offset.
        VkDeviceSize offsets[1] = {0};
        vkCmdBindVertexBuffers(cmdBuffer, VERTEX_BUFFER_BIND_ID, 1, &mVertices.buf, offsets);

        // Issue a draw command, with our 3 vertices.
        vkCmdDraw(cmdBuffer, 3, 1, 0, 0);

        // Now our render pass has ended.
        vkCmdEndRenderPass(cmdBuffer);

        // As stated earlier, now transition the swapchain image to the PRESENT mode.
        SetImageLayout(mSwapchainBuffers[i].image, cmdBuffer, VK_IMAGE_ASPECT_COLOR_BIT,                               VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,         VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);
        SetImageLayout(mDepthBuffers[i].image,     cmdBuffer, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);

        // By ending the command buffer, it is put out of record mode.
        err = vkEndCommandBuffer(cmdBuffer);
        VK_CHECK(!err);
    }
}

///////////////////////////////////////////////////////////////////////////////

void VkSample::DrawFrame()
{
    if (!mInitialized)
    {
        return;
    }

    VkFence nullFence = VK_NULL_HANDLE;
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

    err = vkQueueSubmit(mQueue, 1, &submitInfo, VK_NULL_HANDLE);
    VK_CHECK(!err);

    PresentBackBuffer();

    // Calculate FPS below, displaying every 30 frames
    mFrameTimeEnd = GetTime();
    if (mFrameIdx % 30 == 0)
    {
        uint64_t frameDelta = mFrameTimeEnd - mFrameTimeBegin;
        double fps = 1000000000.0/((double)frameDelta);

        LOGI("FPS: %f", fps);
    }
    mFrameIdx++;
    mFrameTimeBegin = GetTime();

    // uncomment to invoke teardown logic
    //if (mFrameIdx==100)
    //{
    //   ShutDown();
    //}
}

///////////////////////////////////////////////////////////////////////////////

void VkSample::ShutDown()
{
    VkResult ret = VK_SUCCESS;
    ret = vkQueueWaitIdle(mQueue);
    VK_CHECK(!ret);
    TearDown();
    exit(0);
};


///////////////////////////////////////////////////////////////////////////////

uint64_t VkSample::GetTime()
{
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return (uint64_t) now.tv_sec*1000000000LL + now.tv_nsec;
}

///////////////////////////////////////////////////////////////////////////////

// Begin Android Glue entry point
#include <android_native_app_glue.h>

// Shared state for our app.
struct engine
{
    struct android_app* app;
    int animating;
    VkSample sample;
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
            engine->sample.TearDown();
            break;
        case APP_CMD_INIT_WINDOW:
        {
            if(!engine->sample.Initialize(engine->app->window))
            {
                LOGE("VkSample::Initialize Error");
                engine->sample.TearDown();
            }
            else
            {
                LOGI("VkSample::Initialize Success");
            }
            engine->animating = 1;
            break;}
        case APP_CMD_TERM_WINDOW:
            engine->sample.TearDown();
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
//android_native_app_glue.  It runs in its own thread, with its own
//event loop for receiving input events and doing other things.
void android_main(struct android_app* state)
{
    struct engine engine;

    // Make sure glue isn't stripped.
    app_dummy();

    memset(&engine, 0, sizeof(engine));
    state->userData = &engine;
    state->onAppCmd = engine_handle_cmd;
    engine.app = state;

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
                                      (void**)&source)) >= 0) {

            // Process this event.
            if (source != nullptr) {
                source->process(state, source);
            }

            // Check if we are exiting.
            if (state->destroyRequested != 0) {
                engine.sample.TearDown();
                return;
            }
        }

        if (engine.animating && engine.sample.IsInitialized())
        {
            engine.sample.DrawFrame();
        }
    }
}
//END Android Glue
///////////////////////////////////////////////////////////////////////////////
