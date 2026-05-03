/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/


#ifndef __T3D_GL4_ERROR_H__
#define __T3D_GL4_ERROR_H__


#include <Tiny3D.h>


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
