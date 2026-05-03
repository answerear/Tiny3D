/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 ******************************************************************************/


#include "T3DGL4ConsolePlugin.h"
#include "T3DGL4ConsoleRenderer.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    GL4ConsolePlugin::GL4ConsolePlugin()
        : mName("GL4RendererConsole")
        , mRenderer(nullptr)
    {
    }

    //--------------------------------------------------------------------------

    GL4ConsolePlugin::~GL4ConsolePlugin()
    {
    }

    //--------------------------------------------------------------------------

    const String &GL4ConsolePlugin::getName() const
    {
        return mName;
    }

    //--------------------------------------------------------------------------

    TResult GL4ConsolePlugin::install()
    {
        TResult ret = T3D_OK;

        do
        {
            mRenderer = GL4RendererConsole::create();
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

    TResult GL4ConsolePlugin::startup()
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult GL4ConsolePlugin::shutdown()
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult GL4ConsolePlugin::uninstall()
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
