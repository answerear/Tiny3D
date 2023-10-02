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


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    D3D11WindowPtr D3D11Window::create()
    {
        D3D11WindowPtr window = new D3D11Window();
        if (!window->init())
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

    bool D3D11Window::init()
    {
        bool ret = true;

        do
        {
            // mPitch = Image::calcPitch(mWidth, mColorDepth);

            // ret = setupD3D11Environment(param, paramEx);
            // if (T3D_FAILED(ret))
            // {
            //     break;
            // }

            ret = T3D_OK;
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
                T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER,
                    "Present failed ! DX ERROR [%d]", hr);
                break;
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    void D3D11Window::clear(const ColorRGB &clrFill, uint32_t clearFlags, 
        Real depth, uint32_t stencil)
    {
        do 
        {
            // if (mD3DRTView == nullptr)
            // {
            //     break;
            // }
            //
            // if (mD3DDSView == nullptr)
            // {
            //     break;
            // }
            //
            // ID3D11DeviceContext *pD3DContext 
            //     = D3D11_CONTEXT.getD3DDeviceContext();
            // const float clr[4] = 
            // { clrFill.red(), clrFill.green(), clrFill.blue(), 1.0f };
            // pD3DContext->ClearRenderTargetView(mD3DRTView, clr);
            // pD3DContext->ClearDepthStencilView(mD3DDSView, 
            //     D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depth, stencil);
        } while (false);
    }

    //--------------------------------------------------------------------------

    TResult D3D11Window::setupD3D11Environment()
    {
        TResult ret = T3D_OK;

        do 
        {
            // UINT uMSAAQuality
            //     = (DWORD)paramEx.at("MultiSampleQuality").longValue();
            // UINT uMSAACount = 4;
            //
            // if (uMSAAQuality < 2)
            //     uMSAAQuality = 0;
            // else if (uMSAAQuality > 16)
            //     uMSAAQuality = 16;
            //
            // ID3D11Device *pD3DDevice = D3D11_CONTEXT.getD3DDevice();
            // DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
            // HRESULT hr = S_OK;
            //
            // if (uMSAAQuality == 0)
            // {
            //     uMSAACount = 1;
            // }
            // else
            // {
            //     UINT uNumQuality = 0;
            //     hr = pD3DDevice->CheckMultisampleQualityLevels(format,
            //         uMSAACount, &uNumQuality);
            //     if (FAILED(hr))
            //     {
            //         ret = T3D_ERR_D3D11_CHECK_MULTISAMPLE;
            //         T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER,
            //             "Check multiple sample quality levels failed ! "
            //             "DX ERROR [%d]", hr);
            //         break;
            //     }
            //
            //     uMSAAQuality = uNumQuality - 1;
            // }
            //
            // ret = createSwapChain((UINT)mWidth, (UINT)mHeight, param.fullscreen, 
            //     uMSAACount, uMSAAQuality, format);
            // if (T3D_FAILED(ret))
            // {
            //     T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER,
            //         "Create swap chain failed !");
            //     break;
            // }
            //
            // ret = createRenderTargetView();
            // if (T3D_FAILED(ret))
            // {
            //     T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER,
            //         "Create render target view failed !");
            //     break;
            // }
            //
            // ret = createDepthStencilView((UINT)mWidth, (UINT)mHeight, 
            //     uMSAACount, uMSAAQuality);
            // if (T3D_FAILED(ret))
            // {
            //     T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER,
            //         "Create depth and stencil view failed !");
            //     break;
            // }
            //
            // ID3D11DeviceContext *pD3DContext 
            //     = D3D11_CONTEXT.getD3DDeviceContext();
            // pD3DContext->OMSetRenderTargets(1, &mD3DRTView, mD3DDSView);

        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Window::createSwapChain(UINT uWidth, UINT uHeight, 
        bool bFullscreen, UINT uMSAACount, UINT uMSAAQuality, DXGI_FORMAT format)
    {
        TResult ret = T3D_OK;

        // IDXGIDevice *pDXGIDevice = nullptr;
        // IDXGIAdapter *pDXGIAdapter = nullptr;
        // IDXGIFactory *pDXGIFactory = nullptr;
        //
        // do 
        // {
        //     SysWMInfo info;
        //     mWindow->getSystemInfo(info);
        //     HWND hWnd = (HWND)info.hWnd;
        //
        //     ID3D11Device *pD3DDevice = D3D11_CONTEXT.getD3DDevice();
        //
        //     DXGI_SWAP_CHAIN_DESC desc;
        //
        //     desc.BufferDesc.Width = (UINT)mWidth;
        //     desc.BufferDesc.Height = (UINT)mHeight;
        //     desc.BufferDesc.RefreshRate.Numerator = 60;
        //     desc.BufferDesc.RefreshRate.Denominator = 1;
        //     desc.BufferDesc.Format = format;
        //     desc.BufferDesc.ScanlineOrdering 
        //         = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        //     desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
        //
        //     desc.SampleDesc.Count = uMSAACount;
        //     desc.SampleDesc.Quality = uMSAAQuality;
        //
        //     desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        //     desc.BufferCount = 1;
        //     desc.OutputWindow = hWnd;
        //     desc.Windowed = !bFullscreen;
        //     desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
        //     desc.Flags = 0;
        //
        //     HRESULT hr = S_OK;
        //
        //     hr = pD3DDevice->QueryInterface(__uuidof(IDXGIDevice), 
        //         (void **)&pDXGIDevice);
        //     if (FAILED(hr))
        //     {
        //         ret = T3D_ERR_D3D11_CREATE_FAILED;
        //         T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER,
        //             "Query interface for IDXGIDevice failed ! DX ERROR [%d]",
        //             hr);
        //         break;
        //     }
        //
        //     hr = pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), 
        //         (void **)&pDXGIAdapter);
        //     if (FAILED(hr))
        //     {
        //         ret = T3D_ERR_D3D11_GET_INTERFACE;
        //         T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER,
        //             "Get COM for IDXGIAdapter failed ! DX ERROR [%d]", hr);
        //         break;
        //     }
        //
        //     hr = pDXGIAdapter->GetParent(__uuidof(IDXGIFactory), 
        //         (void **)&pDXGIFactory);
        //     if (FAILED(hr))
        //     {
        //         ret = T3D_ERR_D3D11_GET_INTERFACE;
        //         T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER,
        //             "Get COM for IDXGIFactory failed ! DX ERROR [%d]", hr);
        //         break;
        //     }
        //
        //     hr = pDXGIFactory->CreateSwapChain(pD3DDevice, &desc, 
        //         &mD3DSwapChain);
        //     if (FAILED(hr))
        //     {
        //         ret = T3D_ERR_D3D11_CREATE_FAILED;
        //         T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER,
        //             "Create swap chain failed ! DX ERROR [%d]", hr);
        //         break;
        //     }
        // } while (0);
        //
        // D3D_SAFE_RELEASE(pDXGIFactory);
        // D3D_SAFE_RELEASE(pDXGIAdapter);
        // D3D_SAFE_RELEASE(pDXGIDevice);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Window::createRenderTargetView()
    {
        TResult ret = T3D_OK;

        // ID3D11Texture2D *pD3DBackBuffer = nullptr;
        //
        // do 
        // {
        //     HRESULT hr = S_OK;
        //     ID3D11Device *pD3DDevice = D3D11_CONTEXT.getD3DDevice();
        //     hr = mD3DSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
        //         reinterpret_cast<void **>(&pD3DBackBuffer));
        //     if (FAILED(hr))
        //     {
        //         ret = T3D_ERR_D3D11_GET_INTERFACE;
        //         T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER,
        //             "Get COM for ID3D11Texture2D failed ! DX ERROR [%d]", hr);
        //         break;
        //     }
        //
        //     hr = pD3DDevice->CreateRenderTargetView(pD3DBackBuffer, 0, 
        //         &mD3DRTView);
        //     if (FAILED(hr))
        //     {
        //         ret = T3D_ERR_D3D11_CREATE_FAILED;
        //         T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER,
        //             "Create render target view failed ! DX ERROR [%d]", hr);
        //         break;
        //     }
        // } while (0);
        //
        // D3D_SAFE_RELEASE(pD3DBackBuffer);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Window::createDepthStencilView(UINT uWidth, UINT uHeight,
        UINT uMSAACount, UINT uMSAAQuality)
    {
        TResult ret = T3D_OK;

        // ID3D11Texture2D *pD3DTexture = nullptr;
        //
        // do 
        // {
        //     ID3D11Device *pD3DDevice = D3D11_CONTEXT.getD3DDevice();
        //
        //     D3D11_TEXTURE2D_DESC desc;
        //     desc.Width = uWidth;
        //     desc.Height = uHeight;
        //     desc.MipLevels = 1;
        //     desc.ArraySize = 1;
        //     desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        //
        //     desc.SampleDesc.Count = uMSAACount;
        //     desc.SampleDesc.Quality = uMSAAQuality;
        //
        //     desc.Usage = D3D11_USAGE_DEFAULT;
        //     desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        //     desc.CPUAccessFlags = 0;
        //     desc.MiscFlags = 0;
        //
        //     HRESULT hr = S_OK;
        //     hr = pD3DDevice->CreateTexture2D(&desc, nullptr, &pD3DTexture);
        //     if (FAILED(hr))
        //     {
        //         ret = T3D_ERR_D3D11_CREATE_FAILED;
        //         T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER,
        //             "Create texture 2D failed ! DX ERROR [%d]", hr);
        //         break;
        //     }
        //
        //     hr = pD3DDevice->CreateDepthStencilView(pD3DTexture, 0, &mD3DDSView);
        //     if (FAILED(hr))
        //     {
        //         ret = T3D_ERR_D3D11_CREATE_FAILED;
        //         T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER,
        //             "Create depth stencil view failed ! DX ERROR [%d]", hr);
        //         break;
        //     }
        // } while (0);
        //
        // D3D_SAFE_RELEASE(pD3DTexture);

        return ret;
    }
}


