/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/


#include "T3DGLES3Renderer.h"
#include "T3DGLES3Context.h"
#include "T3DGLES3Error.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    GLES3RendererPtr GLES3Renderer::create()
    {
        return T3D_NEW GLES3Renderer();
    }

    //--------------------------------------------------------------------------

    GLES3Renderer::GLES3Renderer()
    {
        mName = OPENGLES3;
    }

    //--------------------------------------------------------------------------

    GLES3Renderer::~GLES3Renderer()
    {
        cleanup();
    }

    //--------------------------------------------------------------------------

    TResult GLES3Renderer::init()
    {
        TResult ret = T3D_OK;

        do
        {
            mContext = GLES3Context::create();
            if (mContext == nullptr)
            {
                ret = T3D_ERR_GLES3_CREATE_CONTEXT;
                T3D_LOG_ERROR(LOG_TAG_GLES3RENDERER, "Failed to create GLES3 context !");
                break;
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult GLES3Renderer::destroy()
    {
        TResult ret = T3D_OK;
        return ret;
    }

    //--------------------------------------------------------------------------

    void GLES3Renderer::getEditorInfo(void *info, RenderWindow *window)
    {
    }

    //--------------------------------------------------------------------------

    void GLES3Renderer::cleanup()
    {
        mContext = nullptr;
    }

    //--------------------------------------------------------------------------
}
