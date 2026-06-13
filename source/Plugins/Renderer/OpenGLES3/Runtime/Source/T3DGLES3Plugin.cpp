/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/


#include "T3DGLES3Plugin.h"
#include "T3DGLES3Renderer.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    GLES3Plugin::GLES3Plugin()
        : mName("GLES3Renderer")
        , mRenderer(nullptr)
    {

    }

    //--------------------------------------------------------------------------

    GLES3Plugin::~GLES3Plugin()
    {

    }

    //--------------------------------------------------------------------------

    const String &GLES3Plugin::getName() const
    {
        return mName;
    }

    //--------------------------------------------------------------------------

    TResult GLES3Plugin::install()
    {
        TResult ret = T3D_OK;

        do
        {
            mRenderer = GLES3Renderer::create();
            if (mRenderer != nullptr)
            {
                ret = T3D_AGENT.addRHIRenderer(mRenderer);
                if (T3D_FAILED(ret))
                {
                    break;
                }
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult GLES3Plugin::startup()
    {
        TResult ret = T3D_OK;
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult GLES3Plugin::shutdown()
    {
        TResult ret = T3D_OK;
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult GLES3Plugin::uninstall()
    {
        TResult ret = T3D_OK;

        do
        {
            ret = T3D_AGENT.removeRHIRenderer(mRenderer);
            if (T3D_FAILED(ret))
            {
                break;
            }

            mRenderer = nullptr;
        } while (0);

        return ret;
    }
}
