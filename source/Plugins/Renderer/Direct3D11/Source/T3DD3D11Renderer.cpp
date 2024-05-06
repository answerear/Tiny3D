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


#include "T3DD3D11Renderer.h"
#include "T3DD3D11Context.h"
#include "T3DD3D11RenderWindow.h"
#include "T3DEditorInfoDX11.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    D3D11RendererPtr D3D11Renderer::create()
    {
        return new D3D11Renderer();
    }
    
    //--------------------------------------------------------------------------

    D3D11Renderer::D3D11Renderer()
    {
        mName = DIRECT3D11;
    }
    
    //--------------------------------------------------------------------------

    D3D11Renderer::~D3D11Renderer()
    {
        cleanup();
    }
    
    //--------------------------------------------------------------------------

    TResult D3D11Renderer::init()
    {
        TResult ret = T3D_OK;

        do 
        {
            mContext = D3D11Context::create();
            // ret = mContext->init();
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Renderer::destroy()
    {
        TResult ret = T3D_OK;

        do 
        {
            cleanup();
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    void D3D11Renderer::getEditorInfo(void *info)
    {
        if (mContext != nullptr)
        {
            EditorInfoDX11 *dx11Info = static_cast<EditorInfoDX11*>(info);
            SDL_Window *sdlWindow = static_cast<SDL_Window*>(T3D_AGENT.getDefaultRenderWindow()->getNativeObject());
            dx11Info->sdlWindow = sdlWindow;
            D3D11Context *d3dContext = static_cast<D3D11Context*>(mContext.get());
            dx11Info->d3dDevice = d3dContext->getD3DDevice();
            dx11Info->d3dContext = d3dContext->getD3DDeviceContext();
        }
    }

    //--------------------------------------------------------------------------

    void D3D11Renderer::cleanup()
    {
        mContext = nullptr;
    }

    //--------------------------------------------------------------------------
}


