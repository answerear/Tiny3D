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


#include "T3DNullD3D11Window.h"
#include "T3DNullD3D11Error.h"
#include "T3DNullD3D11Renderer.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    NullD3D11WindowPtr NullD3D11Window::create()
    {
        NullD3D11WindowPtr window = T3D_NEW NullD3D11Window();
        return window;
    }

    //--------------------------------------------------------------------------

    NullD3D11Window::NullD3D11Window()
    {

    }

    //--------------------------------------------------------------------------

    NullD3D11Window::~NullD3D11Window()
    {
    }

    //--------------------------------------------------------------------------

    TResult NullD3D11Window::swapBuffers()
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult NullD3D11Window::clear(const ColorRGB &clrFill, uint32_t clearFlags, 
        Real depth, uint32_t stencil)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult NullD3D11Window::resize(uint32_t w, uint32_t h)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    void *NullD3D11Window::getNativeObject() const
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
}


