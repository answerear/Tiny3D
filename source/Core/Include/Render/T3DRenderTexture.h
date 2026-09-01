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


#ifndef __T3D_RENDER_TEXTURE_H__
#define __T3D_RENDER_TEXTURE_H__


#include "Resource/T3DTexture.h"


namespace Tiny3D
{
    /**
     * \brief 可渲染的 2D 纹理资源，onCreate/onLoad 时通过 RenderBufferManager 创建 GPU 渲染纹理
     */
    class T3D_ENGINE_API RenderTexture : public Texture2D
    {
    public:
        /**
         * \brief 创建空白渲染纹理
         * \param [in] name : 纹理名称
         * \param [in] width : 宽度
         * \param [in] height : 高度
         * \param [in] format : 像素格式
         * \param [in] mipmaps : Mipmap 级数
         * \param [in] MSAACount : MSAA 采样数
         * \param [in] MSAAQuality : MSAA 质量
         * \param [in] shaderReadable : 是否允许着色器采样读取
         * \param [in] accMode : CPU 访问许可，传 kCPURead 才允许 map 读回
         * \return 新创建的 RenderTexture 指针
         */
        static RenderTexturePtr create(const String &name, uint32_t width, uint32_t height, PixelFormat format, uint32_t mipmaps, uint32_t MSAACount, uint32_t MSAAQuality, bool shaderReadable, uint32_t accMode = kCPUNone);

        /**
         * \brief 创建带初始数据的渲染纹理
         * \param [in] name : 纹理名称
         * \param [in] width : 宽度
         * \param [in] height : 高度
         * \param [in] format : 像素格式
         * \param [in] mipmaps : Mipmap 级数
         * \param [in] MSAACount : MSAA 采样数
         * \param [in] MSAAQuality : MSAA 质量
         * \param [in] data : 初始像素数据
         * \param [in] shaderReadable : 是否允许着色器采样读取
         * \return 新创建的 RenderTexture 指针
         */
        static RenderTexturePtr create(const String &name, uint32_t width, uint32_t height, PixelFormat format, uint32_t mipmaps, uint32_t MSAACount, uint32_t MSAAQuality, const Buffer &data, bool shaderReadable);

        /**
         * \brief 返回纹理类型标识
         * \return TEXTURE_TYPE::TT_RENDER_TEXTURE
         */
        TEXTURE_TYPE getTextureType() const override;

        /**
         * \brief 改变渲染纹理尺寸，先更新描述再让后端按新描述重建 GPU 资源
         * \param [in] width : 新宽度，不能为 0
         * \param [in] height : 新高度，不能为 0
         * \return 调用成功返回 T3D_OK；尺寸非法返回 T3D_ERR_INVALID_PARAM
         * \note 尺寸未变化时直接返回 T3D_OK，不触发重建
         */
        TResult resize(uint32_t width, uint32_t height);

    protected:
        /**
         * \brief 构造空白渲染纹理
         * \param [in] name : 纹理名称
         * \param [in] width : 宽度
         * \param [in] height : 高度
         * \param [in] format : 像素格式
         * \param [in] mipmaps : Mipmap 级数
         * \param [in] MSAACount : MSAA 采样数
         * \param [in] MSAAQuality : MSAA 质量
         * \param [in] shaderReadable : 是否允许着色器采样读取
         */
        RenderTexture(const String &name, uint32_t width, uint32_t height, PixelFormat format, uint32_t mipmaps, uint32_t MSAACount, uint32_t MSAAQuality, bool shaderReadable, uint32_t accMode = kCPUNone);

        /**
         * \brief 构造带初始数据的渲染纹理
         * \param [in] name : 纹理名称
         * \param [in] width : 宽度
         * \param [in] height : 高度
         * \param [in] format : 像素格式
         * \param [in] mipmaps : Mipmap 级数
         * \param [in] MSAACount : MSAA 采样数
         * \param [in] MSAAQuality : MSAA 质量
         * \param [in] data : 初始像素数据
         * \param [in] shaderReadable : 是否允许着色器采样读取
         */
        RenderTexture(const String &name, uint32_t width, uint32_t height, PixelFormat format, uint32_t mipmaps, uint32_t MSAACount, uint32_t MSAAQuality, const Buffer &data, bool shaderReadable);

        ~RenderTexture() override;

        /**
         * \brief 克隆渲染纹理（复制属性后返回新实例）
         * \return 新 RenderTexture 资源指针
         */
        ResourcePtr clone() const override;

        /**
         * \brief 从源 RenderTexture 复制属性（当前实现为空）
         * \param [in] src : 源资源
         */
        void cloneProperties(const Resource * const src) override;

        /**
         * \brief 通过 RenderBufferManager 创建 VRAM 渲染纹理像素缓冲
         * \return 调用成功返回 T3D_OK
         */
        TResult onCreate() override;

        /**
         * \brief 从 Archive 加载时创建 VRAM 渲染纹理像素缓冲
         * \param [in] archive : 归档对象（当前未使用）
         * \return 调用成功返回 T3D_OK
         */
        TResult onLoad(Archive *archive) override;
    };
}




#endif  /*__T3D_RENDER_TEXTURE_H__*/
