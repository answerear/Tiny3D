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

#ifndef __T3D_CONSTANT_H__
#define __T3D_CONSTANT_H__


#include "T3DPrerequisites.h"


namespace Tiny3D
{
    /**
     * \brief 像素 / 深度模板格式枚举
     */
    TENUM()
    enum class PixelFormat : uint32_t
    {
        E_PF_UNKNOWN = 0,           ///< 未知 / 未指定格式
        E_PF_PALETTE8,              ///< 8 位调色板索引
        E_PF_B5G6R5,                ///< 16 位 B5G6R5
        E_PF_B5G5R5A1,              ///< 16 位 B5G5R5A1
        E_PF_B4R4G4A4,              ///< 16 位 B4G4R4A4
        E_PF_R8G8B8,                ///< 24 位 R8G8B8
        E_PF_B8G8R8,                ///< 24 位 B8G8R8
        E_PF_R8G8B8A8,              ///< 32 位 R8G8B8A8
        E_PF_B8G8R8A8,              ///< 32 位 B8G8R8A8
        E_PF_R8G8B8X8,              ///< 32 位 R8G8B8X8（X 通道忽略）
        E_PF_B8G8R8X8,              ///< 32 位 B8G8R8X8（X 通道忽略）

        /// 24 位无符号归一化深度 + 8 位模板
        E_PF_D24_UNORM_S8_UINT,
        /// 32 位浮点深度 + 8 位模板 + 24 位未用
        E_PF_D32_FLOAT_S8X24_UINT,
        /// 32 位浮点深度，无模板
        E_PF_D32_FLOAT,
        /// 16 位无符号归一化深度，无模板
        E_PF_D16_UNORM,
    };

    /**
     * \brief 纹理维度 / 用途类型
     */
    TENUM()
    enum class TEXTURE_TYPE : uint32_t
    {
        TT_1D = 0,          ///< 一维纹理
        TT_2D,              ///< 二维纹理
        TT_2D_ARRAY,        ///< 二维纹理数组
        TT_3D,              ///< 三维纹理
        TT_CUBE,            ///< 立方体贴图
        TT_CUBE_ARRAY,      ///< 立方体贴图数组
        TT_RENDER_TEXTURE,  ///< 渲染目标纹理
        TT_MAX              ///< 枚举上界（非法取值哨兵）
    };

    /**
     * \brief 渲染图元拓扑类型
     */
    TENUM()
    enum class PrimitiveType : uint32_t
    {
        kPointList = 0,     ///< 点列表
        kLineList,          ///< 线列表
        kLineStrip,         ///< 线带
        kTriangleList,      ///< 三角形列表
        kTriangleStrip,     ///< 三角形带
    };
}


#endif    /*__T3D_CONFIG_H__*/
