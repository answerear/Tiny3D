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


#include "T3DD3D11Context.h"
#include "T3DD3D11RenderWindow.h"
#include "T3DD3D11Error.h"
#include "T3DD3D11RenderBuffer.h"
#include "T3DD3D11Mapping.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    D3D11ContextPtr D3D11Context::create()
    {
        D3D11ContextPtr ctx = new D3D11Context();
        if (ctx && T3D_FAILED(ctx->init()))
        {
            ctx = nullptr;
        }
        return ctx;
    }

    //--------------------------------------------------------------------------

    D3D11Context::D3D11Context()
        : mInstance(nullptr)
    {
        
    }

    //--------------------------------------------------------------------------

    D3D11Context::~D3D11Context()
    {
        D3D_SAFE_RELEASE(mD3DDeviceContext);
        D3D_SAFE_RELEASE(mD3DDevice);
    }

    //--------------------------------------------------------------------------

#if 0
    TResult D3D11Context::init()
    {
        TResult ret = T3D_OK;

        do
        {
            HRESULT hr = S_OK;

            // device flags
            UINT flags = 0;
// #ifdef T3D_DEBUG
//             flags |= D3D11_CREATE_DEVICE_DEBUG;
// #endif

            // features level
            const UINT numLevels = 4;
            D3D_FEATURE_LEVEL featureLevels[numLevels] =
            {
                D3D_FEATURE_LEVEL_11_1,
                D3D_FEATURE_LEVEL_11_0,
                D3D_FEATURE_LEVEL_10_1,
                D3D_FEATURE_LEVEL_10_0,
            };

            D3D_FEATURE_LEVEL level;

            hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, flags, 
                featureLevels, numLevels, D3D11_SDK_VERSION, 
                &mD3DDevice, &level, &mD3DDeviceContext);
            if (FAILED(hr))
            {
                ret = T3D_ERR_D3D11_CREATE_FAILED;
                T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER,
                    "Create ID3D11Device object failed ! DX ERROR : %d", hr);
                break;
            }

            mFeatureLevel = level;

        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::renderAllTargets()
    {
        TResult ret = T3D_OK;

        do 
        {
            ret = RHIContext::renderAllTargets();
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    Matrix4 D3D11Context::makePerspectiveMatrix(const Radian &fovY, Real aspect, Real nearDist, Real farDist) const
    {
        // Direct3D 9 NDC (Normalized Device Coordinates) is : 
        //      [left, right] => [-1, 1] => l <= x <= r
        //      [bottom, top] => [-1, 1] => b <= y <= t
        //      [near, far] => [0, 1]    => -n <= z <= -f (Right Hand)
        //
        //  由正交投影矩阵推导过程可得：
        //      x' = 2x/(r-l) - (r+l)/(r-l)                 (1)
        //      y' = 2y/{t-b) - (t+b)/(t-b)                 (2)
        //
        //  根据相似三角形可得：
        //      x0 = nx/-z
        //      y0 = ny/-z
        //
        //  把 x0 和 y0 代入 (1)、(2) 式中的 x 和 y，可得：
        //      x' = (2nx/(r-l))/(-z) + ((r+l)z/(r-l))/(-z) (3)
        //      y' = (2ny/(t-b))/(-z) + ((t+b)z/(t-b))/(-z) (4)
        //
        //  当 z=-n 时 z'=0，当 z=-f 时 z'=1，即 -n <= z <= -f
        //  而 -zz' 和 z 是一种线性关系，即 
        //      z' = (pz + q) / -z                          (5)
        //  
        //  分别把 z'=0 和 z'=1 代入到 (5) 可得：
        //      0 = -pn + q                                 (6)
        //      f = -pf + q                                 (7)
        //  把 (6) 和 (7) 联列方程组可解得：
        //      p = f/(n-f)
        //      q = nf/(n-f)
        //
        //  考虑齐次坐标的w，通常情况下，如正交投影的时候w简单的等于-1，而现在
        //  我们需要为点 (-zx', -zy', -zz', -zw') 写一个变换，所以取而代之的是
        //  把 w'=1 写成：
        //      -zw' = z                                    (8)
        //
        //  最后把 p 和 q 代入等式 (5) 与等式 (3)、(4)、(8) 列方程组可得：
        //      x' = (2nx/(r-l))/(-z) + ((r+l)z/(r-l))/(-z)
        //      y' = (2ny/(t-b))/(-z) + ((t+b)z/(t-b))/(-z)
        //      z' = (fz/(n-f))/(-z) + (nf/(n-f))/(-z)
        //      w' = -1
        //
        //  通过矩阵形式表达上述方程组，可得：
        //          | 2n/(r-l)    0     (r+l)/(r-l)    0     |
        //      M = |    0     2n/(t-b) (t+b)/(t-b)    0     |      [1]
        //          |    0        0       f/(n-f)   nf/(n-f) |
        //          |    0        0         -1         0     |
        //
        //  一般情况下，相机都看屏幕正中，所以左右对称，上下对称，则可简化为：
        //          | 2n/w  0      0       0     |
        //      M = |  0   2n/h    0       0     |                  [2]
        //          |  0    0   f/(n-f) nf/(n-f) |
        //          |  0    0     -1       0     |
        //
        //  fovY 是指 top 和 bottom 之间夹角，则：
        //      tan(fovY/2) = (h/2)/n
        //  aspect 是指宽高比，即：
        //      aspect = w/h
        //
        //  从上可得 ：
        //      h = 2 * n * tan(fovY/2)
        //      w = aspect * h
        //      w = aspect * 2 * n * tan(fovY/2)
        //
        //  把上述代入矩阵[2]，可得：
        //          | 1/(aspect*tan(fovY/2))       0          0       0     |
        //      M = |           0            1/tan(fovY/2)    0       0     |
        //          |           0                  0       f/(n-f) nf/(n-f) |
        //          |           0                  0         -1       0     |

        Radian radian = fovY * REAL_HALF;
        Real m11 = REAL_ONE / (Math::tan(radian));
        Real m00 = m11 / aspect;
        Real m22 = farDist / (nearDist - farDist);
        Real m23 = nearDist * m22;

        return Matrix4(
            m00,   0,   0,   0,
              0, m11,   0,   0,
              0,   0, m22, m23,
              0,   0,  -1,   0);
    }

    Matrix4 D3D11Context::makeOrthographicMatrix(Real width, Real height, Real nearDist, Real farDist)
    {
        // Direct3D 9 NDC (Normalized Device Coordinates) is : 
        //      [left, right] => [-1, 1] => l <= x <= r
        //      [bottom, top] => [-1, 1] => b <= y <= t
        //      [near, far] => [0, 1]    => -n <= z <= -f (Right Hand)
        // 
        //  由 l <= x <= r 
        //      => 0 <= x-l <= r-l 
        //      => 0 <= (x-l)/(r-l) <= 1 
        //      => 0 <= 2(x-l)/(r-) <= 2
        //      => 0 <= (2x-2l)/(r-l) <= 2
        //      => -1 <= (2x-2l)/(r-l)-1 <= 1
        //      => -1 <= (2x-r-l)/(r-l) <= 1
        //      => -1 <= 2x/(r-l)-(r+l)/(r-l) <= 1
        //  由上可得： x' = 2x/(r-l) - (r+l)/(r-l)       (1)
        //
        //  同理可得： y' = 2y/(t-b) - (t+b)/(t-b)       (2)
        //
        //  由 -n <= z <= -f
        //      => 0 <= z+n <= n-f
        //      => 0 <= (z+n)/(n-f> <= 1
        //      => 0 <= z/(n-f) + n/(n-f) <= 1
        //  由上可得： z' = z/(n-f) + n/(n-f)            (3)
        //
        //  由(1)、(2)和(3)等式构成方程组：
        //      x' = 2x/(r-l) - (r+l)/(r-l) 
        //      y' = 2y/(t-b) - (t+b)/(t-b)
        //      z' = z/(n-f) + n/(n-f)
        //
        //  由方程组可得矩阵：
        //          | 2/(r-l)    0       0    -(r+l)/(r-l) |
        //      M = |    0    2/(t-b)    0    -(t+b)/(t-b) |        [1]
        //          |    0       0    1/(n-f)    n/(n-f)   |
        //          |    0       0       0          1      |
        //
        //  一般情况下，相机都看屏幕正中，所以左右对称，上下对称，则可简化为：
        //          | 2/w  0     0       0    |
        //      M = |  0  2/h    0       0    |                     [2]
        //          |  0   0  1/(n-f) n/(n-f) |
        //          |  0   0     0       1    |

        Real m00 = Real(2.0) / width;
        Real m11 = Real(2.0) / height;
        Real m22 = REAL_ONE / (nearDist - farDist);
        Real m23 = nearDist * m22;

        return Matrix4(
            m00,   0,   0,   0,
              0, m11,   0,   0,
              0,   0, m22, m23,
              0,   0,   0,   1);
    }

    Matrix4 D3D11Context::makeViewportMatrix(Viewport *viewport)
    {
        return Matrix4::IDENTITY;
    }

    RHIRenderWindowPtr D3D11Context::createRenderWindow(RenderWindow *window, const RenderWindowDesc &param)
    {
        return D3D11RenderWindow::create(window, param);
    }

    TResult D3D11Context::clear(const ColorRGB &color, uint32_t clearFlags, Real depth, uint32_t stencil)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    RHIBlendStatePtr D3D11Context::createBlendState(const BlendDesc &desc)
    {
        return nullptr;
    }

    RHIDepthStencilStatePtr D3D11Context::createDepthStencilState(const DepthStencilDesc &desc)
    {
        return nullptr;
    }

    RHIRasterizerStatePtr D3D11Context::createRasterizerState(const RasterizerDesc &desc)
    {
        return nullptr;
    }

    RHISamplerStatePtr D3D11Context::createSamplerState(const SamplerDesc &desc)
    {
        return nullptr;
    }

    TResult D3D11Context::setBlendState(RHIBlendStatePtr state)
    {
        return T3D_OK;
    }

    TResult D3D11Context::setDepthStencilState(RHIDepthStencilStatePtr state)
    {
        return T3D_OK;
    }

    TResult D3D11Context::setRasterizerState(RHIRasterizerStatePtr state)
    {
        return T3D_OK;
    }
        
    TResult D3D11Context::setSamplerState(RHISamplerStatePtr state)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::setViewport(Viewport *viewport)
    {
        D3D11_VIEWPORT vp;
        vp.TopLeftX = float(viewport->getActualLeft());
        vp.TopLeftY = float(viewport->getActualTop());
        vp.Width = float(viewport->getActualWidth());
        vp.Height = float(viewport->getActualHeight());
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;

        auto lambda = [this](D3D11_VIEWPORT vp)
        {
            mD3DDeviceContext->RSSetViewports(1, &vp);
            return T3D_OK;
        };
        return ENQUEUE_UNIQUE_COMMAND(lambda, vp);
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::setShader()
    {
        return T3D_OK;
    }

    TResult D3D11Context::setTexture()
    {
        return T3D_OK;
    }

    TResult D3D11Context::renderObject()
    {
        return T3D_OK;
    }
    
    //--------------------------------------------------------------------------

    TResult D3D11Context::checkMultiSampleQuality(UINT width, UINT height, UINT uMSAAQuality, UINT uMSAACount, DXGI_FORMAT format)
    {
        auto lambda = [this](UINT width, UINT height, UINT uMSAAQuality, UINT uMSAACount, DXGI_FORMAT format)
        {
            HRESULT hr = S_OK;

            do
            {
                if (uMSAAQuality == 0)
                {
                    mMSAACount = 1;
                }
                else
                {
                    UINT uNumQuality = 0;
                    hr = mD3DDevice->CheckMultisampleQualityLevels(format, uMSAACount, &uNumQuality);
                    if (FAILED(hr))
                    {
                        T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Check multiple sample quality levels failed ! DX ERROR [%d]", hr);
                        break;
                    }
                
                    uMSAAQuality = uNumQuality - 1;

                    mMSAACount = uNumQuality;
                    mMSAAQuality = uMSAAQuality;
                }
            } while (false);
                
            return T3D_OK;
        };
        return ENQUEUE_UNIQUE_COMMAND(lambda, width, height, uMSAAQuality, uMSAACount, format);
    }
    
    //--------------------------------------------------------------------------

    TResult D3D11Context::createSwapChain(HWND hwnd, UINT width, UINT height, DXGI_FORMAT format, bool fullscreen, IDXGISwapChain **ppD3DSwapChain)
    {
        auto lambda = [this](HWND hwnd, UINT width, UINT height, DXGI_FORMAT format, bool fullscreen, IDXGISwapChain **ppD3DSwapChain)
        {
            TResult ret = T3D_OK;
            IDXGIDevice *pDXGIDevice = nullptr;
            IDXGIAdapter *pDXGIAdapter = nullptr;
            IDXGIFactory *pDXGIFactory = nullptr;
            
            do 
            {
                DXGI_SWAP_CHAIN_DESC desc;
            
                desc.BufferDesc.Width = width;
                desc.BufferDesc.Height = height;
                desc.BufferDesc.RefreshRate.Numerator = 60;
                desc.BufferDesc.RefreshRate.Denominator = 1;
                desc.BufferDesc.Format = format;
                desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
                desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
                desc.SampleDesc.Count = mMSAACount;
                desc.SampleDesc.Quality = mMSAAQuality;
                desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
                desc.BufferCount = 1;
                desc.OutputWindow = hwnd;
                desc.Windowed = !fullscreen;
                desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
                desc.Flags = 0;
            
                HRESULT hr = S_OK;
            
                hr = mD3DDevice->QueryInterface(__uuidof(IDXGIDevice), (void **)&pDXGIDevice);
                if (FAILED(hr))
                {
                    ret = T3D_ERR_D3D11_CREATE_FAILED;
                    T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Query interface for IDXGIDevice failed ! DX ERROR [%d]", hr);
                    break;
                }
            
                hr = pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void **)&pDXGIAdapter);
                if (FAILED(hr))
                {
                    ret = T3D_ERR_D3D11_GET_INTERFACE;
                    T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Get COM for IDXGIAdapter failed ! DX ERROR [%d]", hr);
                    break;
                }
            
                hr = pDXGIAdapter->GetParent(__uuidof(IDXGIFactory), (void **)&pDXGIFactory);
                if (FAILED(hr))
                {
                    ret = T3D_ERR_D3D11_GET_INTERFACE;
                    T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Get COM for IDXGIFactory failed ! DX ERROR [%d]", hr);
                    break;
                }
            
                hr = pDXGIFactory->CreateSwapChain(mD3DDevice, &desc, ppD3DSwapChain);
                if (FAILED(hr))
                {
                    ret = T3D_ERR_D3D11_CREATE_FAILED;
                    T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Create swap chain failed ! DX ERROR [%d]", hr);
                    break;
                }
            } while (false);
            
            D3D_SAFE_RELEASE(pDXGIFactory);
            D3D_SAFE_RELEASE(pDXGIAdapter);
            D3D_SAFE_RELEASE(pDXGIDevice);

            return ret;
        };
        
        return ENQUEUE_UNIQUE_COMMAND(lambda, hwnd, width, height, format, fullscreen, ppD3DSwapChain);
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::createRenderTargetView(D3D11RenderWindow *renderWindow)
    {
        auto lambda = [this](D3D11RenderWindow *renderWindow)
        {
            TResult ret = T3D_OK;
            ID3D11Texture2D *pD3DBackBuffer = nullptr;
                
            do 
            {
                HRESULT hr = S_OK;
                hr = renderWindow->D3DSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&pD3DBackBuffer));
                if (FAILED(hr))
                {
                    ret = T3D_ERR_D3D11_GET_INTERFACE;
                    T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Get COM for ID3D11Texture2D failed ! DX ERROR [%d]", hr);
                    break;
                }
                
                hr = mD3DDevice->CreateRenderTargetView(pD3DBackBuffer, nullptr, &renderWindow->D3DRTView);
                if (FAILED(hr))
                {
                    ret = T3D_ERR_D3D11_CREATE_FAILED;
                    T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Create render target view failed ! DX ERROR [%d]", hr);
                    break;
                }
            } while (false);

            D3D_SAFE_RELEASE(pD3DBackBuffer);

            return ret;
        };
        return ENQUEUE_UNIQUE_COMMAND(lambda, renderWindow);
    }
    
    //--------------------------------------------------------------------------

    TResult D3D11Context::createDepthStencilView(D3D11RenderWindow *renderWindow)
    {
        auto lambda = [this](D3D11RenderWindow *renderWindow)
        {
            TResult ret = T3D_OK;

            ID3D11Texture2D *pD3DTexture = nullptr;
                
            do 
            {
                D3D11_TEXTURE2D_DESC desc;
                desc.Width = width;
                desc.Height = height;
                desc.MipLevels = 1;
                desc.ArraySize = 1;
                desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
                
                desc.SampleDesc.Count = mMSAACount;
                desc.SampleDesc.Quality = mMSAAQuality;
                
                desc.Usage = D3D11_USAGE_DEFAULT;
                desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
                desc.CPUAccessFlags = 0;
                desc.MiscFlags = 0;
                
                HRESULT hr = S_OK;
                hr = mD3DDevice->CreateTexture2D(&desc, nullptr, &pD3DTexture);
                if (FAILED(hr))
                {
                    ret = T3D_ERR_D3D11_CREATE_FAILED;
                    T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Create texture 2D failed ! DX ERROR [%d]", hr);
                    break;
                }
                
                hr = mD3DDevice->CreateDepthStencilView(pD3DTexture, nullptr, ppD3DDSView);
                if (FAILED(hr))
                {
                    ret = T3D_ERR_D3D11_CREATE_FAILED;
                    T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Create depth stencil view failed ! DX ERROR [%d]", hr);
                    break;
                }
            } while (false);
                
            D3D_SAFE_RELEASE(pD3DTexture);

            return ret;
        };
        return ENQUEUE_UNIQUE_COMMAND(lambda, width, height,  ppD3DDSView);
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::createRenderTargetView(IDXGISwapChain **pD3DSwapChain, ID3D11RenderTargetView **ppD3DRTView)
    {
        auto lambda = [this](IDXGISwapChain **pD3DSwapChain, ID3D11RenderTargetView **ppD3DRTView)
        {
            TResult ret = T3D_OK;
            ID3D11Texture2D *pD3DBackBuffer = nullptr;
                
            do 
            {
                HRESULT hr = S_OK;
                hr = (*pD3DSwapChain)->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&pD3DBackBuffer));
                if (FAILED(hr))
                {
                    ret = T3D_ERR_D3D11_GET_INTERFACE;
                    T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Get COM for ID3D11Texture2D failed ! DX ERROR [%d]", hr);
                    break;
                }
                
                hr = mD3DDevice->CreateRenderTargetView(pD3DBackBuffer, nullptr, ppD3DRTView);
                if (FAILED(hr))
                {
                    ret = T3D_ERR_D3D11_CREATE_FAILED;
                    T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Create render target view failed ! DX ERROR [%d]", hr);
                    break;
                }
            } while (false);

            D3D_SAFE_RELEASE(pD3DBackBuffer);

            return ret;
        };
        return ENQUEUE_UNIQUE_COMMAND(lambda, pD3DSwapChain, ppD3DRTView);
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::createDepthStencilView(UINT width, UINT height, ID3D11DepthStencilView **ppD3DDSView)
    {
        auto lambda = [this](UINT width, UINT height, ID3D11DepthStencilView **ppD3DDSView)
        {
            TResult ret = T3D_OK;

            ID3D11Texture2D *pD3DTexture = nullptr;
                
            do 
            {
                D3D11_TEXTURE2D_DESC desc;
                desc.Width = width;
                desc.Height = height;
                desc.MipLevels = 1;
                desc.ArraySize = 1;
                desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
                
                desc.SampleDesc.Count = mMSAACount;
                desc.SampleDesc.Quality = mMSAAQuality;
                
                desc.Usage = D3D11_USAGE_DEFAULT;
                desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
                desc.CPUAccessFlags = 0;
                desc.MiscFlags = 0;
                
                HRESULT hr = S_OK;
                hr = mD3DDevice->CreateTexture2D(&desc, nullptr, &pD3DTexture);
                if (FAILED(hr))
                {
                    ret = T3D_ERR_D3D11_CREATE_FAILED;
                    T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Create texture 2D failed ! DX ERROR [%d]", hr);
                    break;
                }
                
                hr = mD3DDevice->CreateDepthStencilView(pD3DTexture, nullptr, ppD3DDSView);
                if (FAILED(hr))
                {
                    ret = T3D_ERR_D3D11_CREATE_FAILED;
                    T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Create depth stencil view failed ! DX ERROR [%d]", hr);
                    break;
                }
            } while (false);
                
            D3D_SAFE_RELEASE(pD3DTexture);

            return ret;
        };
        return ENQUEUE_UNIQUE_COMMAND(lambda, width, height,  ppD3DDSView);
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::setRenderTargets(UINT numOfViews, ID3D11RenderTargetView **ppD3DRTView, ID3D11DepthStencilView **ppD3DDSView)
    {
        auto lambda = [this](UINT numOfViews, ID3D11RenderTargetView **ppD3DRTView, ID3D11DepthStencilView **ppD3DDSView)
        {
            if (ppD3DDSView != nullptr)
            {
                mD3DDeviceContext->OMSetRenderTargets(numOfViews, ppD3DRTView, *ppD3DDSView);
            }
            else
            {
                mD3DDeviceContext->OMSetRenderTargets(numOfViews, ppD3DRTView, nullptr);
            }
            return T3D_OK;
        };
        return ENQUEUE_UNIQUE_COMMAND(lambda, numOfViews, ppD3DRTView, ppD3DDSView);
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::clearRenderTargetView(ID3D11RenderTargetView **pD3DRTView, const ColorRGB &clrFill)
    {
        auto lambda = [this](ID3D11RenderTargetView **pD3DRTView, ColorRGB clrFill)
        {
            float clr[4] = {clrFill.red(), clrFill.green(), clrFill.blue(), 1.0f};
            mD3DDeviceContext->ClearRenderTargetView(*pD3DRTView, clr);
            T3D_LOG_INFO(LOG_TAG_D3D11RENDERER, "Clear render target view with color {%f, %f, %f, %f}", clr[0], clr[1], clr[2], clr[3]);
            return T3D_OK;
        };
        return ENQUEUE_UNIQUE_COMMAND(lambda, pD3DRTView, clrFill);
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::clearDepthStencilView(ID3D11DepthStencilView **pD3DDSView, Real depth, uint8_t stencil)
    {
        auto lambda = [this](ID3D11DepthStencilView **pD3DDSView, Real depth, uint8_t stencil)
        {
            mD3DDeviceContext->ClearDepthStencilView(*pD3DDSView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depth, stencil);
            return T3D_OK;
        };
        return ENQUEUE_UNIQUE_COMMAND(lambda, pD3DDSView, depth, stencil);
    }
#else

    //--------------------------------------------------------------------------

    TResult D3D11Context::init()
    {
        TResult ret = T3D_OK;

        do
        {
            HRESULT hr = S_OK;

            // device flags
            UINT flags = 0;
#ifdef T3D_DEBUG
            flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

            // features level
            const UINT numLevels = 4;
            D3D_FEATURE_LEVEL featureLevels[numLevels] =
            {
                D3D_FEATURE_LEVEL_11_1,
                D3D_FEATURE_LEVEL_11_0,
                D3D_FEATURE_LEVEL_10_1,
                D3D_FEATURE_LEVEL_10_0,
            };

            D3D_FEATURE_LEVEL level;

            hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, flags, 
                featureLevels, numLevels, D3D11_SDK_VERSION, 
                &mD3DDevice, &level, &mD3DDeviceContext);
            if (FAILED(hr))
            {
                ret = T3D_ERR_D3D11_CREATE_FAILED;
                T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER,
                    "Create ID3D11Device object failed ! DX ERROR : %d", hr);
                break;
            }

            mFeatureLevel = level;

            setupBlitQuad();
        } while (false);

        return ret;
    }
    
    //--------------------------------------------------------------------------

    void D3D11Context::setupBlitQuad()
    {
        BlitVertex vertices[4] =
        {
            { Vector3(-1.0f, 1.0f, 0.5f), Vector2(0.0f, 0.0f) },
            { Vector3(-1.0f, -1.0f, 0.5f), Vector2(0.0f, 1.0f) },
            { Vector3(1.0f, 1.0f, 0.5f), Vector2(1.0f, 0.0f) },
            { Vector3(1.0f, -1.0f, 0.5f), Vector2(1.0f, 1.0f) }
        };

        // 创建顶点缓冲区
        D3D11_BUFFER_DESC bd = {};
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(BlitVertex) * 4;
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = 0;
        D3D11_SUBRESOURCE_DATA initData = {};
        initData.pSysMem = vertices;
        ID3D11Buffer* vertexBuffer = nullptr;
        mD3DDevice->CreateBuffer(&bd, &initData, &vertexBuffer);

        // 编译顶点着色器
        const String vs =
            "struct VS_INPUT\
            {\
                float3 Pos : POSITION;\
                float2 Tex : TEXCOORD0;\
            };\
            struct PS_INPUT\
            {\
                float4 Pos : SV_POSITION;\
                float2 Tex : TEXCOORD0;\
            };\
            PS_INPUT VS(VS_INPUT input)\
            {\
                PS_INPUT output;\
                output.Pos = float4(input.Pos, 1.0f);\
                output.Tex = input.Tex;\
                return output;\
            }";

        ID3DBlob *vertexShaderBlob = nullptr;
        ID3DBlob *errorMsgBlob = nullptr;
        HRESULT hr = D3DCompile(vs.c_str(), vs.length(), "BlitVertexShader.hlsl", nullptr, nullptr, "VS", "vs_4_0", 0, 0, &vertexShaderBlob, &errorMsgBlob);
        T3D_ASSERT(SUCCEEDED(hr), "Compile blit vertex shader !");        
        mD3DDevice->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), nullptr, &mBlitVS);

        // 编译像素着色器
        const String ps =
            "Texture2D gSrcTexture : register(t0);\
            SamplerState gSampler : register(s0);\
            struct PS_INPUT\
            {\
                float4 Pos : SV_POSITION;\
                float2 Tex : TEXCOORD0;\
            };\
            float4 PS(PS_INPUT input) : SV_Target\
            {\
                return gSrcTexture.Sample(gSampler, input.Tex);\
            }";
        
        ID3DBlob* pixelShaderBlob = nullptr;
        hr = D3DCompile(ps.c_str(), ps.length(), "BlitPixelShader.hlsl", nullptr, nullptr, "PS", "ps_4_0", 0, 0, &pixelShaderBlob, &errorMsgBlob);
        T3D_ASSERT(SUCCEEDED(hr), "Compile blit vertex shader !");        
        mD3DDevice->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), nullptr, &mBlitPS);

        // 创建输入布局
        D3D11_INPUT_ELEMENT_DESC layout[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        mD3DDevice->CreateInputLayout(layout, ARRAYSIZE(layout), vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), &mBlitLayout);
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::swapBackBuffer(D3D11RenderWindow *renderWindow)
    {
        auto lambda = [this](D3D11RenderWindow *renderWindow)
        {
            TResult ret = T3D_OK;
            do
            {
                HRESULT hr = S_OK;
                hr = renderWindow->D3DSwapChain->Present(0, 0);
                if (FAILED(hr))
                {
                    ret = T3D_ERR_D3D11_PRESENT;
                    T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Present failed ! DX ERROR [%d]", hr);
                    break;
                }
            } while (false);
            
            return ret;
        };
        return ENQUEUE_UNIQUE_COMMAND(lambda, renderWindow);
    }

    //--------------------------------------------------------------------------
    
    RHIRenderTargetPtr D3D11Context::createRenderWindow(RenderWindowPtr renderWindow)
    {
        D3D11RenderWindowPtr d3dRenderWindow = D3D11RenderWindow::create(renderWindow);

        auto lambda = [this](RenderWindow *pRenderWindow, D3D11RenderWindow *pD3DRenderWindow)
        {
            TResult ret = T3D_OK;
            IDXGIDevice *pDXGIDevice = nullptr;
            IDXGIAdapter *pDXGIAdapter = nullptr;
            IDXGIFactory *pDXGIFactory = nullptr;
            
            do
            {
                HRESULT hr = S_OK;
                
                const RenderWindowDesc &desc = pRenderWindow->getDescriptor();

                // 获取支持的 MSAA
                UINT uMSAACount = desc.MSAA.Count;
                UINT uMSAAQuality = desc.MSAA.Quality;

                DXGI_FORMAT format = DXGI_FORMAT_B8G8R8A8_UNORM;
                
                if (desc.MSAA.Count == 0)
                {
                    uMSAACount = 1;
                }
                else
                {
                    UINT uNumQuality = 0;
                    hr = mD3DDevice->CheckMultisampleQualityLevels(format, uMSAACount, &uNumQuality);
                    if (FAILED(hr))
                    {
                        T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Check multiple sample quality levels failed ! DX ERROR [%d]", hr);
                        break;
                    }
                    
                    uMSAAQuality = uNumQuality - 1;
                    // uMSAACount = uNumQuality;
                }

                // 创建 Swap Chain
                SysWMInfo info;
                pRenderWindow->getSystemInfo(info);

                DXGI_SWAP_CHAIN_DESC d3dSwapChainDesc;
            
                d3dSwapChainDesc.BufferDesc.Width = desc.Width;
                d3dSwapChainDesc.BufferDesc.Height = desc.Height;
                d3dSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
                d3dSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
                d3dSwapChainDesc.BufferDesc.Format = format;
                d3dSwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
                d3dSwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
                d3dSwapChainDesc.SampleDesc.Count = uMSAACount;
                d3dSwapChainDesc.SampleDesc.Quality = uMSAAQuality;
                d3dSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
                d3dSwapChainDesc.BufferCount = 1;
                d3dSwapChainDesc.OutputWindow = (HWND)info.hWnd;
                d3dSwapChainDesc.Windowed = !desc.IsFullscreen;
                d3dSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
                d3dSwapChainDesc.Flags = 0;
            
                hr = mD3DDevice->QueryInterface(__uuidof(IDXGIDevice), (void **)&pDXGIDevice);
                if (FAILED(hr))
                {
                    ret = T3D_ERR_D3D11_CREATE_FAILED;
                    T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Query interface for IDXGIDevice failed ! DX ERROR [%d]", hr);
                    break;
                }
            
                hr = pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void **)&pDXGIAdapter);
                if (FAILED(hr))
                {
                    ret = T3D_ERR_D3D11_GET_INTERFACE;
                    T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Get COM for IDXGIAdapter failed ! DX ERROR [%d]", hr);
                    break;
                }
            
                hr = pDXGIAdapter->GetParent(__uuidof(IDXGIFactory), (void **)&pDXGIFactory);
                if (FAILED(hr))
                {
                    ret = T3D_ERR_D3D11_GET_INTERFACE;
                    T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Get COM for IDXGIFactory failed ! DX ERROR [%d]", hr);
                    break;
                }
            
                hr = pDXGIFactory->CreateSwapChain(mD3DDevice, &d3dSwapChainDesc, &pD3DRenderWindow->D3DSwapChain);
                if (FAILED(hr))
                {
                    ret = T3D_ERR_D3D11_CREATE_FAILED;
                    T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Create swap chain failed ! DX ERROR [%d]", hr);
                    break;
                }

                // 创建 RenderTargetView
                hr = pD3DRenderWindow->D3DSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&pD3DRenderWindow->D3DBackBuffer));
                if (FAILED(hr))
                {
                    ret = T3D_ERR_D3D11_GET_INTERFACE;
                    T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Get COM for ID3D11Texture2D failed ! DX ERROR [%d]", hr);
                    break;
                }
                
                hr = mD3DDevice->CreateRenderTargetView(pD3DRenderWindow->D3DBackBuffer, nullptr, &pD3DRenderWindow->D3DRTView);
                if (FAILED(hr))
                {
                    ret = T3D_ERR_D3D11_CREATE_FAILED;
                    T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Create render target view failed ! DX ERROR [%d]", hr);
                    break;
                }

                // 创建 DepthStencilView
                D3D11_TEXTURE2D_DESC d3dTexDesc;
                d3dTexDesc.Width = desc.Width;
                d3dTexDesc.Height = desc.Height;
                d3dTexDesc.MipLevels = 1;
                d3dTexDesc.ArraySize = 1;
                d3dTexDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
                
                d3dTexDesc.SampleDesc.Count = uMSAACount;
                d3dTexDesc.SampleDesc.Quality = uMSAAQuality;
                
                d3dTexDesc.Usage = D3D11_USAGE_DEFAULT;
                d3dTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
                d3dTexDesc.CPUAccessFlags = 0;
                d3dTexDesc.MiscFlags = 0;

                ID3D11Texture2D *pD3DDSBuffer = nullptr;
                hr = mD3DDevice->CreateTexture2D(&d3dTexDesc, nullptr, &pD3DDSBuffer);
                if (FAILED(hr))
                {
                    ret = T3D_ERR_D3D11_CREATE_FAILED;
                    T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Create texture 2D failed ! DX ERROR [%d]", hr);
                    break;
                }
                
                hr = mD3DDevice->CreateDepthStencilView(pD3DDSBuffer, nullptr, &pD3DRenderWindow->D3DDSView);
                if (FAILED(hr))
                {
                    ret = T3D_ERR_D3D11_CREATE_FAILED;
                    T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Create depth stencil view failed ! DX ERROR [%d]", hr);
                    break;
                }
            } while (false);

            D3D_SAFE_RELEASE(pDXGIFactory);
            D3D_SAFE_RELEASE(pDXGIAdapter);
            D3D_SAFE_RELEASE(pDXGIDevice);
            
            return ret;
        };

        TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, renderWindow.get(), d3dRenderWindow.get());
        
        if (T3D_FAILED(ret))
        {
            d3dRenderWindow = nullptr;
        }

        return d3dRenderWindow;
    }

    //--------------------------------------------------------------------------
    
    RHIPixelBuffer2DPtr D3D11Context::createRenderTexture(PixelBuffer2DPtr buffer)
    {
        D3D11PixelBuffer2DPtr d3dPixelBuffer = D3D11PixelBuffer2D::create();

        auto lambda = [this](PixelBuffer2D *buffer, D3D11PixelBuffer2D *d3dPixelBuffer)
        {
            TResult ret = T3D_OK;

            do
            {
                // 获取支持的 MSAA
                UINT uMSAACount = buffer->getDescriptor().sampleDesc.Count;
                UINT uMSAAQuality = buffer->getDescriptor().sampleDesc.Quality;

                DXGI_FORMAT format = D3D11Mapping::get(buffer->getDescriptor().format);

                if (uMSAACount == 0)
                {
                    uMSAACount = 1;
                }
                else
                {
                    UINT uNumQuality = 0;
                    HRESULT hr = mD3DDevice->CheckMultisampleQualityLevels(format, uMSAACount, &uNumQuality);
                    if (FAILED(hr))
                    {
                        T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Check multiple sample quality levels failed ! DX ERROR [%d]", hr);
                        break;
                    }

                    uMSAAQuality = uNumQuality - 1;
                    // uMSAACount = uNumQuality;
                }

                // 创建颜色纹理资源
                D3D11_TEXTURE2D_DESC texDesc = D3D11Mapping::get(buffer->getDescriptor());  
                texDesc.SampleDesc.Count = uMSAACount;
                texDesc.SampleDesc.Quality = uMSAAQuality;
                texDesc.Usage = D3D11Mapping::get(buffer->getUsage()); // 设置纹理用途
                texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE; // 设置纹理绑定标志
                texDesc.CPUAccessFlags = D3D11Mapping::get(buffer->getCPUAccessMode()); // 设置 CPU 访问标志
                texDesc.MiscFlags = 0; // 设置其他标志

                HRESULT hr = mD3DDevice->CreateTexture2D(&texDesc, nullptr, &d3dPixelBuffer->D3DTexture);
                if (FAILED(hr))
                {
                    // 错误
                    ret = T3D_ERR_D3D11_CREATE_TEXTURE2D;
                    T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Create color texture failed when create render texture ! DX ERROR [%d]", hr);
                    break;
                }

                // 创建渲染目标视图
                hr = mD3DDevice->CreateRenderTargetView(d3dPixelBuffer->D3DTexture, nullptr, &d3dPixelBuffer->D3DRTView);
                if (FAILED(hr))
                {
                    // 错误
                    ret = T3D_ERR_D3D11_CREATE_RENDER_TARGET_VIEW;
                    T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "CreateRenderTargetView failed when create render texture ! DX ERROR [%d]", hr);
                    break;
                }

                // 创建 depth & stencil 纹理
                D3D11_TEXTURE2D_DESC depthStencilDesc = D3D11Mapping::get(buffer->getDescriptor());
                depthStencilDesc.SampleDesc.Count = uMSAACount;
                depthStencilDesc.SampleDesc.Quality = uMSAAQuality;
                depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
                depthStencilDesc.Usage = D3D11Mapping::get(buffer->getUsage());
                depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
                depthStencilDesc.CPUAccessFlags = D3D11Mapping::get(buffer->getCPUAccessMode());
                depthStencilDesc.MiscFlags = 0;

                hr = mD3DDevice->CreateTexture2D(&depthStencilDesc, nullptr, &d3dPixelBuffer->D3DDSTexture);
                if (FAILED(hr))
                {
                    // 错误
                    ret = T3D_ERR_D3D11_CREATE_TEXTURE2D;
                    T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Create depth & stencil texture failed when create render texture ! DX ERROR [%d]", hr);
                    break;
                }

                // 创建 depth & stencil 视图
                hr = mD3DDevice->CreateDepthStencilView(d3dPixelBuffer->D3DDSTexture, nullptr, &d3dPixelBuffer->D3DDSView);
                if (FAILED(hr))
                {
                    // 错误
                    ret = T3D_ERR_D3D11_CREATE_DEPTH_STENCIL_VIEW;
                    T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "CreateDepthStencilView failed when create render texture ! DX ERROR [%d]", hr);
                    break;
                }

                // 创建着色器资源视图
                hr = mD3DDevice->CreateShaderResourceView(d3dPixelBuffer->D3DTexture, nullptr, &d3dPixelBuffer->D3DSRView);
                if (FAILED(hr))
                {
                    // 错误
                    ret = T3D_ERR_D3D11_CREATE_SHADER_RESOURCE_VIEW;
                    T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "CreateShaderResourceView failed when create render texture ! DX ERROR [%d]", hr);
                    break;
                }
            } while (false);
            
            return ret;
        };
        
        TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, buffer.get(), d3dPixelBuffer.get());
        if (T3D_FAILED(ret))
        {
            d3dPixelBuffer = nullptr;
        }
        
        return d3dPixelBuffer;
    }
    
    //--------------------------------------------------------------------------
    
    TResult D3D11Context::setRenderTarget(RenderWindowPtr renderWindow)
    {
        D3D11RenderWindow *pD3DRenderWindow = static_cast<D3D11RenderWindow*>(renderWindow->getRHIRenderWindow().get());
        auto lambda = [this](D3D11RenderWindow *pD3DRenderWindow)
        {
            mD3DDeviceContext->OMSetRenderTargets(1, &pD3DRenderWindow->D3DRTView, pD3DRenderWindow->D3DDSView);
            return T3D_OK;
        };
        
        TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, pD3DRenderWindow);
        
        if (T3D_SUCCEEDED(ret))
        {
            mCurrentRenderWindow = renderWindow;
        }
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::setRenderTarget(RenderTexturePtr renderTexture)
    {
        D3D11PixelBuffer2D *pD3DPixelBuffer = static_cast<D3D11PixelBuffer2D*>(renderTexture->getPixelBuffer()->getRHIResource().get());
        
        auto lambda = [this](D3D11PixelBuffer2D *pD3DPixelBuffer)
        {
            mD3DDeviceContext->OMSetRenderTargets(1, &pD3DPixelBuffer->D3DRTView, pD3DPixelBuffer->D3DDSView);
            return T3D_OK;
        };

        TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, pD3DPixelBuffer);
        
        if (T3D_SUCCEEDED(ret))
        {
            mCurrentRenderTexture = renderTexture;
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::setRenderTarget(RenderTargetPtr renderTarget)
    {
        TResult ret = T3D_OK;

        switch (renderTarget->getType())
        {
        case RenderTarget::Type::E_RT_WINDOW:
            {
                ret = setRenderTarget(renderTarget->getRenderWindow());
            }
            break;
        case RenderTarget::Type::E_RT_TEXTURE:
            {
                ret = setRenderTarget(renderTarget->getRenderTexture());
            }
            break;
        }
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::resetRenderTarget()
    {
        mCurrentRenderWindow = nullptr;
        mCurrentRenderTexture = nullptr;

        auto lambda = [this]()
        { 
            mD3DDeviceContext->OMSetRenderTargets(0, nullptr, nullptr);
            return T3D_OK;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda);
    }

    //--------------------------------------------------------------------------
    
    TResult D3D11Context::setViewport(const Viewport &viewport)
    {
        return T3D_OK;
    }
    
    //--------------------------------------------------------------------------
    
    TResult D3D11Context::clearColor(const ColorRGB &color)
    {
        TResult ret = T3D_OK;

        if (mCurrentRenderWindow == nullptr && mCurrentRenderTexture == nullptr)
            return ret;

        if (mCurrentRenderTexture != nullptr)
        {
            ret = clearColor(mCurrentRenderTexture, color);
        }
        else if (mCurrentRenderWindow != nullptr)
        {
            ret = clearColor(mCurrentRenderWindow, color);
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::clearColor(RenderWindowPtr window, const ColorRGB &color)
    {
        D3D11RenderWindow *pD3DRenderWindow = static_cast<D3D11RenderWindow*>(window->getRHIRenderWindow().get());
        auto lambda = [this](D3D11RenderWindow *pD3DRenderWindow, ColorRGB color)
        {
            float clr[4];
            clr[0] = color.red();
            clr[1] = color.green();
            clr[2] = color.blue();
            clr[3] = 1.0f;
            mD3DDeviceContext->ClearRenderTargetView(pD3DRenderWindow->D3DRTView, clr);
            return T3D_OK;
        };
        return ENQUEUE_UNIQUE_COMMAND(lambda, pD3DRenderWindow, color);
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::clearColor(RenderTexturePtr texture, const ColorRGB &color)
    {
        D3D11PixelBuffer2D *pD3DPixelBuffer = static_cast<D3D11PixelBuffer2D*>(texture->getPixelBuffer()->getRHIResource().get());
        auto lambda = [this](D3D11PixelBuffer2D *pD3DPixelBuffer, ColorRGB color)
        {
            float clr[4];
            clr[0] = color.red();
            clr[1] = color.green();
            clr[2] = color.blue();
            clr[3] = 1.0f;
            mD3DDeviceContext->ClearRenderTargetView(pD3DPixelBuffer->D3DRTView, clr);
            return T3D_OK;
        };
        return ENQUEUE_UNIQUE_COMMAND(lambda, pD3DPixelBuffer, color);
    }

    //--------------------------------------------------------------------------
    
    TResult D3D11Context::clearDepthStencil(Real depth, uint32_t stencil)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    RHIBlendStatePtr D3D11Context::createBlendState(BlendStatePtr state)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    RHIDepthStencilStatePtr D3D11Context::createDepthStencilState(DepthStencilStatePtr state)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    RHIRasterizerStatePtr D3D11Context::createRasterizerState(RasterizerStatePtr state)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    RHISamplerStatePtr D3D11Context::createSamplerState(SamplerStatePtr state)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    TResult D3D11Context::setBlendState(BlendStatePtr state)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    TResult D3D11Context::setDepthStencilState(DepthStencilStatePtr state)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    TResult D3D11Context::setRasterizerState(RasterizerStatePtr state)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    TResult D3D11Context::setSamplerState(SamplerStatePtr state)
    {
        return T3D_OK;
    }
    
    //--------------------------------------------------------------------------

    RHIVertexDeclarationPtr D3D11Context::createVertexDeclaration(VertexDeclarationPtr decl)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::setVertexDeclaration(VertexDeclarationPtr decl)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    RHIVertexBufferPtr D3D11Context::createVertexBuffer(VertexBufferPtr buffer)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    TResult D3D11Context::setVertexBuffer(VertexBufferPtr buffer)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    RHIIndexBufferPtr D3D11Context::createIndexBuffer(IndexBufferPtr buffer)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    TResult D3D11Context::setIndexBuffer(IndexBufferPtr buffer)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    RHIConstantBufferPtr D3D11Context::createConstantBuffer(ConstantBufferPtr buffer)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    RHIPixelBuffer1DPtr D3D11Context::createPixelBuffer1D(PixelBuffer1DPtr buffer)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    TResult D3D11Context::setPixelBuffer1D(PixelBuffer1DPtr buffer)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    RHIPixelBuffer2DPtr D3D11Context::createPixelBuffer2D(PixelBuffer2DPtr buffer)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    TResult D3D11Context::setPixelBuffer2D(PixelBuffer2DPtr buffer)
    {
        return T3D_OK;
    }
    
    //--------------------------------------------------------------------------
    
    RHIPixelBuffer3DPtr D3D11Context::createPixelBuffer3D(PixelBuffer3DPtr buffer)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    TResult D3D11Context::setPixelBuffer3D(PixelBuffer3DPtr buffer)
    {
        return T3D_OK;
    }
    
    //--------------------------------------------------------------------------
    
    RHIShaderPtr D3D11Context::createVertexShader(ShaderVariantPtr shader)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    TResult D3D11Context::setVertexShader(ShaderVariantPtr shader)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::setVSConstantBuffer(ConstantBufferPtr buffer)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    TResult D3D11Context::setVSSampler(SamplerStatePtr sampler)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    RHIShaderPtr D3D11Context::createPixelShader(ShaderVariantPtr shader)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    TResult D3D11Context::setPixelShader(ShaderVariantPtr shader)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::setPSConstantBuffer(ConstantBufferPtr buffer)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    TResult D3D11Context::setPSSampler(SamplerStatePtr sampler)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    RHIShaderPtr D3D11Context::createHullShader(ShaderVariantPtr shader)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    TResult D3D11Context::setHullShader(ShaderVariantPtr shader)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::setHSConstantBuffer(ConstantBufferPtr buffer)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    TResult D3D11Context::setHSSampler(SamplerStatePtr sampler)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    RHIShaderPtr D3D11Context::createDomainShader(ShaderVariantPtr shader)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    TResult D3D11Context::setDomainShader(ShaderVariantPtr shader)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::setDSConstantBuffer(ConstantBufferPtr buffer)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    TResult D3D11Context::setDSSampler(SamplerStatePtr sampler)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    RHIShaderPtr D3D11Context::createGeometryShader(ShaderVariantPtr shader)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    TResult D3D11Context::setGeometryShader(ShaderVariantPtr shader)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::setGSConstantBuffer(ConstantBufferPtr buffer)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    TResult D3D11Context::setGSSampler(SamplerStatePtr sampler)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    RHIShaderPtr D3D11Context::createComputeShader(ShaderVariantPtr shader)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    TResult D3D11Context::setComputeShader(ShaderVariantPtr shader)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::setCSConstantBuffer(ConstantBufferPtr buffer)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    TResult D3D11Context::setCSSampler(SamplerStatePtr sampler)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    TResult D3D11Context::render()
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::reset()
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::blit(RenderTargetPtr src, RenderTargetPtr dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::blit(TexturePtr src, RenderTargetPtr dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)
    {
        TResult ret = T3D_OK;

        switch (dst->getType())
        {
        case RenderTarget::Type::E_RT_WINDOW:
            {
                D3D11RenderWindow *pDst = static_cast<D3D11RenderWindow*>(dst->getRenderWindow()->getRHIRenderWindow().get());
                auto lambda = [this](Texture *pSrc, D3D11RenderWindow *pDst, Vector3 srcOffset, Vector3 size, Vector3 dstOffset)
                {
                    TResult ret = T3D_OK;

                    ID3D11Resource *pD3DSrc = nullptr;
                    ID3D11ShaderResourceView *pD3DSRV = nullptr;
                    
                    switch (pSrc->getTextureType())
                    {
                    case TEXTURE_TYPE::TT_1D:
                        break;
                    case TEXTURE_TYPE::TT_2D:
                        {
                            Texture2D *pTex2D = static_cast<Texture2D *>(pSrc);
                            D3D11PixelBuffer2D *pD3DPixelBuffer = static_cast<D3D11PixelBuffer2D*>(pTex2D->getPixelBuffer()->getRHIResource().get());
                            pD3DSrc = pD3DPixelBuffer->D3DTexture;
                            pD3DSRV = pD3DPixelBuffer->D3DSRView;
                            // width = pTex2D->getWidth();
                            // height = pTex2D->getHeight();
                        }
                        break;
                    case TEXTURE_TYPE::TT_2D_ARRAY:
                        break;
                    case TEXTURE_TYPE::TT_3D:
                        break;
                    case TEXTURE_TYPE::TT_CUBE:
                        break;
                    case TEXTURE_TYPE::TT_CUBE_ARRAY:
                        break;
                    case TEXTURE_TYPE::TT_RENDER_TEXTURE:
                        {
                            RenderTexture *pTex2D = static_cast<RenderTexture *>(pSrc);
                            D3D11PixelBuffer2D *pD3DPixelBuffer = static_cast<D3D11PixelBuffer2D*>(pTex2D->getPixelBuffer()->getRHIResource().get());
                            pD3DSRV = pD3DPixelBuffer->D3DSRView;
                            // pD3DSrc = pD3DPixelBuffer->D3DTexture;
                            // D3D11_TEXTURE2D_DESC srcTexDesc;
                            // pD3DPixelBuffer->D3DTexture->GetDesc(&srcTexDesc);
                            // D3D11_TEXTURE2D_DESC dstTexDesc;
                            // pDst->D3DBackBuffer->GetDesc(&dstTexDesc);
                            // width = pTex2D->getWidth();
                            // height = pTex2D->getHeight();
                        }
                        break;
                    }
                    if (size == Vector3::ZERO)
                    {
                        // 复制全部
                        ret = blitAll(pD3DSrc, pDst);
                    }
                    else
                    {
                        // 按区域复制
                        ret = blitRegion(pD3DSRV, pDst);
                    }

                    return ret;
                };

                ret = ENQUEUE_UNIQUE_COMMAND(lambda, src.get(), pDst, srcOffset, size, dstOffset);
            }
            break;
        case RenderTarget::Type::E_RT_TEXTURE:
            {
                D3D11PixelBuffer2D *pD3DPixelBuffer = static_cast<D3D11PixelBuffer2D*>(dst->getRenderTexture()->getPixelBuffer()->getRHIResource().get());

                auto lambda = [this]()
                {
                    TResult ret = T3D_OK;

                    return ret;
                };

                ret = ENQUEUE_UNIQUE_COMMAND(lambda);
            }
            break;
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::blit(RenderTargetPtr src, TexturePtr dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::blit(TexturePtr src, TexturePtr dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::copyBuffer(RenderBufferPtr src, RenderBufferPtr dst, size_t srcOffset, size_t size, size_t dstOffset)
    {
        return T3D_OK;
    }
    
#endif
    //--------------------------------------------------------------------------

    TResult D3D11Context::blitRegion(ID3D11ShaderResourceView *pD3DSRV, D3D11RenderWindow *pDst)
    {
        // 设置顶点缓冲区
        UINT stride = sizeof(BlitVertex);
        UINT offset = 0;
        mD3DDeviceContext->IASetVertexBuffers(0, 1, &mBlitVB, &stride, &offset);

        // 设置图元类型
        mD3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

        // 设置输入布局
        mD3DDeviceContext->IASetInputLayout(mBlitLayout);

        // 设置顶点和像素着色器
        mD3DDeviceContext->VSSetShader(mBlitVS, nullptr, 0);
        mD3DDeviceContext->PSSetShader(mBlitPS, nullptr, 0);

        // 设置源纹理为像素着色器的输入        
        mD3DDeviceContext->PSSetShaderResources(0, 1, &pD3DSRV);

        // 设置目标纹理为渲染目标
        mD3DDeviceContext->OMSetRenderTargets(1, &pDst->D3DRTView, nullptr);

        // 绘制全屏四边形
        mD3DDeviceContext->Draw(4, 0);

        ID3D11ShaderResourceView *pNullRSV = nullptr;
        mD3DDeviceContext->PSSetShaderResources(0, 1, &pNullRSV);

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::blitAll(ID3D11Resource *pD3DSrc, D3D11RenderWindow *pDst)
    {        
        mD3DDeviceContext->CopyResource(pDst->D3DBackBuffer, pD3DSrc);
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
}

