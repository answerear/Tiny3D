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
        D3D11RenderWindowPtr window = new D3D11RenderWindow();
        if (window && !window->init(renderWindow))
        {
            window = nullptr;
        }
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

    bool D3D11RenderWindow::init(RenderWindow *renderWindow)
    {
        bool ret = true;

        do
        {
            // TResult result;
            // UINT uMSAAQuality = param.MSAA;
            // UINT uMSAACount = 4;
            //
            // if (uMSAAQuality < 2)
            //     uMSAAQuality = 0;
            // else if (uMSAAQuality > 16)
            //     uMSAAQuality = 16;
            //
            // DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
            //
            // result = D3D11_CONTEXT->checkMultiSampleQuality((UINT)renderWindow->getWidth(), (UINT)renderWindow->getHeight(), uMSAAQuality, uMSAACount, format);
            // if (T3D_FAILED(result))
            // {
            //     ret = false;
            //     break;
            // }
            //
            // SysWMInfo info;
            // renderWindow->getSystemInfo(info);
            // result = D3D11_CONTEXT->createSwapChain((HWND)info.hWnd, (UINT)renderWindow->getWidth(), (UINT)renderWindow->getHeight(), format, renderWindow->isFullscreen(), &D3DSwapChain);
            // if (T3D_FAILED(result))
            // {
            //     ret = false;
            //     break;
            // }
            //
            // result = D3D11_CONTEXT->createRenderWindowView(&D3DSwapChain, &D3DRTView);
            // if (T3D_FAILED(result))
            // {
            //     ret = false;
            //     break;
            // }
            //
            // result = D3D11_CONTEXT->createDepthStencilView((UINT)renderWindow->getWidth(), (UINT)renderWindow->getHeight(), &D3DDSView);
            // if (T3D_FAILED(result))
            // {
            //     ret = false;
            //     break;
            // }
            
            // result = D3D11_CONTEXT->setRenderTargets(1, &mD3DRTView, &mD3DDSView);
            // if (T3D_FAILED(result))
            // {
            //     ret = false;
            //     break;
            // }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult D3D11RenderWindow::swapBuffers()
    {
        return  D3D11_CONTEXT->swapBackBuffer(this);
    }

    //--------------------------------------------------------------------------

    TResult D3D11RenderWindow::clear(const ColorRGB &clrFill, uint32_t clearFlags, Real depth, uint32_t stencil)
    {
        TResult ret = T3D_OK;
        
        do 
        {
            // if (D3DRTView == nullptr)
            // {
            //     break;
            // }
            //
            // if (D3DDSView == nullptr)
            // {
            //     break;
            // }
            //
            // D3D11_CONTEXT->clearRenderTargetView(&D3DRTView, clrFill);
            // D3D11_CONTEXT->clearDepthStencilView(&D3DDSView, depth, (uint8_t)stencil);
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------
}


