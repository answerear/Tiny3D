/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/


#include "T3DNullGL4Renderer.h"
#include "T3DNullGL4Context.h"
#include "T3DNullGL4Window.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    NullGL4RendererPtr NullGL4Renderer::create()
    {
        return T3D_NEW NullGL4Renderer();
    }

    //--------------------------------------------------------------------------

    NullGL4Renderer::NullGL4Renderer()
    {
        mName = NULL_OPENGL4;
    }

    //--------------------------------------------------------------------------

    NullGL4Renderer::~NullGL4Renderer()
    {
        cleanup();
    }

    //--------------------------------------------------------------------------

    TResult NullGL4Renderer::init()
    {
        TResult ret = T3D_OK;

        do
        {
            NullGL4ContextPtr ctx = NullGL4Context::create();
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

    TResult NullGL4Renderer::destroy()
    {
        cleanup();
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    void NullGL4Renderer::getEditorInfo(void *info, RenderWindow *window)
    {
    }

    //--------------------------------------------------------------------------

    void NullGL4Renderer::cleanup()
    {
        mContext = nullptr;
    }

    //--------------------------------------------------------------------------
}
