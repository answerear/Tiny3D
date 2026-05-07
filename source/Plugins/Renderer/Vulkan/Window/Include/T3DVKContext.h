/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/


#ifndef __T3D_VK_CONTEXT_H__
#define __T3D_VK_CONTEXT_H__


#include "T3DVKPrerequisites.h"
#include <unordered_map>
#include <string>
#include "T3DVKContextBase.h"


namespace Tiny3D
{
    class VKContext : public VKContextBase
    {
    public:
        static VKContextPtr create();

        TResult init();
        
        TResult swapBackBuffer(VKRenderWindow *renderWindow);

        /**
         * @brief 改变渲染窗口大小
         */
        TResult resizeRenderWindow(VKRenderWindow *rw, uint32_t w, uint32_t h);

        /**
         * @brief 改变渲染纹理大小
         */
        TResult resizeRenderTexture(RenderTexture *rt, uint32_t w, uint32_t h);

        /**
         * @brief 改变渲染目标大小
         */
        TResult resizeRenderTarget(RenderTarget *rt, uint32_t w, uint32_t h);

        RHIRenderTargetPtr createRenderWindow(RenderWindow *renderWindow) override;
        RHIPixelBuffer2DPtr createRenderTexture(PixelBuffer2D *buffer) override;
        TResult setRenderTarget(RenderTarget *renderTarget) override;
        TResult resetRenderTarget() override;
        TResult setViewport(const Viewport &viewport) override;
        TResult clearColor(const ColorRGB &color) override;
        TResult clearDepth(Real depth) override;
        TResult clearDepthStencil(Real depth, uint32_t stencil) override;
        RHIBlendStatePtr createBlendState(BlendState *state) override;
        RHIDepthStencilStatePtr createDepthStencilState(DepthStencilState *state) override;
        RHIRasterizerStatePtr createRasterizerState(RasterizerState *state) override;
        RHISamplerStatePtr createSamplerState(SamplerState *state) override;
        TResult setBlendState(BlendState *state) override;
        TResult setDepthStencilState(DepthStencilState *state) override;
        TResult setRasterizerState(RasterizerState *state) override;
        RHIVertexDeclarationPtr createVertexDeclaration(VertexDeclaration *decl) override;
        TResult setVertexDeclaration(VertexDeclaration *decl) override;
        RHIVertexBufferPtr createVertexBuffer(VertexBuffer *buffer) override;
        TResult setVertexBuffers(uint32_t startSlot, const VertexBuffers &buffers, const VertexStrides &strides, const VertexOffsets &offsets) override;
        RHIIndexBufferPtr createIndexBuffer(IndexBuffer *buffer) override;
        TResult setIndexBuffer(IndexBuffer *buffer) override;
        RHIConstantBufferPtr createConstantBuffer(ConstantBuffer *buffer) override;
        RHIPixelBuffer1DPtr createPixelBuffer1D(PixelBuffer1D *buffer) override;
        RHIPixelBuffer2DPtr createPixelBuffer2D(PixelBuffer2D *buffer) override;
        RHIPixelBuffer3DPtr createPixelBuffer3D(PixelBuffer3D *buffer) override;
        RHIShaderPtr createVertexShader(ShaderVariant *shader) override;
        TResult setVertexShader(ShaderVariant *shader) override;
        TResult setVSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) override;
        TResult setVSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) override;
        TResult setVSSamplers(uint32_t startSlot, const Samplers &samplers) override;
        RHIShaderPtr createPixelShader(ShaderVariant *shader) override;
        TResult setPixelShader(ShaderVariant *shader) override;
        TResult setPSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) override;
        TResult setPSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) override;
        TResult setPSSamplers(uint32_t startSlot, const Samplers &samplers) override;
        RHIShaderPtr createHullShader(ShaderVariant *shader) override;
        TResult setHullShader(ShaderVariant *shader) override;
        TResult setHSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) override;
        TResult setHSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) override;
        TResult setHSSamplers(uint32_t startSlot, const Samplers &samplers) override;
        RHIShaderPtr createDomainShader(ShaderVariant *shader) override;
        TResult setDomainShader(ShaderVariant *shader) override;
        TResult setDSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) override;
        TResult setDSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) override;
        TResult setDSSamplers(uint32_t startSlot, const Samplers &samplers) override;
        RHIShaderPtr createGeometryShader(ShaderVariant *shader) override;
        TResult setGeometryShader(ShaderVariant *shader) override;
        TResult setGSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) override;
        TResult setGSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) override;
        TResult setGSSamplers(uint32_t startSlot, const Samplers &samplers) override;
        RHIShaderPtr createComputeShader(ShaderVariant *shader) override;
        TResult setComputeShader(ShaderVariant *shader) override;
        TResult setCSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) override;
        TResult setCSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers) override;
        TResult setCSSamplers(uint32_t startSlot, const Samplers &samplers) override;
        TResult setPrimitiveType(PrimitiveType primitive) override;
        TResult render(uint32_t indexCount, uint32_t startIndex, uint32_t baseVertex) override;
        TResult render(uint32_t vertexCount, uint32_t startVertex) override;
        TResult reset() override;
        TResult blit(RenderTarget *src, RenderTarget *dst, const Vector3 &srcOffset = Vector3::ZERO, const Vector3 &size = Vector3::ZERO, const Vector3 dstOffset = Vector3::ZERO) override;
        TResult blit(Texture *src, RenderTarget *dst, const Vector3 &srcOffset = Vector3::ZERO, const Vector3 &size = Vector3::ZERO, const Vector3 dstOffset = Vector3::ZERO) override;
        TResult blit(RenderTarget *src, Texture *dst, const Vector3 &srcOffset = Vector3::ZERO, const Vector3 &size = Vector3::ZERO, const Vector3 dstOffset = Vector3::ZERO) override;
        TResult blit(Texture *src, Texture *dst, const Vector3 &srcOffset = Vector3::ZERO, const Vector3 &size = Vector3::ZERO, const Vector3 dstOffset = Vector3::ZERO) override;
        TResult copyBuffer(RenderBuffer *src, RenderBuffer *dst, size_t srcOffset = 0, size_t size = 0, size_t dstOffset = 0) override;
        TResult writeBuffer(RenderBuffer *renderBuffer, const Buffer &buffer, bool discardWholeBuffer = false) override;

        TResult beginRender() override;
        TResult endRender() override;

        TResult beginPass() override;
        TResult endPass() override;

        VkDevice getVkDevice() const { return mVkDevice; }

        VkPhysicalDevice getVkPhysicalDevice() const { return mVkPhysicalDevice; }

        VkInstance getVkInstance() const { return mVkInstance; }

    protected:
        VKContext();

        ~VKContext() override;

        TResult createRenderWindow(VKRenderWindow *vkRenderWindow, uint32_t w, uint32_t h, uint32_t MSAACount, uint32_t MSAAQuality);

        TResult setRenderTarget(RenderWindow *renderWindow, RenderTexture *depthStencil);

        TResult setRenderTarget(const RenderTexturePtr *renderTexture, uint32_t numOfTextures, RenderTexture *depthStencil);

        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;

        TResult createVkBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory);

        TResult createVkImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage &image, VkDeviceMemory &imageMemory);

        VkImageView createVkImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);

        VkCommandBuffer beginSingleTimeCommands();

        void endSingleTimeCommands(VkCommandBuffer commandBuffer);

        void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

        bool isDepthFormat(VkFormat format) const;

        bool hasStencilComponent(VkFormat format) const;

        void insertImageBarrier(VkCommandBuffer cmdBuf, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout, VkAccessFlags srcAccess, VkAccessFlags dstAccess, VkPipelineStageFlags srcStage, VkPipelineStageFlags dstStage, VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT);

    protected:
        static const uint32_t MAX_FRAMES_IN_FLIGHT = 3;

        /// Vulkan instance
        VkInstance          mVkInstance {VK_NULL_HANDLE};
        /// Physical device
        VkPhysicalDevice    mVkPhysicalDevice {VK_NULL_HANDLE};
        /// Logical device
        VkDevice            mVkDevice {VK_NULL_HANDLE};
        /// Graphics queue
        VkQueue             mVkGraphicsQueue {VK_NULL_HANDLE};
        /// Present queue
        VkQueue             mVkPresentQueue {VK_NULL_HANDLE};
        /// Command pool
        VkCommandPool       mVkCommandPool {VK_NULL_HANDLE};

        /// Graphics queue family index
        uint32_t            mGraphicsQueueFamily {UINT32_MAX};
        /// Present queue family index
        uint32_t            mPresentQueueFamily {UINT32_MAX};

        /// Render pass (default)
        VkRenderPass        mVkRenderPass {VK_NULL_HANDLE};
        /// Pipeline layout
        VkPipelineLayout    mVkPipelineLayout {VK_NULL_HANDLE};
        /// Pipeline cache
        VkPipelineCache     mVkPipelineCache {VK_NULL_HANDLE};
        /// Descriptor set layout
        VkDescriptorSetLayout   mVkDescriptorSetLayout {VK_NULL_HANDLE};
        /// Descriptor pool (per-frame to avoid reset conflicts)
        VkDescriptorPool    mVkDescriptorPools[MAX_FRAMES_IN_FLIGHT] {};

        /// Per-frame command buffers
        std::vector<VkCommandBuffer>    mVkCommandBuffers;
        /// Per-frame: image available semaphores
        std::vector<VkSemaphore>        mVkImageAvailableSemaphores;
        /// Per-frame: render finished semaphores
        std::vector<VkSemaphore>        mVkRenderFinishedSemaphores;
        /// Per-frame: in-flight fences
        std::vector<VkFence>            mVkInFlightFences;

        /// Current frame index (0 ~ MAX_FRAMES_IN_FLIGHT-1)
        uint32_t            mCurrentFrame {0};

        /// Current render target
        RenderTargetPtr     mCurrentRenderTarget {nullptr};

        /// Current pipeline state
        VkPrimitiveTopology mVkPrimitiveTopology {VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST};

        /// Current pipeline (cached)
        VkPipeline          mVkCurrentPipeline {VK_NULL_HANDLE};

        /// Current render window (set during createRenderWindow)
        VKRenderWindow      *mCurrentRenderWindow {nullptr};

        /// Current swapchain image index (set by beginRender/acquireNextImage)
        uint32_t            mCurrentImageIndex {0};

        /// Current clear color (set by clearColor, used by clear operations)
        ColorRGB            mClearColor;

        //----------------------------------------------------------------------
        // Render state tracking (for pipeline creation and command recording)
        //----------------------------------------------------------------------

        /// Current vertex shader module
        VkShaderModule      mCurrentVSModule {VK_NULL_HANDLE};
        /// Current pixel shader module
        VkShaderModule      mCurrentPSModule {VK_NULL_HANDLE};
        /// Current vertex shader entry point name
        std::string         mCurrentVSEntryPoint {"main"};
        /// Current pixel shader entry point name
        std::string         mCurrentPSEntryPoint {"main"};
        /// Current vertex input binding descriptions
        std::vector<VkVertexInputBindingDescription>   mVertexBindings;
        /// Current vertex input attribute descriptions
        std::vector<VkVertexInputAttributeDescription> mVertexAttributes;
        /// Whether a render pass is currently active
        bool                mRenderPassActive {false};
        /// Pipeline cache (hash -> VkPipeline)
        std::unordered_map<size_t, VkPipeline> mPipelineCache;
        /// Descriptor set layout cache (binding hash -> VkDescriptorSetLayout)
        std::unordered_map<size_t, VkDescriptorSetLayout> mDescriptorSetLayoutCache;
        /// Pipeline layout cache (dsl hash -> VkPipelineLayout)
        std::unordered_map<size_t, VkPipelineLayout> mPipelineLayoutCache;

        //----------------------------------------------------------------------
        // Descriptor set management
        //----------------------------------------------------------------------

        /// Max descriptor sets per pool
        static const uint32_t MAX_DESCRIPTOR_SETS = 1000;

        /// Binding offset for pixel shader descriptors to avoid conflicts with VS bindings
        static const uint32_t PS_BINDING_OFFSET = 16;

        /// Current constant buffers (VS stage)
        std::vector<VkBuffer>   mCurrentVSConstantBuffers;
        std::vector<VkDeviceSize> mCurrentVSConstantBufferSizes;
        uint32_t                mCurrentVSCBStartSlot {0};
        /// Current constant buffers (PS stage)
        std::vector<VkBuffer>   mCurrentPSConstantBuffers;
        std::vector<VkDeviceSize> mCurrentPSConstantBufferSizes;
        uint32_t                mCurrentPSCBStartSlot {0};
        /// Current pixel buffers (textures, PS stage)
        std::vector<VkImageView> mCurrentPSImageViews;
        uint32_t                mCurrentPSTexStartSlot {0};
        /// Current samplers (PS stage)
        std::vector<VkSampler>  mCurrentPSSamplers;
        uint32_t                mCurrentPSSamplerStartSlot {0};

        /// Current VS shader binding info (from reflection)
        std::vector<VkDescriptorSetLayoutBinding> mCurrentVSBindings;
        /// Current PS shader binding info (from reflection)
        std::vector<VkDescriptorSetLayoutBinding> mCurrentPSBindings;

        //----------------------------------------------------------------------
        // Render pass / Framebuffer cache for render textures
        //----------------------------------------------------------------------

        /// Render pass cache (format hash -> VkRenderPass)
        std::unordered_map<size_t, VkRenderPass> mRenderPassCache;
        /// Framebuffer cache (RT pointer hash -> VkFramebuffer + extent)
        struct FramebufferEntry {
            VkFramebuffer framebuffer {VK_NULL_HANDLE};
            VkExtent2D extent {0, 0};
        };
        std::unordered_map<size_t, FramebufferEntry> mFramebufferCache;
        /// Current render pass for this pass (set by beginPass)
        VkRenderPass        mCurrentPassRenderPass {VK_NULL_HANDLE};
        /// Current framebuffer extent for this pass
        VkExtent2D          mCurrentPassExtent {0, 0};

        /// Bind descriptor set with reflection-driven resource matching
        TResult bindDescriptorSet(VkCommandBuffer cmdBuf, VkPipelineLayout pipelineLayout);

        /// Get or create a graphics pipeline based on current render state
        VkPipeline getOrCreatePipeline();
        /// Get or create descriptor set layout from current VS+PS bindings
        VkDescriptorSetLayout getOrCreateDescriptorSetLayout(size_t &outHash);
        /// Get or create pipeline layout from descriptor set layout
        VkPipelineLayout getOrCreatePipelineLayout(VkDescriptorSetLayout dsl, size_t dslHash);
        /// Get or create render pass for given attachment formats
        VkRenderPass getOrCreateRenderPass(VkFormat colorFormat, VkFormat depthFormat, bool hasColor, bool depthOnly);
        /// Get or create framebuffer for current render target
        VkFramebuffer getOrCreateFramebuffer(VkRenderPass renderPass, VkExtent2D &outExtent);

#if defined (T3D_DEBUG)
        /// Validation layers debug messenger
        VkDebugUtilsMessengerEXT    mVkDebugMessenger {VK_NULL_HANDLE};
#endif
    };
}


#endif  /*__T3D_VK_CONTEXT_H__*/
