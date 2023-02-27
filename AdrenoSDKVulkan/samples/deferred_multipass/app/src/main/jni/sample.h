#pragma once

#include "VkSampleframework.h"
#include <stdlib.h>

#define GLM_FORCE_CXX03
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

    void InitVertexBuffers();
    void InitUniformBuffers();
    void InitTextures();

    ImageViewObject mPositionTarget;
    ImageViewObject mNormalTarget;
    ImageViewObject mColorTarget;

    TextureObject mPositionTexture;
    TextureObject mNormalTexture;
    TextureObject mColorTexture;

    uint32_t mDeferredWidth;
    uint32_t mDeferredHeight;

    // State for the final render pass
    VkDescriptorSetLayout   mDescriptorLayoutSubpass2;
    VkDescriptorSetLayout   mDescriptorLayoutSubpass1;
    VkPipelineLayout        mPipelineLayoutSubpass2;
    VkPipelineLayout        mPipelineLayoutSubpass1;
    VkRenderPass            mRenderPass;
    VkFramebuffer*          mFrameBuffers;
    VkPipeline              mPipelineSubpass1;
    VkPipeline              mPipelineSubpass2;

    VkDescriptorPool        mDescriptorPool;
    VkDescriptorSet         mDescriptorSetSubpass1;
    VkDescriptorSet         mDescriptorSetSubpass2;

    VertexBufferObject      mVertexBuff;
    VertexBufferObject      mQuadVertices;
    VertexBufferObject      mInspectVertices;

    //Textures
    TextureObject           mTex;
    TextureObject           mNormalTex;

    //Uniforms
    struct VertexUniform
    {
        glm::mat4 modelViewProjection;
        glm::mat4 model;
        glm::mat4 view;
        glm::vec4 camPos;
    };
    VertexUniform           mVertUniformData;
    BufferObject            mVertShaderUniformBuffer;

    struct LightConstants
    {
        glm::vec4 posAndSize[4];
        glm::vec4 color[4];
    };

    LightConstants          mLights;
    BufferObject            mLightsUniformBuffer;

    MeshObject              mMesh;

    glm::mat4               mProjectionMatrix;
    glm::mat4               mViewMatrix;
    glm::mat4               mModelMatrix;
    glm::vec3               mCameraPos;
};

