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
#include "Render/T3DRasterizerState.h"
#include "Render/T3DBlendState.h"
#include "Render/T3DDepthStencilState.h"


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
        TResult setScissorRect(int32_t x, int32_t y, uint32_t width, uint32_t height) override;
        TResult clearColor(const ColorRGB &color) override;
        TResult clearDepth(Real depth) override;
        TResult clearDepthStencil(Real depth, uint32_t stencil) override;
        TResult clearStencil(uint32_t stencil) override;
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
        RHIPixelBufferCubemapPtr createPixelBufferCubemap(PixelBufferCubemap *buffer) override;
        RHIShaderPtr createVertexShader(ShaderVariant *shader) override;
        TResult setVertexShader(ShaderVariant *shader) override;
        TResult setVSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) override;
        TResult setVSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers, const TextureSubresources &subresources = TextureSubresources()) override;
        TResult setVSSamplers(uint32_t startSlot, const Samplers &samplers) override;
        RHIShaderPtr createPixelShader(ShaderVariant *shader) override;
        TResult setPixelShader(ShaderVariant *shader) override;
        TResult setPSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) override;
        TResult setPSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers, const TextureSubresources &subresources = TextureSubresources()) override;
        TResult setPSSamplers(uint32_t startSlot, const Samplers &samplers) override;
        RHIShaderPtr createHullShader(ShaderVariant *shader) override;
        TResult setHullShader(ShaderVariant *shader) override;
        TResult setHSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) override;
        TResult setHSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers, const TextureSubresources &subresources = TextureSubresources()) override;
        TResult setHSSamplers(uint32_t startSlot, const Samplers &samplers) override;
        RHIShaderPtr createDomainShader(ShaderVariant *shader) override;
        TResult setDomainShader(ShaderVariant *shader) override;
        TResult setDSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) override;
        TResult setDSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers, const TextureSubresources &subresources = TextureSubresources()) override;
        TResult setDSSamplers(uint32_t startSlot, const Samplers &samplers) override;
        RHIShaderPtr createGeometryShader(ShaderVariant *shader) override;
        TResult setGeometryShader(ShaderVariant *shader) override;
        TResult setGSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) override;
        TResult setGSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers, const TextureSubresources &subresources = TextureSubresources()) override;
        TResult setGSSamplers(uint32_t startSlot, const Samplers &samplers) override;
        RHIShaderPtr createComputeShader(ShaderVariant *shader) override;
        TResult setComputeShader(ShaderVariant *shader) override;
        TResult setCSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers) override;
        TResult setCSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers, const TextureSubresources &subresources = TextureSubresources()) override;
        TResult setCSSamplers(uint32_t startSlot, const Samplers &samplers) override;
        RHIStructuredBufferPtr createStructuredBuffer(StructuredBuffer *buffer) override;
        TResult setVSStructuredBuffers(uint32_t startSlot, const ShaderResourceBuffers &buffers) override;
        TResult setPSStructuredBuffers(uint32_t startSlot, const ShaderResourceBuffers &buffers) override;
        TResult setCSStructuredBuffers(uint32_t startSlot, const ShaderResourceBuffers &buffers) override;
        TResult setCSUnorderedAccessBuffers(uint32_t startSlot, const UnorderedAccessBuffers &buffers, const UAVInitialCounts &initialCounts = UAVInitialCounts(), const TextureSubresources &subresources = TextureSubresources()) override;
        TResult dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) override;
        TResult dispatchIndirect(RenderBuffer *argsBuffer, size_t argsOffset = 0) override;
        TResult uavBarrier(const UnorderedAccessBuffers &buffers) override;
        TResult copyStructureCount(RenderBuffer *dstBuffer, size_t dstOffset, RenderBuffer *srcBuffer) override;
        TResult setPrimitiveType(PrimitiveType primitive) override;
        TResult render(uint32_t indexCount, uint32_t startIndex, uint32_t baseVertex) override;
        TResult render(uint32_t vertexCount, uint32_t startVertex) override;
        TResult renderIndexedInstanced(uint32_t indexCount, uint32_t instanceCount, uint32_t startIndex, int32_t baseVertex, uint32_t startInstance) override;
        TResult renderInstanced(uint32_t vertexCount, uint32_t instanceCount, uint32_t startVertex, uint32_t startInstance) override;
        TResult renderIndexedIndirect(RenderBuffer *argsBuffer, size_t argsOffset = 0) override;
        TResult renderIndirect(RenderBuffer *argsBuffer, size_t argsOffset = 0) override;
        TResult reset() override;
        TResult blit(RenderTarget *src, RenderTarget *dst, const Vector3 &srcOffset = Vector3::ZERO, const Vector3 &size = Vector3::ZERO, const Vector3 dstOffset = Vector3::ZERO) override;
        TResult blit(Texture *src, RenderTarget *dst, const Vector3 &srcOffset = Vector3::ZERO, const Vector3 &size = Vector3::ZERO, const Vector3 dstOffset = Vector3::ZERO) override;
        TResult blit(RenderTarget *src, Texture *dst, const Vector3 &srcOffset = Vector3::ZERO, const Vector3 &size = Vector3::ZERO, const Vector3 dstOffset = Vector3::ZERO) override;
        TResult blit(Texture *src, Texture *dst, const Vector3 &srcOffset = Vector3::ZERO, const Vector3 &size = Vector3::ZERO, const Vector3 dstOffset = Vector3::ZERO) override;
        TResult copyBuffer(RenderBuffer *src, RenderBuffer *dst, size_t srcOffset = 0, size_t size = 0, size_t dstOffset = 0) override;

        TResult generateMips(PixelBuffer *buffer) override;
        TResult writeBuffer(RenderBuffer *renderBuffer, const Buffer &buffer, bool discardWholeBuffer = false) override;

        /// GPU 读回未实现。落地时 Copy 进 host-visible 资源，endRender 的 fence 即完成信号
        ReadbackHandle map(RenderBuffer *src, size_t offset, size_t size) override;
        ReadbackHandle map(RenderBuffer *src, const ReadbackRegion &region) override;
        TResult unmap(ReadbackHandle handle, Buffer &dst) override;

        TResult beginRender() override;
        TResult endRender() override;

        TResult beginPass() override;
        TResult endPass() override;

        /// VK_EXT_debug_utils 的命令缓冲 label，只在 beginRender / endRender 之间生效
        TResult beginDebugGroup(const String &name) override;
        TResult endDebugGroup() override;
        TResult setDebugMarker(const String &name) override;

        /// vkSetDebugUtilsObjectNameEXT
        TResult setDebugName(RenderResource *resource) override;

        /**
         * \brief 创建查询；每个对象带 MAX_FRAMES_IN_FLIGHT 个槽，结果在同一帧槽的 fence 之后读取
         * \remarks 不支持条件渲染；同一个查询每帧只能发起一次
         */
        RHIQueryPtr createQuery(QueryType type) override;
        TResult beginQuery(RHIQuery *query) override;
        TResult endQuery(RHIQuery *query) override;

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

        TResult createVkImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage &image, VkDeviceMemory &imageMemory,
            uint32_t mipLevels = 1, uint32_t arrayLayers = 1, VkImageCreateFlags flags = 0);

        VkImageView createVkImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags,
            VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D, uint32_t baseMip = 0, uint32_t mipCount = 1,
            uint32_t baseLayer = 0, uint32_t layerCount = 1);

        /// 取渲染纹理某个子资源的视图（附件视图只取单个 mip），按子资源缓存
        VkImageView getVkSubresourceView(VKPixelBuffer2D *pb, const TextureSubresource &sub, bool attachment,
            uint32_t *outLayers = nullptr);

        VkCommandBuffer beginSingleTimeCommands();

        void endSingleTimeCommands(VkCommandBuffer commandBuffer);

        void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

        /// 与 transitionImageLayout 相同，但一次性覆盖 cubemap 的 6 个 array layer
        void transitionCubemapLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout);

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
        // Cached render state descriptors (for pipeline creation)
        //----------------------------------------------------------------------

        /// Current rasterizer state (used in pipeline creation)
        RasterizerDesc      mCurrentRasterizerDesc {};
        /// Current blend state (used in pipeline creation)
        BlendDesc           mCurrentBlendDesc {};
        /// Current depth/stencil state (used in pipeline creation)
        DepthStencilDesc    mCurrentDepthStencilDesc {};

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

        /// Pending scissor rect (top-left origin, Y-down). Applied in render() before draw.
        VkRect2D            mPendingScissor {};
        bool                mPendingScissorValid {false};

        /// Bind descriptor set with reflection-driven resource matching
        TResult bindDescriptorSet(VkCommandBuffer cmdBuf, VkPipelineLayout pipelineLayout);

        /// 按物理设备属性填充 mCapabilities
        void fillCapabilities();

        /// 查询 VK_EXT_debug_utils 的函数指针，实例没开扩展时保持为空
        void loadDebugUtils();

        /// 给 Vulkan 对象命名
        void setVkObjectName(VkObjectType type, uint64_t handle, const String &name);

        /// 读出槽 frameSlot 上已完成的查询并发布结果，再在 cmdBuf 里 reset 这些槽
        void resolveFrameQueries(uint32_t frameSlot, VkCommandBuffer cmdBuf);

        /// 一次已 end、等待本帧槽 fence 的查询
        struct PendingQuery
        {
            VKQueryPtr  Query {nullptr};
            uint32_t    Serial {0};
        };

        /**
         * \brief 提交 draw call 前的公共准备：管线、动态状态、描述符集
         * \param [out] outCmdBuf : 本帧的命令缓冲
         * \return 不在 render pass 内或管线创建失败时返回对应错误码
         */
        TResult prepareDrawCall(VkCommandBuffer &outCmdBuf);

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

        /// 实例是否开启了 VK_EXT_debug_utils
        bool                                mDebugUtilsEnabled {false};
        PFN_vkCmdBeginDebugUtilsLabelEXT    mVkCmdBeginDebugUtilsLabel {nullptr};
        PFN_vkCmdEndDebugUtilsLabelEXT      mVkCmdEndDebugUtilsLabel {nullptr};
        PFN_vkCmdInsertDebugUtilsLabelEXT   mVkCmdInsertDebugUtilsLabel {nullptr};
        PFN_vkSetDebugUtilsObjectNameEXT    mVkSetDebugUtilsObjectName {nullptr};
        /// 本帧已打开、尚未关闭的调试分组数，endRender 时补齐
        uint32_t                            mDebugGroupDepth {0};

        /// beginRender 成功开始录制后为 true，endRender 提交后为 false
        bool                mFrameRecording {false};
        /// 帧序号，每次 beginRender 加一，用来拦截同一帧重复发起的查询
        uint64_t            mFrameCounter {0};
        /// 时间戳的有效位掩码与每 tick 纳秒数
        uint64_t            mTimestampMask {0};
        double              mTimestampPeriod {0.0};
        /// 是否开启了 pipelineStatisticsQuery / occlusionQueryPrecise 特性
        bool                mPipelineStatisticsEnabled {false};
        bool                mOcclusionPreciseEnabled {false};
        /// 每个帧槽上已发起、等待 fence 的查询
        std::vector<PendingQuery>   mFrameQueries[MAX_FRAMES_IN_FLIGHT];
    };
}


#endif  /*__T3D_VK_CONTEXT_H__*/
