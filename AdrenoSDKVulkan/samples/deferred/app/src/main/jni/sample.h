#pragma once_

#include "VkSampleframework.h"
#include <stdlib.h>

#define GLM_FORCE_CXX03
#define GLM_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "MeshObject.h"

class VkSample: public VkSampleFramework
{
public:
    VkSample();
    virtual ~VkSample();

protected:

    bool InitSample();
    bool Update();
    bool Draw();
    bool DestroySample();
    void WindowResize(uint32_t newWidth, uint32_t newHeight);

    void InitDescriptorPool();
    void InitDescriptorSet();
    void InitLayouts();
    void InitPipelines();
    void InitRenderPass();
    void InitFrameBuffers();
    void BuildCmdBuffer();

    void InitDeferredLayouts();
    void InitDeferredDescriptorSet();
    void InitDeferredFramebuffers();
    void InitDeferredPipelines();
    void InitDeferredTextures();
    void InitDeferredCommandBuffers();

    void InitVertexBuffers();
    void InitUniformBuffers();
    void InitTextures();

    //state for building g-buffers
    VkRenderPass            mDeferredRenderPass;
    VkFramebuffer           mDeferredFrameBuffer;
    VkDescriptorSetLayout   mDeferredDescriptorLayout;
    VkPipelineLayout        mDeferredPipelineLayout;
    VkDescriptorSet         mDeferredDescriptorSet;
    VkPipeline              mDeferredPipeline;
    VkCommandBuffer         mDeferredCommandBuffer;

    ImageViewObject mPositionTarget;
    ImageViewObject mNormalTarget;
    ImageViewObject mColorTarget;
    ImageViewObject mDepthTarget;

    TextureObject mPositionTexture;
    TextureObject mNormalTexture;
    TextureObject mColorTexture;

    uint32_t mDeferredWidth;
    uint32_t mDeferredHeight;

    // State for the final render pass
    VkDescriptorSetLayout   mDescriptorLayout;
    VkPipelineLayout        mPipelineLayout;
    VkRenderPass            mRenderPass;
    VkFramebuffer*          mFrameBuffers;
    VkPipeline              mPipeline;

    // Pipeline for showing G-buffers
    VkPipeline mInspectPipeline;

    VkDescriptorPool    mDescriptorPool;
    VkDescriptorSet     mDescriptorSet;

    // VBOs
    VertexBufferObject  mVertexBuff;
    VertexBufferObject  mQuadVertices;
    VertexBufferObject  mInspectVertices;

    // Textures
    TextureObject mTex;
    TextureObject mNormalTex;

    //Uniforms
    struct VertexUniform
    {
        glm::mat4 modelViewProjection;
        glm::mat4 model;
        glm::mat4 view;
        glm::vec4 camPos;
    };
    VertexUniform   mVertUniformData;
    BufferObject    mVertShaderUniformBuffer;

    struct LightConstants
    {
        glm::vec4 posAndSize[4];
        glm::vec4 color[4];
    };

    LightConstants mLights;
    BufferObject mLightsUniformBuffer;

    MeshObject mMesh;

    glm::mat4 mProjectionMatrix;
    glm::mat4 mViewMatrix;
    glm::mat4 mModelMatrix;
    glm::vec3 mCameraPos;
};

