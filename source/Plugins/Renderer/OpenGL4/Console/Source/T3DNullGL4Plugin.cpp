/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/


#include "T3DNullGL4Plugin.h"
#include "T3DNullGL4Renderer.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    NullGL4Plugin::NullGL4Plugin()
        : mName("NullGL4Renderer")
        , mRenderer(nullptr)
    {
    }

    //--------------------------------------------------------------------------

    NullGL4Plugin::~NullGL4Plugin()
    {
    }

    //--------------------------------------------------------------------------

    const String &NullGL4Plugin::getName() const
    {
        return mName;
    }

    //--------------------------------------------------------------------------

    TResult NullGL4Plugin::install()
    {
        TResult ret = T3D_OK;

        do
        {
            mRenderer = NullGL4Renderer::create();
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

    TResult NullGL4Plugin::startup()
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult NullGL4Plugin::shutdown()
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult NullGL4Plugin::uninstall()
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
