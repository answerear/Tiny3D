/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/


#ifndef __T3D_GLES3_ERROR_H__
#define __T3D_GLES3_ERROR_H__


#include <Tiny3D.h>


namespace Tiny3D
{
    enum GLES3ErrorCode : int32_t
    {
        T3D_ERR_GLES3_WINDOW_ALREADY = T3D_ERR_RENDERER,
        T3D_ERR_GLES3_INVALID_TARGET,
        T3D_ERR_GLES3_CREATE_CONTEXT,
        T3D_ERR_GLES3_EGL_INIT,
        T3D_ERR_GLES3_EGL_CONFIG,
        T3D_ERR_GLES3_CREATE_SURFACE,
        T3D_ERR_GLES3_CREATE_SHADER,
        T3D_ERR_GLES3_COMPILE_SHADER,
        T3D_ERR_GLES3_LINK_PROGRAM,
        T3D_ERR_GLES3_SHADER_REFLECTION,
        T3D_ERR_GLES3_CREATE_BUFFER,
        T3D_ERR_GLES3_CREATE_TEXTURE,
        T3D_ERR_GLES3_CREATE_FBO,
        T3D_ERR_GLES3_CREATE_VAO,
        T3D_ERR_GLES3_CREATE_SAMPLER,
        T3D_ERR_GLES3_CREATE_BLEND_STATE,
        T3D_ERR_GLES3_CREATE_DEPTH_STENCIL_STATE,
        T3D_ERR_GLES3_CREATE_RASTERIZER_STATE,
        T3D_ERR_GLES3_MAP_BUFFER,
        T3D_ERR_GLES3_SWAP_BUFFERS,
        T3D_ERR_GLES3_NOT_SUPPORTED,
        T3D_ERR_GLES3_RESIZE_BUFFERS,
        T3D_ERR_GLES3_INVALID_USAGE,
        T3D_ERR_GLES3_PRESENT,
    };
}


#endif  /*__T3D_GLES3_ERROR_H__*/
