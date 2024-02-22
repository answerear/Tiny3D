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


#include "T3DNullPlugin.h"
#include "T3DNullRenderer.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    NullPlugin::NullPlugin()
        : mName("NullRenderer")
        , mRenderer(nullptr)
    {

    }

    //--------------------------------------------------------------------------

    NullPlugin::~NullPlugin()
    {

    }

    //--------------------------------------------------------------------------

    const String &NullPlugin::getName() const
    {
        return mName;
    }

    //--------------------------------------------------------------------------

    TResult NullPlugin::install()
    {
        TResult ret = T3D_OK;

        do
        {
            mRenderer = NullRenderer::create();
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

    TResult NullPlugin::startup()
    {
        TResult ret = T3D_OK;

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult NullPlugin::shutdown()
    {
        TResult ret = T3D_OK;

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult NullPlugin::uninstall()
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
