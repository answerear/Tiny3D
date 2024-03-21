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
        mCurrentRenderWindow = nullptr;
        mCurrentRenderTexture = nullptr;
        
        D3D_SAFE_RELEASE(mBlitVB)
        D3D_SAFE_RELEASE(mBlitLayout)
        D3D_SAFE_RELEASE(mBlitVS)
        D3D_SAFE_RELEASE(mBlitPS)
        D3D_SAFE_RELEASE(mBlitSamplerState)
        D3D_SAFE_RELEASE(mBlitDSState)
        D3D_SAFE_RELEASE(mD3DDeviceContext)
        D3D_SAFE_RELEASE(mD3DDevice)

        traceDebugInfo();
        
#if defined (T3D_DEBUG)
        D3D_SAFE_RELEASE(mDebugDevice);
#endif
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::init()
    {
        TResult ret = T3D_OK;

        do
        {
            HRESULT hr = S_OK;

            // device flags
            UINT flags = 0;
#if defined (T3D_DEBUG)
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

            D3D_REF_COUNT("D3D11 #1", mD3DDevice);
            D3D_REF_COUNT("D3D11 #2", mD3DDeviceContext);

#if defined (T3D_DEBUG)
            hr = mD3DDevice->QueryInterface(__uuidof(ID3D11Debug), (void **)(&mDebugDevice));
            if (FAILED(hr))
            {
                ret = T3D_ERR_D3D11_CREATE_FAILED;
                T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER,
                    "Create ID3D11Debug object failed ! DX ERROR : %d", hr);
                break;
            }
#endif

            D3D_REF_COUNT("D3D11 #1", mD3DDevice);
            
            // traceDebugInfo("D3D11 D3DObjects trace - After ", __FUNCTION__);
            setupBlitQuad();
        } while (false);

        return ret;
    }
    
    //--------------------------------------------------------------------------

    void D3D11Context::traceDebugInfo(const String &tag, const String &func)
    {
#if defined (T3D_DEBUG)
        if (!tag.empty() || !func.empty())
        {
            String prefix;
            if (!tag.empty())
            {
                prefix = tag;
            }
            if (!func.empty())
            {
                prefix += func;
            }
            
            T3D_LOG_INFO(LOG_TAG_D3D11RENDERER, prefix.c_str())
        }
        
        mDebugDevice->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL | D3D11_RLDO_IGNORE_INTERNAL);
#endif
    }

    //--------------------------------------------------------------------------

    void D3D11Context::setupBlitQuad()
    {
        BlitVertex vertices[4] =
        {
            { Vector3(-1.0f, 1.0f, 0.5f), Vector2(0.0f, 0.0f) },
            { Vector3(1.0f, 1.0f, 0.5f), Vector2(1.0f, 0.0f) },
            { Vector3(-1.0f, -1.0f, 0.5f), Vector2(0.0f, 1.0f) },
            { Vector3(1.0f, -1.0f, 0.5f), Vector2(1.0f, 1.0f) }
        };

        // 创建顶点缓冲区
        D3D11_BUFFER_DESC bd = {};
        bd.Usage = D3D11_USAGE_IMMUTABLE;
        bd.ByteWidth = sizeof(BlitVertex) * 4;
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = 0;
        D3D11_SUBRESOURCE_DATA initData = {};
        initData.pSysMem = vertices;
        HRESULT hr = mD3DDevice->CreateBuffer(&bd, &initData, &mBlitVB);
        T3D_ASSERT(SUCCEEDED(hr), "Create blit vertex buffer !");

        // 编译顶点着色器
        const String vs =
            "struct VS_INPUT\n"
            "{\n"
            "    float3 Pos : POSITION;\n"
            "    float2 Tex : TEXCOORD0;\n"
            "};\n"
            "struct PS_INPUT\n"
            "{\n"
            "    float4 Pos : SV_POSITION;\n"
            "    float2 Tex : TEXCOORD0;\n"
            "};\n"
            "PS_INPUT VS(VS_INPUT input)\n"
            "{\n"
            "    PS_INPUT output;\n"
            "    output.Pos = float4(input.Pos, 1.0f);\n"
            "    output.Tex = input.Tex;\n"
            "    return output;\n"
            "}";

#if defined (T3D_DEBUG)
        UINT shaderCompileFlags = D3DCOMPILE_DEBUG;
#else
        UINT shaderCompileFlags = 0;
#endif
        ID3DBlob *vertexShaderBlob = nullptr;
        ID3DBlob *errorMsgBlob = nullptr;
        hr = D3DCompile(vs.c_str(), vs.length(), "BlitVertexShader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VS", "vs_4_0", shaderCompileFlags, 0, &vertexShaderBlob, &errorMsgBlob);
        T3D_ASSERT(SUCCEEDED(hr), "Compile blit vertex shader !");        
        mD3DDevice->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), nullptr, &mBlitVS);
        D3D_SAFE_RELEASE(errorMsgBlob);
        
        // 编译像素着色器
        const String ps =
            "Texture2D gSrcTexture : register(t0);\n"
            "SamplerState gSampler : register(s0);\n"
            "struct PS_INPUT\n"
            "{\n"
            "    float4 Pos : SV_POSITION;\n"
            "    float2 Tex : TEXCOORD0;\n"
            "};\n"
            "float4 PS(PS_INPUT input) : SV_Target\n"
            "{\n"
            "    float4 color = gSrcTexture.Sample(gSampler, input.Tex);\n"
            "    return color;\n"
            "}";
        
        ID3DBlob* pixelShaderBlob = nullptr;
        hr = D3DCompile(ps.c_str(), ps.length(), "BlitPixelShader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PS", "ps_4_0", shaderCompileFlags, 0, &pixelShaderBlob, &errorMsgBlob);
        String strError;
        if (errorMsgBlob != nullptr)
        {
            strError.assign((const char *)errorMsgBlob->GetBufferPointer(), errorMsgBlob->GetBufferSize());
        }
        T3D_ASSERT(SUCCEEDED(hr), "Compile blit pixel shader !");        
        mD3DDevice->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), nullptr, &mBlitPS);
        T3D_ASSERT(SUCCEEDED(hr), "Create blit pixel shader !");
        D3D_SAFE_RELEASE(errorMsgBlob);
        
        // 创建输入布局
        D3D11_INPUT_ELEMENT_DESC layout[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        hr = mD3DDevice->CreateInputLayout(layout, ARRAYSIZE(layout), vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), &mBlitLayout);
        T3D_ASSERT(SUCCEEDED(hr), "Create blit vertex input layout !");
        
        // 创建 sampler state
        D3D11_SAMPLER_DESC sampDesc = {};
        sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        sampDesc.MinLOD = 0;
        sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
        hr = mD3DDevice->CreateSamplerState(&sampDesc, &mBlitSamplerState);
        T3D_ASSERT(SUCCEEDED(hr), "Create blit sampler state !");

        // 创建 depth stencil state
        D3D11_DEPTH_STENCIL_DESC dsDesc = {};
        dsDesc.DepthEnable             = FALSE;
        dsDesc.StencilEnable           = FALSE;
        dsDesc.DepthFunc               = D3D11_COMPARISON_ALWAYS;
        dsDesc.DepthWriteMask          = D3D11_DEPTH_WRITE_MASK_ALL;
        dsDesc.BackFace.StencilFailOp  = D3D11_STENCIL_OP_KEEP;
        dsDesc.BackFace.StencilPassOp  = D3D11_STENCIL_OP_KEEP;
        dsDesc.BackFace.StencilFunc    = D3D11_COMPARISON_ALWAYS;
        dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        dsDesc.FrontFace.StencilFunc   = D3D11_COMPARISON_ALWAYS;
        hr = mD3DDevice->CreateDepthStencilState(&dsDesc, &mBlitDSState);
        T3D_ASSERT(SUCCEEDED(hr), "Create blit depth stencil state !");
        
        D3D_SAFE_RELEASE(vertexShaderBlob);
        D3D_SAFE_RELEASE(pixelShaderBlob);
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::swapBackBuffer(D3D11RenderWindow *renderWindow)
    {
        auto lambda = [this](const D3D11RenderWindowSafePtr &renderWindow)
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
        return ENQUEUE_UNIQUE_COMMAND(lambda, D3D11RenderWindowSafePtr(renderWindow));
    }

    //--------------------------------------------------------------------------
    
    RHIRenderTargetPtr D3D11Context::createRenderWindow(RenderWindow *renderWindow)
    {
        D3D11RenderWindowPtr d3dRenderWindow = D3D11RenderWindow::create(renderWindow);

        auto lambda = [this](const RenderWindowSafePtr &pRenderWindow, const D3D11RenderWindowSafePtr &pD3DRenderWindow)
        {
            TResult ret = T3D_OK;
            IDXGIDevice *pDXGIDevice = nullptr;
            IDXGIAdapter *pDXGIAdapter = nullptr;
            IDXGIFactory *pDXGIFactory = nullptr;

            // traceDebugInfo("D3D11 D3DObjects trace - #1 ", __FUNCTION__);
            
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

                    // traceDebugInfo("D3D11 D3DObjects trace - #2 ", __FUNCTION__);
                    
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

                // traceDebugInfo("D3D11 D3DObjects trace - #3 ", __FUNCTION__);
                
                hr = pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void **)&pDXGIAdapter);
                if (FAILED(hr))
                {
                    ret = T3D_ERR_D3D11_GET_INTERFACE;
                    T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Get COM for IDXGIAdapter failed ! DX ERROR [%d]", hr);
                    break;
                }

                // traceDebugInfo("D3D11 D3DObjects trace - #4 ", __FUNCTION__);
                
                hr = pDXGIAdapter->GetParent(__uuidof(IDXGIFactory), (void **)&pDXGIFactory);
                if (FAILED(hr))
                {
                    ret = T3D_ERR_D3D11_GET_INTERFACE;
                    T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Get COM for IDXGIFactory failed ! DX ERROR [%d]", hr);
                    break;
                }

                // traceDebugInfo("D3D11 D3DObjects trace - #5 ", __FUNCTION__);
                
                hr = pDXGIFactory->CreateSwapChain(mD3DDevice, &d3dSwapChainDesc, &pD3DRenderWindow->D3DSwapChain);
                if (FAILED(hr))
                {
                    ret = T3D_ERR_D3D11_CREATE_FAILED;
                    T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Create swap chain failed ! DX ERROR [%d]", hr);
                    break;
                }

                // traceDebugInfo("D3D11 D3DObjects trace - #6 ", __FUNCTION__);
                
                // 创建 RenderTargetView
                hr = pD3DRenderWindow->D3DSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&pD3DRenderWindow->D3DBackBuffer));
                if (FAILED(hr))
                {
                    ret = T3D_ERR_D3D11_GET_INTERFACE;
                    T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Get COM for ID3D11Texture2D failed ! DX ERROR [%d]", hr);
                    break;
                }

                // traceDebugInfo("D3D11 D3DObjects trace - #7 ", __FUNCTION__);
                
                hr = mD3DDevice->CreateRenderTargetView(pD3DRenderWindow->D3DBackBuffer, nullptr, &pD3DRenderWindow->D3DRTView);
                if (FAILED(hr))
                {
                    ret = T3D_ERR_D3D11_CREATE_FAILED;
                    T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Create render target view failed ! DX ERROR [%d]", hr);
                    break;
                }

                // traceDebugInfo("D3D11 D3DObjects trace - #8 ", __FUNCTION__);
                
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

                hr = mD3DDevice->CreateTexture2D(&d3dTexDesc, nullptr, &pD3DRenderWindow->D3DDSBuffer);
                if (FAILED(hr))
                {
                    ret = T3D_ERR_D3D11_CREATE_FAILED;
                    T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Create texture 2D failed ! DX ERROR [%d]", hr);
                    break;
                }

                // traceDebugInfo("D3D11 D3DObjects trace - #9 ", __FUNCTION__);
                
                hr = mD3DDevice->CreateDepthStencilView(pD3DRenderWindow->D3DDSBuffer, nullptr, &pD3DRenderWindow->D3DDSView);
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

            // traceDebugInfo("D3D11 D3DObjects trace - #10 ", __FUNCTION__);
            
            return ret;
        };

        TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, RenderWindowSafePtr(renderWindow), D3D11RenderWindowSafePtr(d3dRenderWindow));
        
        if (T3D_FAILED(ret))
        {
            d3dRenderWindow = nullptr;
        }

        return d3dRenderWindow;
    }

    //--------------------------------------------------------------------------
    
    RHIPixelBuffer2DPtr D3D11Context::createRenderTexture(PixelBuffer2D *buffer)
    {
        D3D11PixelBuffer2DPtr d3dPixelBuffer = D3D11PixelBuffer2D::create();

        auto lambda = [this](const PixelBuffer2DSafePtr &buffer, const D3D11PixelBuffer2DSafePtr &d3dPixelBuffer)
        {
            TResult ret = T3D_OK;

            do
            {
                // 获取支持的 MSAA
                UINT uMSAACount = 1;//buffer->getDescriptor().sampleDesc.Count;
                UINT uMSAAQuality = 0;//buffer->getDescriptor().sampleDesc.Quality;

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
                texDesc.SampleDesc.Count = 1;
                texDesc.SampleDesc.Quality = 0;
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
                D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
                rtvDesc.Format = texDesc.Format;
                rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
                hr = mD3DDevice->CreateRenderTargetView(d3dPixelBuffer->D3DTexture, &rtvDesc, &d3dPixelBuffer->D3DRTView);
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
                D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
                dsvDesc.Format = depthStencilDesc.Format;
                dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
                dsvDesc.Texture2D.MipSlice = 0;
                hr = mD3DDevice->CreateDepthStencilView(d3dPixelBuffer->D3DDSTexture, &dsvDesc, &d3dPixelBuffer->D3DDSView);
                if (FAILED(hr))
                {
                    // 错误
                    ret = T3D_ERR_D3D11_CREATE_DEPTH_STENCIL_VIEW;
                    T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "CreateDepthStencilView failed when create render texture ! DX ERROR [%d]", hr);
                    break;
                }

                // 创建着色器资源视图
                D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
                srvDesc.Format = texDesc.Format;
                srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
                srvDesc.Texture2D.MostDetailedMip = 0;
                srvDesc.Texture2D.MipLevels = 1;
                hr = mD3DDevice->CreateShaderResourceView(d3dPixelBuffer->D3DTexture, &srvDesc, &d3dPixelBuffer->D3DSRView);
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
        
        TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, PixelBuffer2DSafePtr(buffer), D3D11PixelBuffer2DSafePtr(d3dPixelBuffer));
        if (T3D_FAILED(ret))
        {
            d3dPixelBuffer = nullptr;
        }
        
        return d3dPixelBuffer;
    }
    
    //--------------------------------------------------------------------------
    
    TResult D3D11Context::setRenderTarget(RenderWindow *renderWindow)
    {
        D3D11RenderWindow *pD3DRenderWindow = static_cast<D3D11RenderWindow*>(renderWindow->getRHIRenderWindow().get());
        auto lambda = [this](const D3D11RenderWindowSafePtr &pD3DRenderWindow)
        {
            mD3DDeviceContext->OMSetRenderTargets(1, &pD3DRenderWindow->D3DRTView, pD3DRenderWindow->D3DDSView);
            return T3D_OK;
        };
        
        TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, D3D11RenderWindowSafePtr(pD3DRenderWindow));
        
        if (T3D_SUCCEEDED(ret))
        {
            mCurrentRenderWindow = renderWindow;
        }
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::setRenderTarget(RenderTexture *renderTexture)
    {
        D3D11PixelBuffer2D *pD3DPixelBuffer = static_cast<D3D11PixelBuffer2D*>(renderTexture->getPixelBuffer()->getRHIResource().get());
        
        auto lambda = [this](const D3D11PixelBuffer2DSafePtr &pD3DPixelBuffer)
        {
            mD3DDeviceContext->OMSetRenderTargets(1, &pD3DPixelBuffer->D3DRTView, pD3DPixelBuffer->D3DDSView);
            return T3D_OK;
        };

        TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, D3D11PixelBuffer2DSafePtr(pD3DPixelBuffer));
        
        if (T3D_SUCCEEDED(ret))
        {
            mCurrentRenderTexture = renderTexture;
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::setRenderTarget(RenderTarget *renderTarget)
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
        Real width, height;
        
        if (mCurrentRenderTexture != nullptr)
        {
            width = Real(mCurrentRenderTexture->getWidth());
            height = Real(mCurrentRenderTexture->getHeight());
        }
        else if (mCurrentRenderWindow != nullptr)
        {
            width = Real(mCurrentRenderWindow->getDescriptor().Width);
            height = Real(mCurrentRenderWindow->getDescriptor().Height);
        }
        else
        {
            return T3D_OK;
        }
        
        auto lambda = [this](const Viewport &vp, Real width, Real height)
        {
            D3D11_VIEWPORT d3dViewport = {};
            d3dViewport.TopLeftX = vp.Left * width;
            d3dViewport.TopLeftY = vp.Top * height;
            d3dViewport.Width = width;
            d3dViewport.Height = height;
            d3dViewport.MinDepth = 0.0f;
            d3dViewport.MaxDepth = 1.0f;
            mD3DDeviceContext->RSSetViewports(1, &d3dViewport);
            return T3D_OK;
        };
        
        return ENQUEUE_UNIQUE_COMMAND(lambda, viewport, width, height);
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

    TResult D3D11Context::clearColor(RenderWindow *window, const ColorRGB &color)
    {
        D3D11RenderWindow *pD3DRenderWindow = static_cast<D3D11RenderWindow*>(window->getRHIRenderWindow().get());
        auto lambda = [this](const D3D11RenderWindowSafePtr &pD3DRenderWindow, const ColorRGB &color)
        {
            float clr[4];
            clr[0] = color.red();
            clr[1] = color.green();
            clr[2] = color.blue();
            clr[3] = 1.0f;
            mD3DDeviceContext->ClearRenderTargetView(pD3DRenderWindow->D3DRTView, clr);
            return T3D_OK;
        };
        return ENQUEUE_UNIQUE_COMMAND(lambda, D3D11RenderWindowSafePtr(pD3DRenderWindow), color);
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::clearColor(RenderTexture *texture, const ColorRGB &color)
    {
        D3D11PixelBuffer2D *pD3DPixelBuffer = static_cast<D3D11PixelBuffer2D*>(texture->getPixelBuffer()->getRHIResource().get());
        auto lambda = [this](const D3D11PixelBuffer2DSafePtr &pD3DPixelBuffer, const ColorRGB &color)
        {
            float clr[4];
            clr[0] = color.red();
            clr[1] = color.green();
            clr[2] = color.blue();
            clr[3] = 1.0f;
            mD3DDeviceContext->ClearRenderTargetView(pD3DPixelBuffer->D3DRTView, clr);
            return T3D_OK;
        };
        return ENQUEUE_UNIQUE_COMMAND(lambda, D3D11PixelBuffer2DSafePtr(pD3DPixelBuffer), color);
    }

    //--------------------------------------------------------------------------
    
    TResult D3D11Context::clearDepthStencil(Real depth, uint32_t stencil)
    {
        TResult ret = T3D_OK;

        if (mCurrentRenderWindow == nullptr && mCurrentRenderTexture == nullptr)
            return ret;

        if (mCurrentRenderTexture != nullptr)
        {
            ret = clearDepthStencil(mCurrentRenderTexture, depth, stencil);
        }
        else if (mCurrentRenderWindow != nullptr)
        {
            ret = clearDepthStencil(mCurrentRenderWindow, depth, stencil);
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::clearDepthStencil(RenderWindow *window, const Real &depth, uint8_t stencil)
    {
        D3D11RenderWindow *pD3DRenderWindow = static_cast<D3D11RenderWindow*>(window->getRHIRenderWindow().get());
        auto lambda = [this](const D3D11RenderWindowSafePtr &pD3DRenderWindow, const Real &depth, uint8_t stencil)
        {
            mD3DDeviceContext->ClearDepthStencilView(pD3DRenderWindow->D3DDSView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, depth, stencil);
            return T3D_OK;
        };
        return ENQUEUE_UNIQUE_COMMAND(lambda, D3D11RenderWindowSafePtr(pD3DRenderWindow), depth, stencil);
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::clearDepthStencil(RenderTexture *texture, const Real &depth, uint8_t stencil)
    {
        D3D11PixelBuffer2D *pD3DPixelBuffer = static_cast<D3D11PixelBuffer2D*>(texture->getPixelBuffer()->getRHIResource().get());
        auto lambda = [this](const D3D11PixelBuffer2DSafePtr &pD3DPixelBuffer, const Real &depth, uint8_t stencil)
        {
            mD3DDeviceContext->ClearDepthStencilView(pD3DPixelBuffer->D3DDSView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, depth, stencil);
            return T3D_OK;
        };
        return ENQUEUE_UNIQUE_COMMAND(lambda, D3D11PixelBuffer2DSafePtr(pD3DPixelBuffer), depth, stencil);
    }

    //--------------------------------------------------------------------------
    
    RHIBlendStatePtr D3D11Context::createBlendState(BlendState *state)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    RHIDepthStencilStatePtr D3D11Context::createDepthStencilState(DepthStencilState*state)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    RHIRasterizerStatePtr D3D11Context::createRasterizerState(RasterizerState *state)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    RHISamplerStatePtr D3D11Context::createSamplerState(SamplerState *state)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    TResult D3D11Context::setBlendState(BlendState *state)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    TResult D3D11Context::setDepthStencilState(DepthStencilState *state)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    TResult D3D11Context::setRasterizerState(RasterizerState *state)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    RHIVertexDeclarationPtr D3D11Context::createVertexDeclaration(VertexDeclaration *decl)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::setVertexDeclaration(VertexDeclaration *decl)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    RHIVertexBufferPtr D3D11Context::createVertexBuffer(VertexBuffer *buffer)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    TResult D3D11Context::setVertexBuffers(uint32_t startSlot, uint32_t numOfBuffers, VertexBuffer * const *buffers, const uint32_t *strides, const uint32_t *offsets)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    RHIIndexBufferPtr D3D11Context::createIndexBuffer(IndexBuffer *buffer)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    TResult D3D11Context::setIndexBuffer(IndexBuffer *buffer)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    RHIConstantBufferPtr D3D11Context::createConstantBuffer(ConstantBuffer *buffer)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    RHIPixelBuffer1DPtr D3D11Context::createPixelBuffer1D(PixelBuffer1D *buffer)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    RHIPixelBuffer2DPtr D3D11Context::createPixelBuffer2D(PixelBuffer2D *buffer)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    RHIPixelBuffer3DPtr D3D11Context::createPixelBuffer3D(PixelBuffer3D *buffer)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    RHIShaderPtr D3D11Context::createVertexShader(ShaderVariant *shader)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    TResult D3D11Context::setVertexShader(ShaderVariant *shader)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::setVSConstantBuffer(uint32_t startSlot, uint32_t numOfBuffers, ConstantBuffer * const *buffers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::setVSPixelBuffer(uint32_t startSlot, uint32_t numOfBuffers, PixelBuffer * const *buffers)
    {
        return T3D_OK;
    }
    
    //--------------------------------------------------------------------------
    
    TResult D3D11Context::setVSSampler(uint32_t startSlot, uint32_t numOfSamplers, SamplerState * const *samplers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    RHIShaderPtr D3D11Context::createPixelShader(ShaderVariant *shader)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    TResult D3D11Context::setPixelShader(ShaderVariant *shader)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::setPSConstantBuffer(uint32_t startSlot, uint32_t numOfBuffers, ConstantBuffer * const *buffers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::setPSPixelBuffer(uint32_t startSlot, uint32_t numOfBuffers, PixelBuffer * const *buffers)
    {
        return T3D_OK;
    }
    
    //--------------------------------------------------------------------------
    
    TResult D3D11Context::setPSSampler(uint32_t startSlot, uint32_t numOfSamplers, SamplerState * const *samplers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    RHIShaderPtr D3D11Context::createHullShader(ShaderVariant *shader)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    TResult D3D11Context::setHullShader(ShaderVariant *shader)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::setHSConstantBuffer(uint32_t startSlot, uint32_t numOfBuffers, ConstantBuffer * const *buffers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::setHSPixelBuffer(uint32_t startSlot, uint32_t numOfBuffers, PixelBuffer * const *buffers)
    {
        return T3D_OK;
    }
    
    //--------------------------------------------------------------------------
    
    TResult D3D11Context::setHSSampler(uint32_t startSlot, uint32_t numOfSamplers, SamplerState * const *samplers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    RHIShaderPtr D3D11Context::createDomainShader(ShaderVariant *shader)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    TResult D3D11Context::setDomainShader(ShaderVariant *shader)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::setDSConstantBuffer(uint32_t startSlot, uint32_t numOfBuffers, ConstantBuffer * const *buffers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::setDSPixelBuffer(uint32_t startSlot, uint32_t numOfBuffers, PixelBuffer * const *buffers)
    {
        return T3D_OK;
    }
    
    //--------------------------------------------------------------------------
    
    TResult D3D11Context::setDSSampler(uint32_t startSlot, uint32_t numOfSamplers, SamplerState * const *samplers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    RHIShaderPtr D3D11Context::createGeometryShader(ShaderVariant *shader)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    TResult D3D11Context::setGeometryShader(ShaderVariant *shader)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::setGSConstantBuffer(uint32_t startSlot, uint32_t numOfBuffers, ConstantBuffer * const *buffers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::setGSPixelBuffer(uint32_t startSlot, uint32_t numOfBuffers, PixelBuffer * const *buffers)
    {
        return T3D_OK;
    }
    
    //--------------------------------------------------------------------------
    
    TResult D3D11Context::setGSSampler(uint32_t startSlot, uint32_t numOfSamplers, SamplerState * const *samplers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    RHIShaderPtr D3D11Context::createComputeShader(ShaderVariant *shader)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    TResult D3D11Context::setComputeShader(ShaderVariant *shader)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::setCSConstantBuffer(uint32_t startSlot, uint32_t numOfBuffers, ConstantBuffer * const *buffers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::setCSPixelBuffer(uint32_t startSlot, uint32_t numOfBuffers, PixelBuffer * const *buffers)
    {
        return T3D_OK;
    }
    
    //--------------------------------------------------------------------------
    
    TResult D3D11Context::setCSSampler(uint32_t startSlot, uint32_t numOfSamplers, SamplerState * const *samplers)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::compileShader(ShaderVariant *shader)
    {
        TResult ret = T3D_OK;
        
        do
        {
            String profile;
        
            switch (shader->getShaderStage())
            {
            case SHADER_STAGE::kVertex:
                profile = "vs_5_0";
                break;
            case SHADER_STAGE::kPixel:
                profile = "ps_5_0";
                break;
            case SHADER_STAGE::kCompute:
                profile = "cs_5_0";
                break;
            case SHADER_STAGE::kGeometry:
                profile = "gs_5_0";
                break;
            case SHADER_STAGE::kHull:
                profile = "hs_5_0";
                break;
            case SHADER_STAGE::kDomain:
                profile = "ds_5_0";
                break;
            case SHADER_STAGE::kUnknown:
            default:
                T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Invalid shader stage [%d] !", shader->getShaderStage());
                ret = T3D_ERR_INVALID_PARAM;
                break;
            }

            if (T3D_FAILED(ret))
            {
                break;
            }
            
            size_t bytesLength = 0;
            const char *bytes = shader->getBytesCode(bytesLength);

            ID3DBlob *shaderBlob = nullptr;
            ID3DBlob *errorBlob = nullptr;
            HRESULT hr = D3DCompile(bytes, bytesLength, nullptr, nullptr, nullptr, "main", profile.c_str(), 0, 0, &shaderBlob, &errorBlob);
            if (FAILED(hr))
            {
                ret = T3D_ERR_D3D11_COMPILE_SHADER;
                String error;
                if (errorBlob != nullptr)
                {
                    error.assign(static_cast<const char *>(errorBlob->GetBufferPointer()), errorBlob->GetBufferSize());
                }
                T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Compile shader failed ! DX ERROR : %d (%s)", hr, error.c_str());
                D3D_SAFE_RELEASE(shaderBlob);
                D3D_SAFE_RELEASE(errorBlob);
                break;
            }

            // 把编译后的设置到 shader 里面
            shader->setBytesCode(static_cast<const char*>(shaderBlob->GetBufferPointer()), shaderBlob->GetBufferSize());
            D3D_SAFE_RELEASE(shaderBlob);
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::reflectShaderAllBindings(ShaderVariantPtr shader, ShaderConstantBindings &constantBindings, ShaderTexSamplerBindings texSamplerBindings)
    {
        TResult ret = T3D_OK;

        do
        {
            // 创建 shader 字节码 D3D11 对象
            size_t bytesLength = 0;
            const char *bytes = shader->getBytesCode(bytesLength);
            ID3DBlob *pShaderBlob = nullptr;
            HRESULT hr = D3DCreateBlob(bytesLength, &pShaderBlob);
            if (FAILED(hr))
            {
                ret = T3D_ERR_D3D11_CREATE_BLOB;
                T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Create blob with shader code failed ! DX ERROR : %d", hr);
                break;
            }

            // 反射 shader
            void *pData = pShaderBlob->GetBufferPointer();
            memcpy(pData, bytes, bytesLength);
            ID3D11ShaderReflection *pReflection = nullptr;
            hr = D3DReflect(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&pReflection);
            if (FAILED(hr))
            {
                ret = T3D_ERR_D3D11_SHADER_REFLECTION;
                T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Reflect shadef failed ! DX ERROR : %d", hr);
                D3D_SAFE_RELEASE(pShaderBlob);
                break;
            }

            // 获取 shader 信息
            D3D11_SHADER_DESC shaderDesc = {};
            hr = pReflection->GetDesc(&shaderDesc);
            if (FAILED(hr))
            {
                ret = T3D_ERR_D3D11_GET_SHADER_DESC;
                T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Get shader description failed ! DX ERROR : %d", hr);
                break;
            }

            for (UINT i = 0; i < shaderDesc.BoundResources; ++i)
            {
                D3D11_SHADER_INPUT_BIND_DESC bindDesc;
                pReflection->GetResourceBindingDesc(i, &bindDesc);

                switch (bindDesc.Type)
                {
                case D3D_SIT_CBUFFER:   // 常量缓冲区
                    {
                        ShaderConstantBinding constBinding;
                        constBinding.name = bindDesc.Name;
                        constBinding.binding = bindDesc.BindPoint;

                        uint32_t size = 0;
                    
                        // 获取常量缓冲区反射对象
                        ID3D11ShaderReflectionConstantBuffer *pConstBufferReflection = pReflection->GetConstantBufferByName(bindDesc.Name);
                        D3D11_SHADER_BUFFER_DESC bufferDesc;
                        pConstBufferReflection->GetDesc(&bufferDesc);
                    
                        // 遍历常量缓冲区中的所有常量
                        for (UINT j = 0; j < bufferDesc.Variables; ++j)
                        {
                            // 获取常量反射对象
                            ID3D11ShaderReflectionVariable* pVariableReflection = pConstBufferReflection->GetVariableByIndex(j);

                            // 获取常量描述
                            D3D11_SHADER_VARIABLE_DESC variableDesc;
                            pVariableReflection->GetDesc(&variableDesc);

                            ShaderVariableBinding varBinding;
                            varBinding.name = variableDesc.Name;
                            varBinding.offset = variableDesc.StartOffset;
                            varBinding.size = variableDesc.Size;
                            size += varBinding.size;
                            constBinding.variables.emplace(varBinding.name, varBinding);
                        }

                        constBinding.size = size;
                        constantBindings.emplace(constBinding.name, constBinding);
                    }
                    break;
                case D3D_SIT_TEXTURE:   // 纹理
                    {
                        String name = bindDesc.Name;

                        auto itr = texSamplerBindings.find(name);
                        if (itr == texSamplerBindings.end())
                        {
                            // 没有，则新建一个
                            ShaderTexSamplerBinding texSamplerBinding;
                            texSamplerBinding.texBinding.name = name;
                            texSamplerBinding.texBinding.binding = bindDesc.BindPoint;
                            texSamplerBinding.texBinding.bindingCount = bindDesc.BindCount;
                            texSamplerBinding.texBinding.texType = D3D11Mapping::get(bindDesc.Dimension);

                            texSamplerBindings.emplace(name, texSamplerBinding);
                        }
                        else
                        {
                            // 已有，用已有的
                            ShaderTexSamplerBinding &texSamplerBinding = itr->second;
                            texSamplerBinding.texBinding.name = name;
                            texSamplerBinding.texBinding.binding = bindDesc.BindPoint;
                            texSamplerBinding.texBinding.bindingCount = bindDesc.BindCount;
                            texSamplerBinding.texBinding.texType = D3D11Mapping::get(bindDesc.Dimension);
                        }
                    }
                    break;
                case D3D_SIT_SAMPLER:   // 纹理采样器
                    {
                        String name = bindDesc.Name;

                        if (!StringUtil::startsWith(name, "sampler_"))
                        {
                            // sampler 一定要以 sampler_ 开头
                            ret = T3D_ERR_D3D11_INVALID_SHADER_SAMPLER_NAME;
                            T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Invalid shader sampler name (%s) ! It must start with 'sampler_' !", name.c_str());
                            break;
                        }

                        String key = name.substr(8);
                        auto itr = texSamplerBindings.find(key);
                        if (itr == texSamplerBindings.end())
                        {
                            // 没有，则新建一个
                            ShaderTexSamplerBinding texSamplerBinding;
                            texSamplerBinding.samplerBinding.name = name;
                            texSamplerBinding.samplerBinding.binding = bindDesc.BindPoint;

                            texSamplerBindings.emplace(key, texSamplerBinding);
                        }
                        else
                        {
                            // 已有，用已有的
                            ShaderTexSamplerBinding &texSamplerBinding = itr->second;
                            texSamplerBinding.samplerBinding.name = name;
                            texSamplerBinding.samplerBinding.binding = bindDesc.BindPoint;
                        }
                    }
                    break;
                }

                if (T3D_FAILED(ret))
                {
                    break;
                }
            }
        } while (false);
        
        return ret;
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
                auto lambda = [this](const TextureSafePtr &pSrc, const D3D11RenderWindowSafePtr &pDst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 &dstOffset)
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
                            Texture2D *pTex2D = static_cast<Texture2D *>(pSrc.get());
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
                            RenderTexture *pTex2D = static_cast<RenderTexture *>(pSrc.get());
                            D3D11PixelBuffer2D *pD3DPixelBuffer = static_cast<D3D11PixelBuffer2D*>(pTex2D->getPixelBuffer()->getRHIResource().get());
                            pD3DSRV = pD3DPixelBuffer->D3DSRView;
                            pD3DSrc = pD3DPixelBuffer->D3DTexture;
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
                        ret = blitRegion(pD3DSRV, pDst, srcOffset, size, dstOffset);
                    }

                    return ret;
                };

                ret = ENQUEUE_UNIQUE_COMMAND(lambda, TextureSafePtr(src), D3D11RenderWindowSafePtr(pDst), srcOffset, size, dstOffset);
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

    //--------------------------------------------------------------------------

    TResult D3D11Context::blitRegion(ID3D11ShaderResourceView *pD3DSRV, D3D11RenderWindow *pDst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 &dstOffset)
    {
        ID3D11RenderTargetView *pCurRTV = nullptr;
        ID3D11DepthStencilView *pCurDSV = nullptr;
        mD3DDeviceContext->OMGetRenderTargets(1, &pCurRTV, &pCurDSV);

        // ID3D11RasterizerState *pCurRState = nullptr;
        // mD3DDeviceContext->RSGetState(&pCurRState);

        ID3D11DepthStencilState *pCurDSState = nullptr;
        UINT curStencilRef = 0;
        mD3DDeviceContext->OMGetDepthStencilState(&pCurDSState, &curStencilRef);

        UINT numOfViewports = 1;
        D3D11_VIEWPORT originalVP;
        mD3DDeviceContext->RSGetViewports(&numOfViewports, &originalVP);

        mD3DDeviceContext->OMSetRenderTargets(1, &pDst->D3DRTView, pDst->D3DDSView);

        // rasterizer state
        // mD3DDeviceContext->RSSetState(mBlitRasterState);

        // depth stencil state
        mD3DDeviceContext->OMSetDepthStencilState(mBlitDSState, 0);

        D3D11_TEXTURE2D_DESC d3dTexDesc;
        pDst->D3DBackBuffer->GetDesc(&d3dTexDesc);
                    
        D3D11_VIEWPORT viewport = {};
        viewport.TopLeftX = dstOffset.x();
        viewport.TopLeftY = dstOffset.y();
        viewport.Width = size.x();
        viewport.Height = size.y();
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;
        mD3DDeviceContext->RSSetViewports(1, &viewport);

        // 设置顶点缓冲区
        UINT stride = sizeof(BlitVertex);
        UINT offset = 0;
        mD3DDeviceContext->IASetVertexBuffers(0, 1, &mBlitVB, &stride, &offset);

        // 设置图元类型
        mD3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

        // 设置输入布局
        mD3DDeviceContext->IASetInputLayout(mBlitLayout);

        // 设置源纹理为像素着色器的输入
        mD3DDeviceContext->PSSetShaderResources(0, 1, &pD3DSRV);
        
        // 设置纹理采样
        mD3DDeviceContext->PSSetSamplers(0, 1, &mBlitSamplerState);

        // 设置混合状态
        // mD3DDeviceContext->OMSetBlendState(mBlitBlendState, );
        
        // 设置顶点和像素着色器
        mD3DDeviceContext->VSSetShader(mBlitVS, nullptr, 0);
        mD3DDeviceContext->PSSetShader(mBlitPS, nullptr, 0);

        // 设置目标纹理为渲染目标
        // mD3DDeviceContext->OMSetRenderTargets(1, &pDst->D3DRTView, nullptr);

        // 绘制全屏四边形
        mD3DDeviceContext->Draw(4, 0);

        ID3D11ShaderResourceView *pNullRSV = nullptr;
        mD3DDeviceContext->PSSetShaderResources(0, 1, &pNullRSV);
        
        mD3DDeviceContext->OMSetRenderTargets(1, &pCurRTV, pCurDSV);
        mD3DDeviceContext->RSSetViewports(1, &originalVP);
        mD3DDeviceContext->OMSetDepthStencilState(pCurDSState, curStencilRef);

        D3D_SAFE_RELEASE(pCurDSV);
        D3D_SAFE_RELEASE(pCurRTV);
        
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

