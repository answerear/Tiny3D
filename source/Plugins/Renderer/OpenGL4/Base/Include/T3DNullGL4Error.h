/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/


#ifndef __T3D_NULLGL4_ERROR_H__
#define __T3D_NULLGL4_ERROR_H__


#include "T3DNullGL4Prerequisites.h"


namespace Tiny3D
{
    enum NullGL4ErrorCode : int32_t
    {
        T3D_ERR_NULLGL4_CREATE_CONTEXT = T3D_ERR_RENDERER,
        T3D_ERR_NULLGL4_SET_PIXEL_FORMAT,
        T3D_ERR_NULLGL4_GLAD_LOAD,
        T3D_ERR_NULLGL4_CREATE_SHADER,
        T3D_ERR_NULLGL4_COMPILE_SHADER,
        T3D_ERR_NULLGL4_LINK_PROGRAM,
        T3D_ERR_NULLGL4_SHADER_REFLECTION,
    };
}


#endif  /*__T3D_NULLGL4_ERROR_H__*/
