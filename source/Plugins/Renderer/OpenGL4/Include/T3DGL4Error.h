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


#ifndef __T3D_GL4_ERROR_H__
#define __T3D_GL4_ERROR_H__


#include "T3DGL4Prerequisites.h"


namespace Tiny3D
{
    enum GL4ErrorCode : int32_t
    {
        /// 窗口已经创建
        T3D_ERR_GL4_WINDOW_ALREADY = T3D_ERR_RENDERER,
        /// 错误的渲染目标
        T3D_ERR_GL4_INVALID_TARGET,
        /// 创建 GL 上下文失败
        T3D_ERR_GL4_CREATE_CONTEXT,
        /// 设置像素格式失败
        T3D_ERR_GL4_SET_PIXEL_FORMAT,
        /// GLAD 加载失败
        T3D_ERR_GL4_GLAD_LOAD,
        /// 创建 Shader 失败
        T3D_ERR_GL4_CREATE_SHADER,
        /// Shader 编译失败
        T3D_ERR_GL4_COMPILE_SHADER,
        /// Shader 链接失败
        T3D_ERR_GL4_LINK_PROGRAM,
        /// Shader 反射失败
        T3D_ERR_GL4_SHADER_REFLECTION,
        /// 创建缓冲区失败
        T3D_ERR_GL4_CREATE_BUFFER,
        /// 创建纹理失败
        T3D_ERR_GL4_CREATE_TEXTURE,
        /// 创建 FBO 失败
        T3D_ERR_GL4_CREATE_FBO,
        /// 创建 VAO 失败
        T3D_ERR_GL4_CREATE_VAO,
        /// 创建 Sampler 失败
        T3D_ERR_GL4_CREATE_SAMPLER,
        /// 创建 BlendState 失败
        T3D_ERR_GL4_CREATE_BLEND_STATE,
        /// 创建 DepthStencilState 失败
        T3D_ERR_GL4_CREATE_DEPTH_STENCIL_STATE,
        /// 创建 RasterizerState 失败
        T3D_ERR_GL4_CREATE_RASTERIZER_STATE,
        /// 映射缓冲区失败
        T3D_ERR_GL4_MAP_BUFFER,
        /// SwapBuffers 失败
        T3D_ERR_GL4_SWAP_BUFFERS,
        /// 不支持的操作
        T3D_ERR_GL4_NOT_SUPPORTED,
        /// 调整窗口大小失败
        T3D_ERR_GL4_RESIZE_BUFFERS,
        /// Present 失败
        T3D_ERR_GL4_PRESENT,
        /// 无效的用途
        T3D_ERR_GL4_INVALID_USAGE,
    };
}


#endif  /*__T3D_GL4_ERROR_H__*/
