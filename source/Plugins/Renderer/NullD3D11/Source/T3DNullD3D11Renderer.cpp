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


#include "T3DNullD3D11Renderer.h"
#include "T3DNullD3D11Context.h"
#include "T3DNullD3D11Window.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    NullD3D11RendererPtr NullD3D11Renderer::create()
    {
        return new NullD3D11Renderer();
    }
    
    //--------------------------------------------------------------------------

    NullD3D11Renderer::NullD3D11Renderer()
    {
        mName = NULL_DIRECT3D11;
    }
    
    //--------------------------------------------------------------------------

    NullD3D11Renderer::~NullD3D11Renderer()
    {
        cleanup();
    }
    
    //--------------------------------------------------------------------------

    TResult NullD3D11Renderer::init()
    {
        TResult ret = T3D_OK;

        do 
        {
            mContext = NullD3D11Context::create();
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult NullD3D11Renderer::destroy()
    {
        TResult ret = T3D_OK;

        do 
        {
            cleanup();
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    void NullD3D11Renderer::getEditorInfo(void *info, RenderWindow *window)
    {
        
    }

    //--------------------------------------------------------------------------

    void NullD3D11Renderer::cleanup()
    {
        mContext = nullptr;
    }

    //--------------------------------------------------------------------------
}


