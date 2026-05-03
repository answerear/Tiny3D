/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/


#include "T3DGL4ConsoleRenderer.h"
#include "T3DGL4ConsoleContext.h"
#include "T3DGL4ConsoleWindow.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    GL4RendererConsolePtr GL4RendererConsole::create()
    {
        return T3D_NEW GL4RendererConsole();
    }

    //--------------------------------------------------------------------------

    GL4RendererConsole::GL4RendererConsole()
    {
        mName = NULL_OPENGL4;
    }

    //--------------------------------------------------------------------------

    GL4RendererConsole::~GL4RendererConsole()
    {
        cleanup();
    }

    //--------------------------------------------------------------------------

    TResult GL4RendererConsole::init()
    {
        TResult ret = T3D_OK;

        do
        {
            GL4ConsoleContextPtr ctx = GL4ConsoleContext::create();
            ret = ctx->init();
            if (T3D_FAILED(ret))
            {
                break;
            }
            mContext = ctx;
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult GL4RendererConsole::destroy()
    {
        cleanup();
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    void GL4RendererConsole::getEditorInfo(void *info, RenderWindow *window)
    {
    }

    //--------------------------------------------------------------------------

    void GL4RendererConsole::cleanup()
    {
        mContext = nullptr;
    }

    //--------------------------------------------------------------------------
}
