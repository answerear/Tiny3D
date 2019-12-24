/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2019  Answer Wong
 * For latest info, see https://github.com/answerear/Tiny3D
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/


#ifndef __T3D_RENDERER_CAPABILITIES_H__
#define __T3D_RENDERER_CAPABILITIES_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"
#include "T3DErrorDef.h"


namespace Tiny3D
{

    enum Capabilities : uint32_t
    {
        PRIMITIVE_RESTART,          /**< 支持通过在索引缓冲中指定 -1 来启动新的渲染命令，在 Geometry Shader 中使用 */
        AUTOMIPMAP_COMPRESSED,      /**< 支持硬件生成压缩格式的 mipmap */
        ANISOTROPY,                 /**< 支持各向异性纹理过滤 */
        WIDE_LINES,                 /**< 支持设置线条宽度 */
        INDEX_32BITS,               /**< 支持32位索引缓冲 */
        ATOMIC_COUNTERS,            /**< 支持 Atomic counters 缓冲 */

        TWO_SIDED_STENCIL,          /**< 支持前后两面模板缓冲独立更新方式 */
        WRAP_STENCIL,               /**< 支持 wrap 模板缓冲值 */
        HWSTENCIL,                  /**< 支持硬件模板缓冲 */

        HWOCCLUSION,                /**< 支持硬件遮挡查询 */
        HWRENDER_TO_VERTEX_BUFFER,  /**< 支持渲染到顶点缓冲 */
        HWRENDER_TO_TEXTURE,        /**< 支持硬件渲染到纹理 */
        TEXTURE_FLOAT,              /**< 支持浮点纹理和渲染目标 */
        NON_POWER_OF_2_TEXTURES,    /**< 支持非2幂尺寸的纹理 */
        TEXTURE_3D,                 /**< 支持体积纹理 */
        
        USER_CLIP_PLANES,           /**< 支持用户定义裁剪平面 */
        VERTEX_FORMAT_UBYTE4,       /**< 支持顶点使用 UBYTE4 格式 */
        
        INFINITE_FAR_PLANE,         /**< 支持无穷远平面 */
        POINT_SPRITES,              /**< 支持点精灵渲染 */
        POINT_EXTENDED_PARAMETERS,  /**< 支持点精灵需要的参数 */
        VERTEX_TEXTURE_FETCH,       /**< 支持顶点纹理拾取 */
        MIPMAP_LOD_BIAS,            /**< 支持 mipmap LOD 偏差 */
        MRT_DIFFERENT_BIT_DEPTHS,   /**< 支持多渲染目标有不同的深度值 */

        VERTEX_SHADER,              /**< 支持顶点着色器 */
        PIXEL_SHADER,               /**< 支持像素着色器 */
        GEOMETRY_SHADER,            /**< 支持几何着色器 */

        TEXTURE_COMPRESSION,        /**< 支持纹理压缩 */
        TEXTURE_COMPRESSION_DXT,    /**< 支持 DXT (MS DirectX) 纹理压缩 */
        TEXTURE_COMPRESSION_BC4_BC5,/**< BC4 和 BC5 格式 (DirectX feature level 10_0) */
        TEXTURE_COMPRESSION_BC6_BC7,/**< BC6H 和 BC7 格式 (DirectX feature level 11_0) */
        TEXTURE_COMPRESSION_VTC,    /**< 支持 VTC (体积纹理, OpenGL) 压缩格式 */
        TEXTURE_COMPRESSION_PVRTC,  /**< 支持 PVRTC (PowerVR iOS/Android) 压缩格式 */
        TEXTURE_COMPRESSION_ASTC,   /**< 支持 ASTC (iOS) 压缩格式 */
        TEXTURE_COMPRESSION_ATC,    /**< 支持 ATC (QUALCOMM AMD Adreno) 压缩格式 */
        TEXTURE_COMPRESSION_ETC1,   /**< 支持 ETC1 (Android) 压缩格式 */
        TEXTURE_COMPRESSION_ETC2,   /**< 支持 ETC2 (Android) 压缩格式 */
        
    };

    class T3D_ENGINE_API RendererCapabilities : public Object
    {
    public:
        virtual ~RendererCapabilities();


    };
}


#endif  /*__T3D_RENDERER_CAPABILITIES_H__*/
