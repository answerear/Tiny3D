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


#include "T3DMetalRenderer.h"
#include "T3DMetalContext.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    MetalRendererPtr MetalRenderer::create()
    {
        return T3D_NEW MetalRenderer();
    }

    //--------------------------------------------------------------------------

    MetalRenderer::MetalRenderer()
    {
        mName = METAL;
    }

    //--------------------------------------------------------------------------

    MetalRenderer::~MetalRenderer()
    {
        cleanup();
    }

    //--------------------------------------------------------------------------

    TResult MetalRenderer::init()
    {
        TResult ret = T3D_OK;

        do
        {
            mContext = MetalContext::create();
            if (mContext == nullptr)
            {
                ret = T3D_ERR_INVALID_POINTER;
                T3D_LOG_ERROR(LOG_TAG_METALRENDERER, "Failed to create Metal context !");
                break;
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult MetalRenderer::destroy()
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    void MetalRenderer::getEditorInfo(void *info, RenderWindow *window)
    {
        (void)info;
        (void)window;
    }

    //--------------------------------------------------------------------------

    void MetalRenderer::cleanup()
    {
        mContext = nullptr;
    }

    //--------------------------------------------------------------------------
}
