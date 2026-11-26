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

#ifndef __T3D_VK_RENDER_BUFFER_H__
#define __T3D_VK_RENDER_BUFFER_H__


#include "T3DVKPrerequisites.h"


namespace Tiny3D
{
    class VKVertexDeclaration : public RHIVertexDeclaration
    {
    public:
        static VKVertexDeclarationPtr create();

        ~VKVertexDeclaration() override;

        void *getNativeObject() const override;

    protected:
        VKVertexDeclaration() = default;
    };
    
    class VKVertexBuffer : public RHIVertexBuffer
    {
    public:
        static VKVertexBufferPtr create();

        ~VKVertexBuffer() override;

        void *getNativeObject() const override;
        
        VkBuffer        VkBuf {VK_NULL_HANDLE};
        VkDeviceMemory  VkBufMemory {VK_NULL_HANDLE};
        
    protected:
        VKVertexBuffer() = default;
    };

    class VKIndexBuffer : public RHIIndexBuffer
    {
    public:
        static VKIndexBufferPtr create();

        ~VKIndexBuffer() override;

        void *getNativeObject() const override;
        
        VkBuffer        VkBuf {VK_NULL_HANDLE};
        VkDeviceMemory  VkBufMemory {VK_NULL_HANDLE};
        
    protected:
        VKIndexBuffer() = default;
    };

    class VKConstantBuffer : public RHIConstantBuffer
    {
    public:
        static VKConstantBufferPtr create();

        ~VKConstantBuffer() override;

        void *getNativeObject() const override;
        
        VkBuffer        VkBuf {VK_NULL_HANDLE};
        VkDeviceMemory  VkBufMemory {VK_NULL_HANDLE};

    protected:
        VKConstantBuffer() = default;
    };

    class VKPixelBuffer1D : public RHIPixelBuffer1D
    {
    public:
        static VKPixelBuffer1DPtr create();

        ~VKPixelBuffer1D() override;

        void *getNativeObject() const override;
        
        VkImage         VkTex {VK_NULL_HANDLE};
        VkDeviceMemory  VkTexMemory {VK_NULL_HANDLE};
        VkImageView     VkTexView {VK_NULL_HANDLE};
        
    protected:
        VKPixelBuffer1D() = default;
    };

    class VKPixelBuffer2D : public RHIPixelBuffer2D
    {
    public:
        static VKPixelBuffer2DPtr create();

        ~VKPixelBuffer2D() override;

        void *getNativeObject() const override;

        VkImage         VkTex {VK_NULL_HANDLE};
        VkDeviceMemory  VkTexMemory {VK_NULL_HANDLE};
        VkImageView     VkTexView {VK_NULL_HANDLE};
        /// Render target view (for render texture)
        VkImageView     VkRTView {VK_NULL_HANDLE};
        /// Depth stencil view
        VkImageView     VkDSView {VK_NULL_HANDLE};
        /// Current image layout (tracked for correct barrier transitions)
        VkImageLayout   VkCurrentLayout {VK_IMAGE_LAYOUT_UNDEFINED};
        /// 渲染纹理的 mip 层数、数组层数（Cube 为 6 的倍数）
        uint32_t        VkMipLevels {1};
        uint32_t        VkLayers {1};
        bool            VkCubemap {false};
        VkFormat        VkPixelFormat {VK_FORMAT_UNDEFINED};
        /// 按子资源缓存的附件 / 采样视图
        TMap<uint64_t, VkImageView> VkSubViews;

    protected:
        VKPixelBuffer2D() = default;
    };

    class VKPixelBuffer3D : public RHIPixelBuffer3D
    {
    public:
        void *getNativeObject() const override;

        VkImage         VkTex {VK_NULL_HANDLE};
        VkDeviceMemory  VkTexMemory {VK_NULL_HANDLE};
        VkImageView     VkTexView {VK_NULL_HANDLE};
    };

    class VKPixelBufferCubemap : public RHIPixelBufferCubemap
    {
    public:
        static VKPixelBufferCubemapPtr create();

        ~VKPixelBufferCubemap() override;

        void *getNativeObject() const override;

        VkImage         VkTex {VK_NULL_HANDLE};
        VkDeviceMemory  VkTexMemory {VK_NULL_HANDLE};
        VkImageView     VkTexView {VK_NULL_HANDLE};
        /// Current image layout (tracked for correct barrier transitions)
        VkImageLayout   VkCurrentLayout {VK_IMAGE_LAYOUT_UNDEFINED};

    protected:
        VKPixelBufferCubemap() = default;
    };

    /**
     * \brief Vulkan 查询对象
     * \remarks 每个在途帧一个查询槽：第 f 帧发起用槽 f，等到该帧槽的 fence 之后读结果并在新命令缓冲开头 reset，
     *          所以同一个对象可以每帧都发起而不必等上一帧的结果
     */
    class VKQuery : public RHIQuery
    {
    public:
        static VKQueryPtr create(QueryType type);

        ~VKQuery() override;

        void *getNativeObject() const override;

        VkQueryPool     VkPool {VK_NULL_HANDLE};
        /// 每个槽的结果占用的 uint64 个数（管线统计为启用的计数器个数）
        uint32_t        ValueCount {1};
        /// 最近一次 begin / end 所在的帧序号，同一帧只能发起一次
        uint64_t        LastBeginFrame {0};
        uint64_t        LastEndFrame {0};
        /// begin 时是否在 render pass 内；Vulkan 要求 begin / end 在同一个 subpass 或都在 pass 外
        bool            BeganInRenderPass {false};

    protected:
        explicit VKQuery(QueryType type) : RHIQuery(type) {}
    };
}


#endif    /*__T3D_VK_RENDER_BUFFER_H__*/
