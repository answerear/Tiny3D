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


#include "T3DD3D11RenderWindow.h"
#include "T3DD3D11Error.h"
#include "T3DD3D11Renderer.h"
#include "T3DD3D11Context.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    D3D11RenderWindowPtr D3D11RenderWindow::create(RenderWindow *renderWindow)
    {
        // SwapChain / BackBuffer / RTView / DSView 的创建都在
        // D3D11Context::createRenderWindow 里完成，这里只负责产出空壳对象
        (void)renderWindow;
        D3D11RenderWindowPtr window = T3D_NEW D3D11RenderWindow();
        return window;
    }

    //--------------------------------------------------------------------------

    D3D11RenderWindow::D3D11RenderWindow()
        : D3DSwapChain(nullptr)
        , D3DRTView(nullptr)
        , D3DDSView(nullptr)
    {
    }

    //--------------------------------------------------------------------------

    D3D11RenderWindow::~D3D11RenderWindow()
    {
        D3D_SAFE_RELEASE(D3DDSView);
        D3D_SAFE_RELEASE(D3DDSBuffer);
        D3D_SAFE_RELEASE(D3DRTView);
        D3D_SAFE_RELEASE(D3DBackBuffer);
        D3D_SAFE_RELEASE(D3DSwapChain);
    }

    //--------------------------------------------------------------------------

    TResult D3D11RenderWindow::swapBuffers()
    {
        return D3D11_CONTEXT->swapBackBuffer(this);
    }

    //--------------------------------------------------------------------------

    TResult D3D11RenderWindow::resize(uint32_t w, uint32_t h)
    {
        return D3D11_CONTEXT->resizeRenderWindow(this, w, h);
    }

    //--------------------------------------------------------------------------

    void *D3D11RenderWindow::getNativeObject() const
    {
        return D3DSwapChain;
    }

    //--------------------------------------------------------------------------
}


