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


#ifndef __T3D_RHI_RESOURCE_H__
#define __T3D_RHI_RESOURCE_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"


namespace Tiny3D
{
    /**
     * \brief RHI 资源的抽象基类，提供资源类型标识与后端原生句柄访问
     */
    class T3D_ENGINE_API RHIResource : public Object
    {
    public:
        /// 默认构造
        RHIResource() = default;

        /// 默认析构
        ~RHIResource() override = default;

        /**
         * \brief RHI 资源类型枚举
         */
        enum class ResourceType : uint32_t
        {
            kNone = 0,              ///< 无类型
            kBlendState,            ///< 颜色混合状态
            kDepthStencilState,     ///< 深度/模板状态
            kRasterizerState,       ///< 光栅化状态
            kSamplerState,          ///< 纹理采样状态
            kVertexBuffer,          ///< 顶点缓冲
            kIndexBuffer,           ///< 索引缓冲
            kPixelBuffer1D,         ///< 一维像素缓冲（纹理）
            kPixelBuffer2D,         ///< 二维像素缓冲（纹理）
            kPixelBuffer3D,         ///< 三维像素缓冲（纹理）
            kPixelBufferCubemap,    ///< 立方体贴图像素缓冲
            kConstantBuffer,        ///< 常量缓冲
            kVertexDeclaration,     ///< 顶点声明/输入布局
            kRenderWindow,          ///< 渲染窗口（交换链）
            kRenderTexture,         ///< 渲染纹理
            kShader,                ///< 着色器
            // 新增类型一律追加在末尾，避免既有日志里已出现过的数值含义漂移
            kStructuredBuffer,      ///< 结构化缓冲（Structured / ByteAddress / Typed）
        };

        /**
         * \brief 获取本 RHI 对象的资源类型
         * \return 对应的 ResourceType 枚举值
         */
        virtual ResourceType getResourceType() const = 0;

        /**
         * \brief 获取后端原生 GPU 对象指针
         * \return 具体后端返回对应 API 句柄（如 ID3D11Buffer*、VkBuffer 等）；由派生类实现
         */
        virtual void *getNativeObject() const = 0;
    };
}


#endif  /*__T3D_RHI_RESOURCE_H__*/
