/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/


#include "T3DVKConsolePlugin.h"
#include "T3DVKConsoleRenderer.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    VKConsolePlugin::VKConsolePlugin()
        : mName("VKRendererConsole")
        , mRenderer(nullptr)
    {
    }

    //--------------------------------------------------------------------------

    VKConsolePlugin::~VKConsolePlugin()
    {
    }

    //--------------------------------------------------------------------------

    const String &VKConsolePlugin::getName() const
    {
        return mName;
    }

    //--------------------------------------------------------------------------

    TResult VKConsolePlugin::install()
    {
        TResult ret = T3D_OK;

        do
        {
            mRenderer = VKRendererConsole::create();
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

    TResult VKConsolePlugin::startup()
    {
        TResult ret = T3D_OK;

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult VKConsolePlugin::shutdown()
    {
        TResult ret = T3D_OK;

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult VKConsolePlugin::uninstall()
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

    //--------------------------------------------------------------------------
}
