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


#ifndef __T3D_RENDER_BUFFER_DESC_H__
#define __T3D_RENDER_BUFFER_DESC_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"
#include "Kernel/T3DConstant.h"
#include "Render/T3DRenderConstant.h"
#include "T3DMultisamplerDesc.h"


namespace Tiny3D
{
    /**
     * \brief 顶点缓冲区创建描述
     */
    TSTRUCT()
    struct T3D_ENGINE_API VertexBufferDesc
    {
        /// 单个顶点的字节大小
        TPROPERTY()
        uint32_t  verteSize {0};
        /// 顶点个数
        TPROPERTY()
        uint32_t  vertexCount {0};
        /// 初始 CPU 顶点数据
        TPROPERTY()
        Buffer  buffer {};
    };

    /**
     * \brief 索引缓冲区创建描述
     */
    TSTRUCT()
    struct T3D_ENGINE_API IndexBufferDesc
    {
        /// 单个索引的字节大小
        TPROPERTY()
        uint32_t  indexSize {0};
        /// 索引个数
        TPROPERTY()
        uint32_t  indexCount {0};
        /// 初始 CPU 索引数据
        TPROPERTY()
        Buffer  buffer {};
    };


    /**
     * \brief 一维像素缓冲区（1D 纹理）创建描述
     */
    TSTRUCT()
    struct T3D_ENGINE_API PixelBuffer1DDesc
    {
        /// 纹理宽度（像素）
        TPROPERTY()
        uint32_t    width {0};
        /// mipmap 层数
        TPROPERTY()
        uint32_t    mipmaps {0};
        /// 数组层数
        TPROPERTY()
        uint32_t    arraySize {0};
        /// 像素格式
        TPROPERTY()
        PixelFormat format {PixelFormat::E_PF_UNKNOWN};
        /// 初始 CPU 像素数据
        TPROPERTY()
        Buffer  buffer {};
        /// 是否允许 shader 采样读取
        TPROPERTY()
        bool    shaderReadable {false};
    };

    /**
     * \brief 二维像素缓冲区（2D 纹理 / RenderTexture）创建描述
     */
    TSTRUCT()
    struct T3D_ENGINE_API PixelBuffer2DDesc
    {
        /// 纹理宽度（像素）
        TPROPERTY()
        uint32_t    width {0};
        /// 纹理高度（像素）
        TPROPERTY()
        uint32_t    height {0};
        /// mipmap 层数
        TPROPERTY()
        uint32_t    mipmaps {1};
        /// 数组层数（立方体贴图复用时固定为 6）
        TPROPERTY()
        uint32_t    arraySize {1};
        /// 像素格式
        TPROPERTY()
        PixelFormat format {PixelFormat::E_PF_B8G8R8A8};
        /// MSAA 多重采样配置
        TPROPERTY()
        MultisamplerDesc    sampleDesc {};
        /// 初始 CPU 像素数据
        TPROPERTY()
        Buffer  buffer {};
        /// 是否允许 shader 采样读取
        TPROPERTY()
        bool    shaderReadable {false};
    };

    /**
     * \brief 三维像素缓冲区（3D 纹理）创建描述
     */
    TSTRUCT()
    struct T3D_ENGINE_API PixelBuffer3DDesc
    {
        /// 纹理宽度（像素）
        uint32_t    width {0};
        /// 纹理高度（像素）
        TPROPERTY()
        uint32_t    height {0};
        /// 纹理深度（像素）
        TPROPERTY()
        uint32_t    depth {0};
        /// mipmap 层数
        TPROPERTY()
        uint32_t    mipmaps {1};
        /// 像素格式
        TPROPERTY()
        PixelFormat format {PixelFormat::E_PF_UNKNOWN};
        /// 初始 CPU 像素数据
        TPROPERTY()
        Buffer  buffer {};
        /// 是否允许 shader 采样读取
        TPROPERTY()
        bool    shaderReadable {false};
    };
}


#endif  /*__T3D_RENDER_BUFFER_DESC_H__*/
