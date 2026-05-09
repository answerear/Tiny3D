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


#include "T3DGL4Renderer.h"
#include "T3DGL4Context.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    GL4RendererPtr GL4Renderer::create()
    {
        return T3D_NEW GL4Renderer();
    }

    //--------------------------------------------------------------------------

    GL4Renderer::GL4Renderer()
    {
        mName = OPENGL4;
    }

    //--------------------------------------------------------------------------

    GL4Renderer::~GL4Renderer()
    {
        cleanup();
    }

    //--------------------------------------------------------------------------

    TResult GL4Renderer::init()
    {
        TResult ret = T3D_OK;

        do
        {
            mContext = GL4Context::create();
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult GL4Renderer::destroy()
    {
        TResult ret = T3D_OK;

        do
        {
            // Do NOT cleanup() here. Context destruction is deferred to
            // ~GL4Renderer() when mRenderers map is cleared.
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    void GL4Renderer::getEditorInfo(void *info, RenderWindow *window)
    {
        // TODO: 填充编辑器需要的 GL 上下文信息
    }

    //--------------------------------------------------------------------------

    void GL4Renderer::cleanup()
    {
        mContext = nullptr;
    }

    //--------------------------------------------------------------------------
}
