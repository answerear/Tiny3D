/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/


#ifndef __T3D_GL4CONSOLE_ERROR_H__
#define __T3D_GL4CONSOLE_ERROR_H__


#include "T3DGL4ConsolePrerequisites.h"


namespace Tiny3D
{
    enum GL4ConsoleErrorCode : int32_t
    {
        T3D_ERR_GL4CONSOLE_CREATE_CONTEXT = T3D_ERR_RENDERER,
        T3D_ERR_GL4CONSOLE_SET_PIXEL_FORMAT,
        T3D_ERR_GL4CONSOLE_GLAD_LOAD,
        T3D_ERR_GL4CONSOLE_CREATE_SHADER,
        T3D_ERR_GL4CONSOLE_COMPILE_SHADER,
        T3D_ERR_GL4CONSOLE_LINK_PROGRAM,
        T3D_ERR_GL4CONSOLE_SHADER_REFLECTION,
    };
}


#endif  /*__T3D_GL4CONSOLE_ERROR_H__*/
