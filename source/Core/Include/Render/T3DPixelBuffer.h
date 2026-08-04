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


#ifndef __T3D_PIXEL_BUFFER_H__
#define __T3D_PIXEL_BUFFER_H__


#include "Render/T3DRenderBuffer.h"
#include "Kernel/T3DConstant.h"
#include "Render/T3DRenderBufferDesc.h"


namespace Tiny3D
{
    /**
     * \brief 像素缓冲区基类，纹理与 RenderTexture 的 CPU/GPU 数据载体
     */
    class T3D_ENGINE_API PixelBuffer : public RenderBuffer
    {
    public:
        /**
         * \brief 构造像素缓冲区
         * \param [in] buffer : 初始 CPU 像素数据
         * \param [in] memType : 内存驻留类型
         * \param [in] usage : 缓冲区用途
         * \param [in] accMode : CPU 访问模式
         */
        PixelBuffer(const Buffer &buffer, MemoryType memType, Usage usage, uint32_t accMode);

    protected:
        ~PixelBuffer() override = default;
    };
    
    /**
     * \brief 带维度描述的像素缓冲区模板基类
     * \tparam Object_t : 具体像素缓冲类型（PixelBuffer1D/2D/3D/Cubemap）
     * \tparam Descriptor_t : 对应的描述结构体
     */
    template<typename Object_t, typename Descriptor_t>
    class T3D_ENGINE_API PixelBufferT : public PixelBuffer
    {
    public:
        /**
         * \brief 工厂创建具体维度的像素缓冲区
         * \param [in] desc : 像素缓冲描述（生命周期由 Object_t 持有）
         * \param [in] memType : 内存驻留类型
         * \param [in] usage : 缓冲区用途
         * \param [in] accMode : CPU 访问模式
         * \return 新建的 Object_t 智能指针
         */
        static SmartPtr<Object_t> create(Descriptor_t *desc, MemoryType memType, Usage usage, uint32_t accMode)
        {
            return T3D_NEW Object_t(desc, memType, usage, accMode);
        }

        /// 像素缓冲维度与格式描述
        const Descriptor_t &getDescriptor() const { return *mDesc; }

    protected:
        /**
         * \brief 构造并保存描述指针
         * \param [in] desc : 像素缓冲描述
         * \param [in] memType : 内存驻留类型
         * \param [in] usage : 缓冲区用途
         * \param [in] accMode : CPU 访问模式
         */
        PixelBufferT(Descriptor_t *desc, MemoryType memType, Usage usage, uint32_t accMode)
            : PixelBuffer(desc->buffer, memType, usage, accMode)
            , mDesc(desc)
        {
            
        }
        
    protected:
        Descriptor_t *mDesc {nullptr};
    };

    /**
     * \brief 一维像素缓冲区（1D 纹理）
     */
    class T3D_ENGINE_API PixelBuffer1D : public PixelBufferT<PixelBuffer1D, PixelBuffer1DDesc>
    {
    public:
        /**
         * \brief 构造 1D 像素缓冲
         * \param [in] desc : 1D 描述
         * \param [in] memType : 内存驻留类型
         * \param [in] usage : 缓冲区用途
         * \param [in] accMode : CPU 访问模式
         */
        PixelBuffer1D(PixelBuffer1DDesc *desc, MemoryType memType, Usage usage, uint32_t accMode);

        /**
         * \brief 返回渲染资源类型标识
         * \return Type::kPixelBuffer1D
         */
        Type getType() const override;
        
    protected:        
        ~PixelBuffer1D() override = default;

        /**
         * \brief 通过活动 RHI 上下文创建 RHIPixelBuffer1D
         * \return 始终返回 true
         */
        bool onLoad() override;

        /**
         * \brief 释放 RHI 1D 像素缓冲引用
         * \return 始终返回 true
         */
        bool onUnload() override;
    };

    template class T3D_ENGINE_API PixelBufferT<PixelBuffer1D, PixelBuffer1DDesc>;

    /**
     * \brief 二维像素缓冲区（2D 纹理或 RenderTexture）
     */
    class T3D_ENGINE_API PixelBuffer2D : public PixelBufferT<PixelBuffer2D, PixelBuffer2DDesc>
    {
    public:
        /**
         * \brief 创建 2D 像素缓冲或 RenderTexture
         * \param [in] desc : 2D 描述
         * \param [in] memType : 内存驻留类型
         * \param [in] usage : 缓冲区用途
         * \param [in] accMode : CPU 访问模式
         * \param [in] isRenderTexture : true 时 onLoad 创建 RenderTexture 而非普通 2D 纹理
         * \return 新建的 PixelBuffer2D 智能指针
         */
        static PixelBuffer2DPtr create(PixelBuffer2DDesc *desc, MemoryType memType, Usage usage, uint32_t accMode, bool isRenderTexture);

        /**
         * \brief 构造 2D 像素缓冲
         * \param [in] desc : 2D 描述
         * \param [in] memType : 内存驻留类型
         * \param [in] usage : 缓冲区用途
         * \param [in] accMode : CPU 访问模式
         * \param [in] isRenderTexture : 是否为 RenderTexture
         */
        PixelBuffer2D(PixelBuffer2DDesc *desc, MemoryType memType, Usage usage, uint32_t accMode, bool isRenderTexture = false);

        /**
         * \brief 返回渲染资源类型标识
         * \return Type::kPixelBuffer2D
         */
        Type getType() const override;
        
    protected:
        ~PixelBuffer2D() override = default;
        
        /**
         * \brief 按 mIsRenderTexture 创建 RenderTexture 或 RHIPixelBuffer2D
         * \return 始终返回 true
         */
        bool onLoad() override;

        /**
         * \brief 释放 RHI 2D 像素缓冲引用
         * \return 始终返回 true
         */
        bool onUnload() override;

    protected:
        /// 为 true 时 onLoad 走 createRenderTexture 路径
        bool    mIsRenderTexture {false};
    };

    template class T3D_ENGINE_API PixelBufferT<PixelBuffer2D, PixelBuffer2DDesc>;

    /**
     * \brief 三维像素缓冲区（3D 纹理）
     */
    class T3D_ENGINE_API PixelBuffer3D : public PixelBufferT<PixelBuffer3D, PixelBuffer3DDesc>
    {
    public:
        /**
         * \brief 构造 3D 像素缓冲
         * \param [in] desc : 3D 描述
         * \param [in] memType : 内存驻留类型
         * \param [in] usage : 缓冲区用途
         * \param [in] accMode : CPU 访问模式
         */
        PixelBuffer3D(PixelBuffer3DDesc *desc, MemoryType memType, Usage usage, uint32_t accMode);

        /**
         * \brief 返回渲染资源类型标识
         * \return Type::kPixelBuffer3D
         */
        Type getType() const override;
        
    protected:
        ~PixelBuffer3D() override = default;

        /**
         * \brief 通过活动 RHI 上下文创建 RHIPixelBuffer3D
         * \return 始终返回 true
         */
        bool onLoad() override;

        /**
         * \brief 释放 RHI 3D 像素缓冲引用
         * \return 始终返回 true
         */
        bool onUnload() override;
    };

    template class T3D_ENGINE_API PixelBufferT<PixelBuffer3D, PixelBuffer3DDesc>;

    /**
     * \brief 立方体贴图像素缓冲区
     * \remarks 复用 PixelBuffer2DDesc，arraySize 固定为 6，buffer 按
     *          +X、-X、+Y、-Y、+Z、-Z 顺序连续存放六个面的像素数据
     */
    class T3D_ENGINE_API PixelBufferCubemap : public PixelBufferT<PixelBufferCubemap, PixelBuffer2DDesc>
    {
    public:
        /// 立方体贴图面数
        static const uint32_t FACE_COUNT = 6;

        /**
         * \brief 构建立方体贴图像素缓冲
         * \param [in] desc : 2D 描述（arraySize 应为 6）
         * \param [in] memType : 内存驻留类型
         * \param [in] usage : 缓冲区用途
         * \param [in] accMode : CPU 访问模式
         */
        PixelBufferCubemap(PixelBuffer2DDesc *desc, MemoryType memType, Usage usage, uint32_t accMode);

        /**
         * \brief 返回渲染资源类型标识
         * \return Type::kPixelBufferCubemap
         */
        Type getType() const override;

    protected:
        ~PixelBufferCubemap() override = default;

        /**
         * \brief 通过活动 RHI 上下文创建 RHIPixelBufferCubemap
         * \return 始终返回 true
         */
        bool onLoad() override;

        /**
         * \brief 释放 RHI 立方体贴图引用
         * \return 始终返回 true
         */
        bool onUnload() override;
    };

    template class T3D_ENGINE_API PixelBufferT<PixelBufferCubemap, PixelBuffer2DDesc>;
}


#endif  /*__T3D_PIXEL_BUFFER_H__*/
