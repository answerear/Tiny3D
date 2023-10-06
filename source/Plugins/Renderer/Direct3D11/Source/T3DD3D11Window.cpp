/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2020  Answer Wong
 * For latest info, see https://github.com/answerear/Tiny3D
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/


#include "T3DD3D11Window.h"
#include "T3DD3D11Error.h"
#include "T3DD3D11Renderer.h"
#include "T3DD3D11Context.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    D3D11WindowPtr D3D11Window::create(RenderWindow *renderWindow, const RenderWindowCreateParam &param)
    {
        D3D11WindowPtr window = new D3D11Window();
        if (!window->init(renderWindow, param))
        {
            window = nullptr;
        }
        return window;
    }

    //--------------------------------------------------------------------------

    D3D11Window::D3D11Window()
        : mD3DSwapChain(nullptr)
        , mD3DRTView(nullptr)
        , mD3DDSView(nullptr)
    {
    }

    //--------------------------------------------------------------------------

    D3D11Window::~D3D11Window()
    {
        D3D_SAFE_RELEASE(mD3DDSView);
        D3D_SAFE_RELEASE(mD3DRTView);
        D3D_SAFE_RELEASE(mD3DSwapChain);
    }

    //--------------------------------------------------------------------------

    bool D3D11Window::init(RenderWindow *renderWindow, const RenderWindowCreateParam &param)
    {
        bool ret = true;

        do
        {
            TResult result;
            UINT uMSAAQuality = param.MSAA;
            UINT uMSAACount = 4;
            
            if (uMSAAQuality < 2)
                uMSAAQuality = 0;
            else if (uMSAAQuality > 16)
                uMSAAQuality = 16;
            
            DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;

            result = D3D11_CONTEXT->checkMultiSampleQuality((UINT)renderWindow->getWidth(), (UINT)renderWindow->getHeight(), uMSAAQuality, uMSAACount, format);
            if (T3D_FAILED(result))
            {
                ret = false;
                break;
            }

            SysWMInfo info;
            renderWindow->getSystemInfo(info);
            result = D3D11_CONTEXT->createSwapChain((HWND)info.hWnd, (UINT)renderWindow->getWidth(), (UINT)renderWindow->getHeight(), format, renderWindow->isFullscreen(), &mD3DSwapChain);
            if (T3D_FAILED(result))
            {
                ret = false;
                break;
            }

            result = D3D11_CONTEXT->createRenderTargetView(&mD3DSwapChain, &mD3DRTView);
            if (T3D_FAILED(result))
            {
                ret = false;
                break;
            }

            result = D3D11_CONTEXT->createDepthStencilView((UINT)renderWindow->getWidth(), (UINT)renderWindow->getHeight(), &mD3DDSView);
            if (T3D_FAILED(result))
            {
                ret = false;
                break;
            }
            
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

    TResult D3D11Window::swapBuffers()
    {
        TResult ret = T3D_OK;

        do
        {
            HRESULT hr = S_OK;
            hr = mD3DSwapChain->Present(0, 0);
            if (FAILED(hr))
            {
                ret = T3D_ERR_D3D11_PRESENT;
                T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Present failed ! DX ERROR [%d]", hr);
                break;
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Window::beginRender()
    {
        TResult ret = T3D_OK;

        do
        {
            if (mD3DRTView == nullptr)
            {
                break;
            }

            if (mD3DDSView == nullptr)
            {
                break;
            }

            ret = D3D11_CONTEXT->setRenderTargets(1, &mD3DRTView, &mD3DDSView);
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Window::endRender()
    {
        return D3D11_CONTEXT->setRenderTargets(0, nullptr, nullptr);
    }

    //--------------------------------------------------------------------------

    TResult D3D11Window::clear(const ColorRGB &clrFill, uint32_t clearFlags, Real depth, uint32_t stencil)
    {
        TResult ret = T3D_OK;
        
        do 
        {
            if (mD3DRTView == nullptr)
            {
                break;
            }
            
            if (mD3DDSView == nullptr)
            {
                break;
            }
            
            D3D11_CONTEXT->clearRenderTargetView(&mD3DRTView, clrFill);
            D3D11_CONTEXT->clearDepthStencilView(&mD3DDSView, depth, (uint8_t)stencil);
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------
}


