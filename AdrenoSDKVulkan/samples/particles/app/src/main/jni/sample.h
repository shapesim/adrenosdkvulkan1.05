//                  Copyright (c) 2016 QUALCOMM Technologies Inc.
//                              All Rights Reserved.
#pragma once

#include "VkSampleframework.h"
#include <stdlib.h>

#define GLM_FORCE_CXX03
#define GLM_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define NUM_PARTICLES               100
#define FLOOR_SIZE                  5.00f
#define SPIRAL_SPACING              0.45f
#define MAX_VELOCITY_PER_SECOND     (FLOOR_SIZE/10.0);
#define RESTART_FRAMES              900
#define PARTICLE_MIN_LIFETIME       5.00f
#define PARTICLE_MAX_LIFETIME       10.00f
#define PARTICLE_SPAWN_DELAY        0.05f

struct ParticleData
{
    uint32_t    particleID;
    float       position[3];
    float       initialPosition[3];
    float       velocity[3];
    float       spawnTime;
    float       endTime;
    bool        bAlive;
    float       cameraDistance;
    uint32_t    tileID;
};

struct UniformData
{
    glm::mat4   modelviewprojectionMatrix;
    glm::mat4   inverseCameraMatrix;
    uint32_t    tileRows;
    uint32_t    tileCols;
};

struct Uniform
{
    VkBuffer                buf;
    VkDeviceMemory          mem;
    VkDescriptorBufferInfo  bufferInfo;
    VkDeviceSize            allocSize;
};

struct Vertices
{
    Vertices() : vi_bindings(nullptr), vi_attrs(nullptr)
    {
    }

    ~Vertices()
    {
        if (!vi_bindings)
        {
            return;
        }
        delete [] vi_bindings;
        delete [] vi_attrs;
    }

    VkBuffer buf;
    VkDeviceMemory mem;
    VkPipelineVertexInputStateCreateInfo    vi;
    VkVertexInputBindingDescription*        vi_bindings;
    VkVertexInputAttributeDescription*      vi_attrs;
    VkDeviceSize                            allocSize;
};

class VkSample : public VkSampleFramework
{
public:
    VkSample();
    ~VkSample();

    // Matrices
    glm::mat4               mProjectionMatrix;
    glm::mat4               mViewMatrix;
    glm::mat4               mInvViewMatrix;

protected:
    // Overloaded VkSampleFramework entry points.
    bool InitSample();
    bool Update();
    bool Draw();
    bool DestroySample();
    void WindowResize(uint32_t newWidth, uint32_t newHeight);

    // Vertices
    void InitVertexBuffers();
    void InitParticleVertexBuffer();
    void InitFloorVertexBuffer();

    void InitUniformBuffers();
    void InitLayouts();
    void InitRenderPass();
    void InitPipelines();
    void InitFrameBuffers();
    void InitDescriptorSets();
    void InitTextures();
    void BuildCmdBuffer();
    void UpdateUniforms();

    void UpdateParticleVertexBuffers();

    VkFramebuffer*          mFrameBuffers;
    Vertices                mParticleVertices;
    Vertices                mFloorVertices;

    // Pipelines
    VkPipelineLayout        mParticlePipelineLayout;
    VkPipeline              mParticlePipeline;
    VkPipelineLayout        mFloorPipelineLayout;
    VkPipeline              mFloorPipeline;

    // Render Pass
    VkRenderPass            mRenderPass;

    // Descriptor pool and set
    VkDescriptorPool        mDescriptorPool;
    VkDescriptorSet         mParticleDescriptorSet;
    VkDescriptorSet         mFloorDescriptorSet;
    VkDescriptorSetLayout   mParticleDescriptorLayout;
    VkDescriptorSetLayout   mFloorDescriptorLayout;

    // Fences
    VkFence                 mSubmitFences[3];
    bool                    mFenceSubmitted[3];

    //Uniforms
    Uniform                 mUniform;

    // Texture
    TextureObject*          mpParticleTextureObject;
    TextureObject*          mpFloorTextureObject;
    uint32_t                mTileFrameRate;
    uint32_t                mTileNumRows;
    uint32_t                mTileNumCols;
    uint32_t                mNumTiles;

    // Camera
    glm::vec3               mCameraPosition;
    glm::vec3               mCameraLookAt;
    glm::vec3               mCameraUp;

    // Time
    float                   mUpdateTimeSeconds;
    float                   mDeltaTimeSeconds;

    // Particles
    void InitParticles();
    void InitializeParticle(uint32_t p);
    void SortParticles();
    void UpdateParticles();
    void StartParticles();
    ParticleData*           mpParticles;
};
