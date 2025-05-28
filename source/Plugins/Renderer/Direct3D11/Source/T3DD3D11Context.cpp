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
#include "T3DD3D11RenderState.h"
#include "T3DD3D11Shader.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    #define TINY3D_CBUFFER_PER_DRAW         "Tiny3DPerDraw"
    #define TINY3D_CBUFFER_PER_DRAW_LEN     13
    #define TINY3D_CBUFFER_PER_FRAME        "Tiny3DPerFrame"
    #define TINY3D_CBUFFER_PER_FRAME_LEN    14
    
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
        mCurrentDepthStencil = nullptr;

        D3D_SAFE_RELEASE(mBlitVB)
        D3D_SAFE_RELEASE(mBlitLayout)
        D3D_SAFE_RELEASE(mBlitVS)
        D3D_SAFE_RELEASE(mBlitPS)
        D3D_SAFE_RELEASE(mBlitSamplerState)
        D3D_SAFE_RELEASE(mBlitBState)
        D3D_SAFE_RELEASE(mBlitDSState)
        D3D_SAFE_RELEASE(mBlitRState)
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
                    "Create ID3D11Device object failed ! DX ERROR [%d]", hr);
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
                    "Create ID3D11Debug object failed ! DX ERROR [%d]", hr);
                break;
            }
#endif

            D3D_REF_COUNT("D3D11 #1", mD3DDevice);
            
            // traceDebugInfo("D3D11 D3DObjects trace - After ", __FUNCTION__);
            setupBlitQuad();
            setupInternalCBuffers();
        } while (false);

        return ret;
    }
    
    //--------------------------------------------------------------------------

    void D3D11Context::setupInternalCBuffers()
    {
        // // constant buffer per frame
        // D3D11_BUFFER_DESC d3dDesc;
        // memset(&d3dDesc, 0, sizeof(d3dDesc));
        // d3dDesc.Usage = D3D11_USAGE_DYNAMIC;
        // d3dDesc.ByteWidth = sizeof(CBufferPerFrame);
        // d3dDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        // d3dDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        //
        // // 创建顶点缓冲区子资源数据
        // D3D11_SUBRESOURCE_DATA initData;
        // memset(&initData, 0, sizeof(initData));
        // initData.pSysMem = &mCBufferPerFrame;
        // initData.SysMemPitch = 0;
        // initData.SysMemSlicePitch = 0;
        //
        // ID3D11Buffer *pD3DBuffer = nullptr;
        // HRESULT hr = mD3DDevice->CreateBuffer(&d3dDesc, &initData, &pD3DBuffer);
        // T3D_ASSERT(SUCCEEDED(hr));
        // mPerFrameCBuffer = pD3DBuffer;
        //
        // // constant buffer per draw
        // memset(&d3dDesc, 0, sizeof(d3dDesc));
        // d3dDesc.Usage = D3D11_USAGE_DYNAMIC;
        // d3dDesc.ByteWidth = sizeof(CBufferPerDraw);
        // d3dDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        // d3dDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        //
        // memset(&initData, 0, sizeof(initData));
        // initData.pSysMem = &mCBufferPerDraw;
        // initData.SysMemPitch = 0;
        // initData.SysMemSlicePitch = 0;
        // hr = mD3DDevice->CreateBuffer(&d3dDesc, &initData, &pD3DBuffer);
        // T3D_ASSERT(SUCCEEDED(hr));
        // mPerDrawCBuffer = pD3DBuffer;
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
        D3D11_BUFFER_DESC bd;
        memset(&bd, 0, sizeof(bd));
        bd.Usage = D3D11_USAGE_IMMUTABLE;
        bd.ByteWidth = sizeof(BlitVertex) * 4;
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = 0;
        D3D11_SUBRESOURCE_DATA initData = {};
        initData.pSysMem = vertices;
        HRESULT hr = mD3DDevice->CreateBuffer(&bd, &initData, &mBlitVB);
        T3D_ASSERT(SUCCEEDED(hr));

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
        T3D_ASSERT(SUCCEEDED(hr));        
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
        T3D_ASSERT(SUCCEEDED(hr));        
        mD3DDevice->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), nullptr, &mBlitPS);
        T3D_ASSERT(SUCCEEDED(hr));
        D3D_SAFE_RELEASE(errorMsgBlob);
        
        // 创建输入布局
        D3D11_INPUT_ELEMENT_DESC layout[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        hr = mD3DDevice->CreateInputLayout(layout, ARRAYSIZE(layout), vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), &mBlitLayout);
        T3D_ASSERT(SUCCEEDED(hr));
        
        // 创建 sampler state
        D3D11_SAMPLER_DESC sampDesc;
        memset(&sampDesc, 0, sizeof(sampDesc));
        sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        sampDesc.MinLOD = 0;
        sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
        hr = mD3DDevice->CreateSamplerState(&sampDesc, &mBlitSamplerState);
        T3D_ASSERT(SUCCEEDED(hr));

        // 创建 blend state
        D3D11_BLEND_DESC blendDesc = {};
        blendDesc.AlphaToCoverageEnable = FALSE; // 禁用Alpha到覆盖
        blendDesc.IndependentBlendEnable = FALSE; // 禁用独立混合
        // 设置RenderTarget[0]的混合状态
        blendDesc.RenderTarget[0].BlendEnable = TRUE; // 启用混合
        blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA; // 源颜色混合因子：源Alpha
        blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA; // 目标颜色混合因子：1 - 源Alpha
        blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD; // 颜色混合操作：加法
        blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE; // 源Alpha混合因子：1
        blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO; // 目标Alpha混合因子：0
        blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD; // Alpha混合操作：加法
        blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL; // 启用所有颜色通道
        hr = mD3DDevice->CreateBlendState(&blendDesc, &mBlitBState);
        T3D_ASSERT(SUCCEEDED(hr));

        // 创建 depth stencil state
        D3D11_DEPTH_STENCIL_DESC dsDesc;
        memset(&dsDesc, 0, sizeof(dsDesc));
        dsDesc.DepthEnable             = FALSE;
        dsDesc.StencilEnable           = FALSE;
        dsDesc.DepthFunc               = D3D11_COMPARISON_LESS;
        dsDesc.DepthWriteMask          = D3D11_DEPTH_WRITE_MASK_ALL;
        dsDesc.BackFace.StencilFailOp  = D3D11_STENCIL_OP_KEEP;
        dsDesc.BackFace.StencilPassOp  = D3D11_STENCIL_OP_KEEP;
        dsDesc.BackFace.StencilFunc    = D3D11_COMPARISON_ALWAYS;
        dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        dsDesc.FrontFace.StencilFunc   = D3D11_COMPARISON_ALWAYS;
        hr = mD3DDevice->CreateDepthStencilState(&dsDesc, &mBlitDSState);
        T3D_ASSERT(SUCCEEDED(hr));

        // 创建 rasterizer state
        D3D11_RASTERIZER_DESC rasterizerDesc;
        memset(&rasterizerDesc, 0, sizeof(rasterizerDesc));
        rasterizerDesc.CullMode = D3D11_CULL_BACK;  // 设置剔除模式为背面剔除
        rasterizerDesc.FillMode = D3D11_FILL_SOLID; // 设置填充模式为实心
        rasterizerDesc.FrontCounterClockwise = FALSE; // 设置正面为顺时针
        rasterizerDesc.DepthBias = 0;
        rasterizerDesc.DepthBiasClamp = 0.0f;
        rasterizerDesc.SlopeScaledDepthBias = 0.0f;
        rasterizerDesc.DepthClipEnable = true; // 启用深度裁剪
        rasterizerDesc.ScissorEnable = false; // 禁用剪裁测试
        rasterizerDesc.MultisampleEnable = false; // 禁用多采样
        rasterizerDesc.AntialiasedLineEnable = false; // 禁用抗锯齿线条
        hr = mD3DDevice->CreateRasterizerState(&rasterizerDesc, &mBlitRState);
        T3D_ASSERT(SUCCEEDED(hr));
        
        D3D_SAFE_RELEASE(vertexShaderBlob);
        D3D_SAFE_RELEASE(pixelShaderBlob);
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::swapBackBuffer(D3D11RenderWindow *renderWindow)
    {
        auto lambda = [this](const D3D11RenderWindowPtr &renderWindow)
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
        
        return ENQUEUE_UNIQUE_COMMAND(lambda, D3D11RenderWindowPtr(renderWindow));
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::setConstantBuffer(uint32_t startSlot, const Buffer &buffer, ID3D11Buffer *pD3DBuffer)
    {
        auto lambda = [this](uint32_t startSlot, Buffer &buffer, ID3D11Buffer *pD3DBuffer)
        {
            TResult ret = T3D_OK;
            
            do
            {
                D3D11_MAPPED_SUBRESOURCE d3dMappedData;
                memset(&d3dMappedData, 0, sizeof(d3dMappedData));
                HRESULT hr = mD3DDeviceContext->Map(pD3DBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedData);
                if (FAILED(hr))
                {
                    T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Failed to map buffer to set transform ! DX ERROR [%d]", hr);
                    ret = T3D_ERR_D3D11_MAP_RESOURCE;
                    break;
                }

                memcpy(d3dMappedData.pData, buffer.Data, buffer.DataSize);
                mD3DDeviceContext->Unmap(pD3DBuffer, 0);
                buffer.release();
                mD3DDeviceContext->VSSetConstantBuffers(startSlot, 1, &pD3DBuffer);
            } while (false);
            
            return ret;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, startSlot, buffer, pD3DBuffer);
    }

    //--------------------------------------------------------------------------

    // TResult D3D11Context::setWorldTransform(const Matrix4 &mat)
    // {
    //     mCBufferPerDraw.objectToWorld = mat;
    //     mCBufferPerDraw.worldToObject = mat.inverse();
    //     Buffer buffer;
    //     buffer.setData(&mCBufferPerDraw, sizeof(mCBufferPerDraw));
    //     
    //     return setConstantBuffer(0, buffer, mPerDrawCBuffer);
    // }

    //--------------------------------------------------------------------------

    TResult D3D11Context::setViewProjectionTransform(const Matrix4 &viewMat, const Matrix4 &projMat)
    {
        static Matrix4 conversionMat(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.5f, 0.5f,
            0.0f, 0.0f, 0.0f, 1.0f
            );
        mViewMatrix = viewMat;
        mProjMatrix = conversionMat * projMat;
        mProjViewMatrix = mProjMatrix * mViewMatrix;
        return T3D_OK;
        // mCBufferPerFrame.matrixV = viewMat;
        // mCBufferPerFrame.matrixP = conversionMat * projMat;
        // mCBufferPerFrame.matrixVP = mCBufferPerFrame.matrixP * mCBufferPerFrame.matrixV;
        // Buffer buffer;
        // buffer.setData(&mCBufferPerFrame, sizeof(CBufferPerFrame));
        //
        // return setConstantBuffer(1, buffer, mPerFrameCBuffer);
    }

    //--------------------------------------------------------------------------
    
    RHIRenderTargetPtr D3D11Context::createRenderWindow(RenderWindow *renderWindow)
    {
        D3D11RenderWindowPtr d3dRenderWindow = D3D11RenderWindow::create(renderWindow);

        auto lambda = [this](const RenderWindowPtr &pRenderWindow, const D3D11RenderWindowPtr &pD3DRenderWindow)
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
                memset(&d3dSwapChainDesc, 0, sizeof(d3dSwapChainDesc));
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
                
                ret = createRenderWindow(pD3DRenderWindow, desc.Width, desc.Height, uMSAACount, uMSAAQuality);
            } while (false);

            D3D_SAFE_RELEASE(pDXGIFactory);
            D3D_SAFE_RELEASE(pDXGIAdapter);
            D3D_SAFE_RELEASE(pDXGIDevice);

            // traceDebugInfo("D3D11 D3DObjects trace - #10 ", __FUNCTION__);
            
            return ret;
        };

        TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, RenderWindowPtr(renderWindow), D3D11RenderWindowPtr(d3dRenderWindow));
        
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

        auto lambda = [this](const PixelBuffer2DPtr &buffer, const D3D11PixelBuffer2DPtr &d3dPixelBuffer)
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
                        T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Failed to check multiple sample quality levels ! DX ERROR [%d]", hr);
                        break;
                    }
                    
                    uMSAAQuality = uNumQuality - 1;
                }
                
                bool isColorRT = true;
                if (buffer->getDescriptor().format >= PixelFormat::E_PF_D24_UNORM_S8_UINT)
                {
                    isColorRT = false;
                }

                if (isColorRT)
                {
                    // 创建颜色纹理资源
                    
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
                        T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Failed to create color texture when create render texture ! DX ERROR [%d]", hr);
                        break;
                    }

                    if (uMSAACount > 1)
                    {
                        // 创建 MSAA 解析后的纹理
                        D3D11_TEXTURE2D_DESC texResolvDesc = texDesc;  
                        texResolvDesc.SampleDesc.Count = 1;
                        texResolvDesc.SampleDesc.Quality = 0;
                        hr = mD3DDevice->CreateTexture2D(&texResolvDesc, nullptr, &d3dPixelBuffer->D3DResolveTex);
                        if (FAILED(hr))
                        {
                            // 错误
                            ret = T3D_ERR_D3D11_CREATE_TEXTURE2D;
                            T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Failed to create resolved color texture when create render texture ! DX ERROR [%d]", hr);
                            break;
                        }
                    }
                    
                    // 创建渲染目标视图
                    D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
                    memset(&rtvDesc, 0, sizeof(rtvDesc));
                    rtvDesc.Format = texDesc.Format;
                    if (uMSAACount == 1)
                    {
                        // 没开 MSAA
                        rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
                    }
                    else
                    {
                        // 开了 MSAA
                        rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
                    }
                    
                    hr = mD3DDevice->CreateRenderTargetView(d3dPixelBuffer->D3DTexture, &rtvDesc, &d3dPixelBuffer->D3DRTView);
                    if (FAILED(hr))
                    {
                        // 错误
                        ret = T3D_ERR_D3D11_CREATE_RENDER_TARGET_VIEW;
                        T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Failed to create RTV when create render texture ! DX ERROR [%d]", hr);
                        break;
                    }

                    // 创建着色器资源视图
                    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
                    memset(&srvDesc, 0, sizeof(srvDesc));
                    srvDesc.Format = texDesc.Format;
                    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
                    srvDesc.Texture2D.MostDetailedMip = 0;
                    srvDesc.Texture2D.MipLevels = 1;
                    ID3D11Texture2D *pD3DTex = nullptr;
                    if (uMSAACount == 1)
                    {
                        // 没开 MSAA
                        pD3DTex = d3dPixelBuffer->D3DTexture;
                    }
                    else
                    {
                        // 开了 MSAA
                        pD3DTex = d3dPixelBuffer->D3DResolveTex;
                    }
                    hr = mD3DDevice->CreateShaderResourceView(pD3DTex, &srvDesc, &d3dPixelBuffer->D3DSRView);
                    if (FAILED(hr))
                    {
                        // 错误
                        ret = T3D_ERR_D3D11_CREATE_SHADER_RESOURCE_VIEW;
                        T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Failed to create SRV when create render texture ! DX ERROR [%d]", hr);
                        break;
                    }
                }
                else
                {
                    // 創建深度模板緩衝紋理
                    // 创建 depth & stencil 纹理
                    D3D11_TEXTURE2D_DESC depthStencilDesc = D3D11Mapping::get(buffer->getDescriptor());
                    depthStencilDesc.SampleDesc.Count = uMSAACount;
                    depthStencilDesc.SampleDesc.Quality = uMSAAQuality;
                    depthStencilDesc.Format = D3D11Mapping::get(buffer->getDescriptor().format);
                    depthStencilDesc.Usage = D3D11Mapping::get(buffer->getUsage());
                    depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
                    depthStencilDesc.CPUAccessFlags = D3D11Mapping::get(buffer->getCPUAccessMode());
                    depthStencilDesc.MiscFlags = 0;
                    
                    HRESULT hr = mD3DDevice->CreateTexture2D(&depthStencilDesc, nullptr, &d3dPixelBuffer->D3DTexture);
                    if (FAILED(hr))
                    {
                        // 错误
                        ret = T3D_ERR_D3D11_CREATE_TEXTURE2D;
                        T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Create depth & stencil texture failed when create render texture ! DX ERROR [%d]", hr);
                        break;
                    }
                    
                    // 创建 depth & stencil 视图
                    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
                    memset(&dsvDesc, 0, sizeof(dsvDesc));
                    dsvDesc.Format = depthStencilDesc.Format;
                    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
                    dsvDesc.Texture2D.MipSlice = 0;
                    hr = mD3DDevice->CreateDepthStencilView(d3dPixelBuffer->D3DTexture, &dsvDesc, &d3dPixelBuffer->D3DDSView);
                    if (FAILED(hr))
                    {
                        // 错误
                        ret = T3D_ERR_D3D11_CREATE_DEPTH_STENCIL_VIEW;
                        T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "CreateDepthStencilView failed when create render texture ! DX ERROR [%d]", hr);
                        break;
                    }

                    // D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
                    // memset(&srvDesc, 0, sizeof(srvDesc));
                    // srvDesc.Format = depthStencilDesc.Format;
                    // srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
                    // srvDesc.Texture2D.MostDetailedMip = 0;
                    // srvDesc.Texture2D.MipLevels = 1;
                    // hr = mD3DDevice->CreateShaderResourceView(d3dPixelBuffer->D3DTexture, &srvDesc, &d3dPixelBuffer->D3DSRView);
                    // if (FAILED(hr))
                    // {
                    //     // 错误
                    //     ret = T3D_ERR_D3D11_CREATE_SHADER_RESOURCE_VIEW;
                    //     T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Failed to create SRV when create render texture ! DX ERROR [%d]", hr);
                    //     break;
                    // }
                }
            } while (false);
            
            return ret;
        };
        
        TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, PixelBuffer2DPtr(buffer), D3D11PixelBuffer2DPtr(d3dPixelBuffer));
        if (T3D_FAILED(ret))
        {
            d3dPixelBuffer = nullptr;
        }
        
        return d3dPixelBuffer;
    }
    
    //--------------------------------------------------------------------------

    TResult D3D11Context::createRenderWindow(D3D11RenderWindow *pD3DRenderWindow, uint32_t w, uint32_t h, uint32_t MSAACount, uint32_t MSAAQuality)
    {
        TResult ret = T3D_OK;

        do
        {
            // 创建 RenderTargetView
            HRESULT hr = pD3DRenderWindow->D3DSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&pD3DRenderWindow->D3DBackBuffer));
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
            
            // // 创建 DepthStencilView
            // D3D11_TEXTURE2D_DESC d3dTexDesc;
            // memset(&d3dTexDesc, 0, sizeof(d3dTexDesc));
            // d3dTexDesc.Width = w;
            // d3dTexDesc.Height = h;
            // d3dTexDesc.MipLevels = 1;
            // d3dTexDesc.ArraySize = 1;
            // d3dTexDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
            //
            // d3dTexDesc.SampleDesc.Count = MSAACount;
            // d3dTexDesc.SampleDesc.Quality = MSAAQuality;
            //
            // d3dTexDesc.Usage = D3D11_USAGE_DEFAULT;
            // d3dTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
            // d3dTexDesc.CPUAccessFlags = 0;
            // d3dTexDesc.MiscFlags = 0;
            //
            // hr = mD3DDevice->CreateTexture2D(&d3dTexDesc, nullptr, &pD3DRenderWindow->D3DDSBuffer);
            // if (FAILED(hr))
            // {
            //     ret = T3D_ERR_D3D11_CREATE_FAILED;
            //     T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Create texture 2D failed ! DX ERROR [%d]", hr);
            //     break;
            // }
            //
            // hr = mD3DDevice->CreateDepthStencilView(pD3DRenderWindow->D3DDSBuffer, nullptr, &pD3DRenderWindow->D3DDSView);
            // if (FAILED(hr))
            // {
            //     ret = T3D_ERR_D3D11_CREATE_FAILED;
            //     T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Create depth stencil view failed ! DX ERROR [%d]", hr);
            //     break;
            // }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::resizeRenderWindow(D3D11RenderWindow *rw, uint32_t w, uint32_t h)
    {
        D3D11RenderWindow *pD3DRenderWindow = rw;
        auto lambda = [this](const D3D11RenderWindowPtr &pD3DRenderWindow, uint32_t w, uint32_t h)
        {
            TResult ret = T3D_OK;

            do
            {
                D3D11_TEXTURE2D_DESC desc;
                pD3DRenderWindow->D3DBackBuffer->GetDesc(&desc);
            
                D3D_SAFE_RELEASE(pD3DRenderWindow->D3DRTView);
                D3D_SAFE_RELEASE(pD3DRenderWindow->D3DBackBuffer);
                D3D_SAFE_RELEASE(pD3DRenderWindow->D3DDSView);
                D3D_SAFE_RELEASE(pD3DRenderWindow->D3DDSBuffer);

                HRESULT hr = pD3DRenderWindow->D3DSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
                if (FAILED(hr))
                {
                    ret = T3D_ERR_D3D11_RESIZE_BUFFERS;
                    T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Resize render window failed ! DX ERROR [%d]", hr);
                    break;
                }

                ret = createRenderWindow(pD3DRenderWindow, w, h, desc.SampleDesc.Count, desc.SampleDesc.Quality);
            } while (false);

            return ret;
        };
        
        return ENQUEUE_UNIQUE_COMMAND(lambda, D3D11RenderWindowSafePtr(pD3DRenderWindow), w, h);
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::resizeRenderTexture(RenderTexture *rt, uint32_t w, uint32_t h)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::resizeRenderTarget(RenderTarget *rt, uint32_t w, uint32_t h)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    void D3D11Context::backupRenderState()
    {
        mD3DDeviceContext->OMGetRenderTargets(1, &mBackupState.RenderTargetView, &mBackupState.DepthStencilView);
        mBackupState.ScissorRectsCount = mBackupState.ViewportsCount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
        mD3DDeviceContext->RSGetScissorRects(&mBackupState.ScissorRectsCount, mBackupState.ScissorRects);
        mD3DDeviceContext->RSGetViewports(&mBackupState.ViewportsCount, mBackupState.Viewports);
        mD3DDeviceContext->RSGetState(&mBackupState.RS);
        mD3DDeviceContext->OMGetBlendState(&mBackupState.BlendState, mBackupState.BlendFactor, &mBackupState.SampleMask);
        mD3DDeviceContext->OMGetDepthStencilState(&mBackupState.DepthStencilState, &mBackupState.StencilRef);
        mD3DDeviceContext->PSGetShaderResources(0, 1, &mBackupState.PSShaderResource);
        mD3DDeviceContext->PSGetSamplers(0, 1, &mBackupState.PSSampler);
        mBackupState.PSInstancesCount = mBackupState.VSInstancesCount = mBackupState.GSInstancesCount = 256;
        mD3DDeviceContext->PSGetShader(&mBackupState.PS, mBackupState.PSInstances, &mBackupState.PSInstancesCount);
        mD3DDeviceContext->VSGetShader(&mBackupState.VS, mBackupState.VSInstances, &mBackupState.VSInstancesCount);
        mD3DDeviceContext->VSGetConstantBuffers(0, 1, &mBackupState.VSConstantBuffer);
        mD3DDeviceContext->GSGetShader(&mBackupState.GS, mBackupState.GSInstances, &mBackupState.GSInstancesCount);
        mD3DDeviceContext->IAGetPrimitiveTopology(&mBackupState.PrimitiveTopology);
        mD3DDeviceContext->IAGetIndexBuffer(&mBackupState.IndexBuffer, &mBackupState.IndexBufferFormat, &mBackupState.IndexBufferOffset);
        mD3DDeviceContext->IAGetVertexBuffers(0, 1, &mBackupState.VertexBuffer, &mBackupState.VertexBufferStride, &mBackupState.VertexBufferOffset);
        mD3DDeviceContext->IAGetInputLayout(&mBackupState.InputLayout);
    }

    //--------------------------------------------------------------------------
    
    TResult D3D11Context::setRenderTarget(RenderWindow *renderWindow)
    {
        D3D11RenderWindow *pD3DRenderWindow = static_cast<D3D11RenderWindow*>(renderWindow->getRHIRenderWindow());
        D3D11PixelBuffer2D *pD3DDepthStencil = static_cast<D3D11PixelBuffer2D*>(renderWindow->getRHIDepthStencilTex());
        auto lambda = [this](const D3D11RenderWindowPtr &pD3DRenderWindow, const D3D11PixelBuffer2DPtr &pD3DDepthStencil)
        {
            backupRenderState();
            if (pD3DDepthStencil != nullptr)
            {
                mD3DDeviceContext->OMSetRenderTargets(1, &pD3DRenderWindow->D3DRTView, pD3DDepthStencil->D3DDSView);                
            }
            else
            {
                mD3DDeviceContext->OMSetRenderTargets(1, &pD3DRenderWindow->D3DRTView, nullptr);
            }
            
            return T3D_OK;
        };
        
        TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, D3D11RenderWindowPtr(pD3DRenderWindow), D3D11PixelBuffer2DPtr(pD3DDepthStencil));
        
        if (T3D_SUCCEEDED(ret))
        {
            mCurrentRenderWindow = renderWindow;
            mCurrentDepthStencil = renderWindow->getRHIDepthStencilTex();
        }
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::setRenderTarget(RenderTexture *renderTexture, RenderTexture *depthStencil)
    {
        D3D11PixelBuffer2D *pD3DPixelBuffer = static_cast<D3D11PixelBuffer2D*>(renderTexture->getPixelBuffer()->getRHIResource().get());
        D3D11PixelBuffer2D *pD3DDepthStencil = nullptr;
        if (depthStencil != nullptr)
        {
            pD3DDepthStencil = static_cast<D3D11PixelBuffer2D*>(depthStencil->getPixelBuffer()->getRHIResource().get());
        }
        auto lambda = [this](const D3D11PixelBuffer2DPtr &pD3DPixelBuffer, const D3D11PixelBuffer2DPtr &pD3DDepthStencil)
        {
            backupRenderState();
            if (pD3DDepthStencil != nullptr)
            {
                mD3DDeviceContext->OMSetRenderTargets(1, &pD3DPixelBuffer->D3DRTView, pD3DDepthStencil->D3DDSView);                
            }
            else
            {
                mD3DDeviceContext->OMSetRenderTargets(1, &pD3DPixelBuffer->D3DRTView, nullptr);
            }
            return T3D_OK;
        };

        TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, D3D11PixelBuffer2DPtr(pD3DPixelBuffer), D3D11PixelBuffer2DPtr(pD3DDepthStencil));
        
        if (T3D_SUCCEEDED(ret))
        {
            mCurrentRenderTexture = renderTexture;
            mCurrentDepthStencil = depthStencil;
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
                ret = setRenderTarget(renderTarget->getRenderTexture(), renderTarget->getDepthStencil());
            }
            break;
        default:
            T3D_ASSERT(false);
            break;
        }
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::resetRenderTarget()
    {
        mCurrentRenderWindow = nullptr;
        mCurrentRenderTexture = nullptr;
        mCurrentDepthStencil = nullptr;

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
        D3D11RenderWindow *pD3DRenderWindow = static_cast<D3D11RenderWindow*>(window->getRHIRenderWindow());
        auto lambda = [this](const D3D11RenderWindowPtr &pD3DRenderWindow, const ColorRGB &color)
        {
            float clr[4];
            clr[0] = color.red();
            clr[1] = color.green();
            clr[2] = color.blue();
            clr[3] = 1.0f;
            mD3DDeviceContext->ClearRenderTargetView(pD3DRenderWindow->D3DRTView, clr);
            return T3D_OK;
        };
        return ENQUEUE_UNIQUE_COMMAND(lambda, D3D11RenderWindowPtr(pD3DRenderWindow), color);
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
        return ENQUEUE_UNIQUE_COMMAND(lambda, D3D11PixelBuffer2DPtr(pD3DPixelBuffer), color);
    }

    //--------------------------------------------------------------------------
    
    TResult D3D11Context::clearDepthStencil(Real depth, uint32_t stencil)
    {
        TResult ret = T3D_OK;
        
        if (mCurrentDepthStencil != nullptr)
        {
            ret = clearDepthStencil(mCurrentDepthStencil, depth, static_cast<uint8_t>(stencil));
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::clearDepthStencil(RenderWindow *window, const Real &depth, uint8_t stencil)
    {
        D3D11PixelBuffer2D *pD3DDepthStencil = static_cast<D3D11PixelBuffer2D*>(window->getRHIDepthStencilTex());
        auto lambda = [this](const D3D11PixelBuffer2DPtr &pD3DDepthStencil, const Real &depth, uint8_t stencil)
        {
            mD3DDeviceContext->ClearDepthStencilView(pD3DDepthStencil->D3DDSView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, depth, stencil);
            return T3D_OK;
        };
        return ENQUEUE_UNIQUE_COMMAND(lambda, D3D11PixelBuffer2DPtr(pD3DDepthStencil), depth, stencil);
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
        D3D11BlendStatePtr d3dState = D3D11BlendState::create();

        const BlendDesc &desc = state->getStateDesc();
        D3D11_BLEND_DESC d3dDesc;
        memset(&d3dDesc, 0, sizeof(d3dDesc));
        d3dDesc.AlphaToCoverageEnable = desc.AlphaToCoverageEnable;
        d3dDesc.IndependentBlendEnable = desc.IndependentBlendEnable;
        for (uint32_t i = 0; i < BlendDesc::kMaxRenderTarget; ++i)
        {
            auto &dst = d3dDesc.RenderTarget[i];
            const auto &src = desc.RenderTargetStates[i];
            dst.BlendEnable = src.BlendEnable;
            dst.SrcBlend = D3D11Mapping::get(src.SrcBlend);
            dst.DestBlend = D3D11Mapping::get(src.DestBlend);
            dst.BlendOp = D3D11Mapping::get(src.BlendOp);
            dst.SrcBlendAlpha = D3D11Mapping::get(src.SrcBlendAlpha);
            dst.DestBlendAlpha = D3D11Mapping::get(src.DstBlendAlpha);
            dst.BlendOpAlpha = D3D11Mapping::get(src.BlendOpAlpha);
            dst.RenderTargetWriteMask = D3D11Mapping::get(static_cast<BlendColorWriteMask>(src.ColorMask));
        }
        
        auto lambda = [this](const D3D11_BLEND_DESC &d3dDesc, const D3D11BlendStatePtr &d3dState)
        {
            TResult ret = T3D_OK;

            do
            {
                ID3D11BlendState *pD3DState = nullptr;
                HRESULT hr = mD3DDevice->CreateBlendState(&d3dDesc, &pD3DState);
                if (FAILED(hr))
                {
                    T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Failed to create blend state ! DX ERROR [%d]", hr);
                    ret = T3D_ERR_D3D11_CREATE_BLEND_STATE;
                    break;
                }

                d3dState->D3DBlendState = pD3DState;
            } while (false);
            
            return ret;
        };

        TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, d3dDesc, D3D11BlendStatePtr(d3dState));
        if (T3D_FAILED(ret))
        {
            d3dState = nullptr;
        }
        
        return d3dState;
    }

    //--------------------------------------------------------------------------
    
    RHIDepthStencilStatePtr D3D11Context::createDepthStencilState(DepthStencilState *state)
    {
        D3D11DepthStencilStatePtr d3dState = D3D11DepthStencilState::create();

        const DepthStencilDesc &desc = state->getStateDesc();
        D3D11_DEPTH_STENCIL_DESC d3dDesc;
        memset(&d3dDesc, 0, sizeof(d3dDesc));
        d3dDesc.DepthEnable = desc.DepthTestEnable;
        d3dDesc.DepthWriteMask = (desc.DepthWriteEnable ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO);
        d3dDesc.DepthFunc = D3D11Mapping::get(desc.DepthFunc);
        d3dDesc.StencilEnable = desc.StencilEnable;
        d3dDesc.StencilReadMask = desc.StencilReadMask;
        d3dDesc.StencilWriteMask = desc.StencilWriteMask;
        d3dDesc.FrontFace.StencilFunc = D3D11Mapping::get(desc.FrontFace.StencilFunc);
        d3dDesc.FrontFace.StencilDepthFailOp = D3D11Mapping::get(desc.FrontFace.StencilDepthFailOp);
        d3dDesc.FrontFace.StencilFailOp = D3D11Mapping::get(desc.FrontFace.StencilFailOp);
        d3dDesc.FrontFace.StencilPassOp = D3D11Mapping::get(desc.FrontFace.StencilPassOp);
        d3dDesc.BackFace.StencilFunc = D3D11Mapping::get(desc.BackFace.StencilFunc);
        d3dDesc.BackFace.StencilDepthFailOp = D3D11Mapping::get(desc.BackFace.StencilDepthFailOp);
        d3dDesc.BackFace.StencilFailOp = D3D11Mapping::get(desc.BackFace.StencilFailOp);
        d3dDesc.BackFace.StencilPassOp = D3D11Mapping::get(desc.BackFace.StencilPassOp);

        auto lambda = [this](const D3D11_DEPTH_STENCIL_DESC &d3dDesc, const D3D11DepthStencilStatePtr &d3dState)
        {
            TResult ret = T3D_OK;

            do
            {
                ID3D11DepthStencilState *pD3DState = nullptr;
                HRESULT hr = mD3DDevice->CreateDepthStencilState(&d3dDesc, &pD3DState);
                if (FAILED(hr))
                {
                    T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Failed to create depth stencil state ! DX ERROR [%d]", hr);
                    ret = T3D_ERR_D3D11_CREATE_DEPTH_STENCIL_STATE;
                    break;
                }

                d3dState->D3DDepthStencilState = pD3DState;
            } while (false);
            
            return ret;
        };

        TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, d3dDesc, D3D11DepthStencilStatePtr(d3dState));
        if (T3D_FAILED(ret))
        {
            d3dState = nullptr;
        }
        
        return d3dState;
    }

    //--------------------------------------------------------------------------
    
    RHIRasterizerStatePtr D3D11Context::createRasterizerState(RasterizerState *state)
    {
        D3D11RasterizerStatePtr d3dState = D3D11RasterizerState::create();

        const RasterizerDesc &desc = state->getStateDesc();
        D3D11_RASTERIZER_DESC d3dDesc;
        memset(&d3dDesc, 0, sizeof(d3dDesc));
        d3dDesc.FillMode = D3D11Mapping::get(desc.FillMode);
        d3dDesc.CullMode = D3D11Mapping::get(desc.CullMode);
        d3dDesc.FrontCounterClockwise = desc.FrontAnticlockwise;
        d3dDesc.DepthBias = static_cast<INT>(desc.DepthBias);
        d3dDesc.DepthBiasClamp = desc.DepthBiasClamp;
        d3dDesc.SlopeScaledDepthBias = desc.SlopeScaledDepthBias;
        d3dDesc.DepthClipEnable = desc.DepthClipEnable; // 启用深度裁剪
        d3dDesc.ScissorEnable = desc.ScissorEnable; // 禁用剪裁测试
        d3dDesc.MultisampleEnable = desc.MultisampleEnable; // 禁用多采样
        d3dDesc.AntialiasedLineEnable = desc.AntialiasedLineEnable; // 禁用抗锯齿线条
        
        auto lambda = [this](const D3D11_RASTERIZER_DESC &d3dDesc, const D3D11RasterizerStatePtr &d3dState)
        {
            TResult ret = T3D_OK;

            do
            {
                ID3D11RasterizerState *pD3DState = nullptr;
                HRESULT hr = mD3DDevice->CreateRasterizerState(&d3dDesc, &pD3DState);
                if (FAILED(hr))
                {
                    T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Failed to create rasterizer state ! DX ERROR [%d]", hr);
                    ret = T3D_ERR_D3D11_CREATE_RASTERIZER_STATE;
                    break;
                }

                d3dState->D3DRasterizerState = pD3DState;
            } while (false);
            return ret;
        };

        TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, d3dDesc, d3dState);
        if (T3D_FAILED(ret))
        {
            d3dState = nullptr;
        }
        
        return d3dState;
    }

    //--------------------------------------------------------------------------
    
    RHISamplerStatePtr D3D11Context::createSamplerState(SamplerState *state)
    {
        D3D11SamplerStatePtr d3dState = D3D11SamplerState::create();

        const SamplerDesc &desc = state->getStateDesc();
        D3D11_SAMPLER_DESC d3dDesc;
        memset(&d3dDesc, 0, sizeof(d3dDesc));
        d3dDesc.Filter = D3D11Mapping::get(desc.MinFilter,desc.MagFilter, desc.MipFilter);
        d3dDesc.AddressU = D3D11Mapping::get(desc.AddressU); // 设置U方向寻址模式为环绕
        d3dDesc.AddressV = D3D11Mapping::get(desc.AddressV); // 设置V方向寻址模式为环绕
        d3dDesc.AddressW = D3D11Mapping::get(desc.AddressW); // 设置W方向寻址模式为环绕
        d3dDesc.MipLODBias = desc.MipLODBias;
        d3dDesc.MaxAnisotropy = desc.MaxAnisotropy;
        d3dDesc.ComparisonFunc = D3D11Mapping::get(desc.CompareFunc);
        d3dDesc.BorderColor[0] = desc.BorderColor.blue();
        d3dDesc.BorderColor[1] = desc.BorderColor.green();
        d3dDesc.BorderColor[2] = desc.BorderColor.red();
        d3dDesc.BorderColor[3] = desc.BorderColor.alpha();
        d3dDesc.MinLOD = desc.MinLOD;
        d3dDesc.MaxLOD = desc.MaxLOD;

        auto lambda = [this](const D3D11_SAMPLER_DESC &d3dDesc, const D3D11SamplerStatePtr &d3dState)
        {
            TResult ret = T3D_OK;

            do
            {
                ID3D11SamplerState *pD3DState = nullptr;
                HRESULT hr = mD3DDevice->CreateSamplerState(&d3dDesc, &pD3DState);
                if (FAILED(hr))
                {
                    T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Failed to create sampler state ! DX ERROR [%d]", hr);
                    ret = T3D_ERR_D3D11_CREATE_SAMPLER_STATE;
                    break;
                }

                d3dState->D3DSamplerState = pD3DState;
            } while (false);

            return ret;
        };

        TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, d3dDesc, d3dState);
        if (T3D_FAILED(ret))
        {
            d3dState = nullptr;
        }
        
        return d3dState;
    }

    //--------------------------------------------------------------------------
    
    TResult D3D11Context::setBlendState(BlendState *state)
    {
        auto lambda = [this](const D3D11BlendStatePtr &d3dState)
        {
            mD3DDeviceContext->OMSetBlendState(d3dState->D3DBlendState, nullptr, -1);
            return T3D_OK;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, smart_pointer_cast<D3D11BlendState>(state->getRHIState()));
    }

    //--------------------------------------------------------------------------
    
    TResult D3D11Context::setDepthStencilState(DepthStencilState *state)
    {
        auto lambda = [this](const D3D11DepthStencilStatePtr &d3dState)
        {
            mD3DDeviceContext->OMSetDepthStencilState(d3dState->D3DDepthStencilState, 1);
            return T3D_OK;
        };
        return ENQUEUE_UNIQUE_COMMAND(lambda, smart_pointer_cast<D3D11DepthStencilState>(state->getRHIState()));
    }

    //--------------------------------------------------------------------------
    
    TResult D3D11Context::setRasterizerState(RasterizerState *state)
    {
        auto lambda = [this](const D3D11RasterizerStatePtr &d3dState)
        {
            mD3DDeviceContext->RSSetState(d3dState->D3DRasterizerState);
            return T3D_OK;
        };
        return ENQUEUE_UNIQUE_COMMAND(lambda, smart_pointer_cast<D3D11RasterizerState>(state->getRHIState()));
    }

    //--------------------------------------------------------------------------

    RHIVertexDeclarationPtr D3D11Context::createVertexDeclaration(VertexDeclaration *decl)
    {
        D3D11VertexDeclarationPtr d3dDecl = D3D11VertexDeclaration::create();

        do
        {
            using D3D11InputDescs = TArray<D3D11_INPUT_ELEMENT_DESC>;
            D3D11InputDescs d3dDescs(decl->getAttributeCount());
            for (uint32_t i = 0; i < decl->getAttributeCount(); ++i)
            {
                const VertexAttribute &attrib = decl->getAttributes()[i];
                d3dDescs[i].SemanticName = D3D11Mapping::get(attrib.getSemantic());
                d3dDescs[i].SemanticIndex = (UINT)attrib.getSemanticIndex();
                d3dDescs[i].Format = D3D11Mapping::get(attrib.getType());
                d3dDescs[i].InputSlot = (UINT)attrib.getSlot();
                d3dDescs[i].AlignedByteOffset = (UINT)attrib.getOffset();
                d3dDescs[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
                d3dDescs[i].InstanceDataStepRate = 0;
            }
            
            auto lambda = [this](const D3D11InputDescs &d3dDescs, const D3D11VertexDeclarationPtr &d3dDecl, const ShaderVariantPtr &vertexShader)
            {
                TResult ret = T3D_OK;

                do
                {
                    ID3D11InputLayout *pD3DInputLayout = nullptr;
                    size_t bytesLength = 0;
                    char *bytes = vertexShader->getBytesCode(bytesLength);
                    HRESULT hr = mD3DDevice->CreateInputLayout(d3dDescs.data(), (UINT)d3dDescs.size(), bytes, bytesLength, &pD3DInputLayout);
                    if (FAILED(hr))
                    {
                        T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Failed to create input layout ! DX ERROR [%d]", hr);
                        ret = T3D_ERR_D3D11_CREATE_INPUT_LAYOUT;
                        break;
                    }

                    d3dDecl->D3D11InputLayout = pD3DInputLayout;
                } while (false);
                
                return ret;
            };

            TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, d3dDescs, d3dDecl, ShaderVariantPtr(decl->getVertexShader()));
            if (T3D_FAILED(ret))
            {
                d3dDecl = nullptr;
                break;
            }
        } while (false);
        
        return d3dDecl;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::setVertexDeclaration(VertexDeclaration *decl)
    {
        auto lambda = [this](const D3D11VertexDeclarationPtr &d3dDecl)
        {
            mD3DDeviceContext->IASetInputLayout(d3dDecl->D3D11InputLayout);
            return T3D_OK;
        };
        
        return ENQUEUE_UNIQUE_COMMAND(lambda, smart_pointer_cast<D3D11VertexDeclaration>(decl->getRHIResource()));
    }

    //--------------------------------------------------------------------------
    
    RHIVertexBufferPtr D3D11Context::createVertexBuffer(VertexBuffer *buffer)
    {
        D3D11VertexBufferPtr d3dBuffer = D3D11VertexBuffer::create();

        do
        {
            D3D11_USAGE d3dUsage;
            uint32_t d3dAccess = 0;

            TResult ret = D3D11Mapping::get(buffer->getUsage(), buffer->getCPUAccessMode(), d3dUsage, d3dAccess);
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Failed to mapping usage & cpu access mode when create vertex buffer !");
                break;
            }
            
            D3D11_BUFFER_DESC d3dDesc;
            memset(&d3dDesc, 0, sizeof(d3dDesc));
            d3dDesc.Usage = d3dUsage;
            d3dDesc.ByteWidth = (UINT)buffer->getBufferSize();
            d3dDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
            d3dDesc.CPUAccessFlags = d3dAccess;
            
            auto lambda = [this](const D3D11_BUFFER_DESC &d3dDesc, const D3D11VertexBufferPtr &d3dBuffer, const VertexBufferPtr &buffer)
            {
                TResult ret = T3D_OK;
                
                do
                {
                    // 创建顶点缓冲区子资源数据
                    D3D11_SUBRESOURCE_DATA initData;
                    memset(&initData, 0, sizeof(initData));
                    initData.pSysMem = buffer->getBuffer().Data;
                    initData.SysMemPitch = 0;
                    initData.SysMemSlicePitch = 0;

                    ID3D11Buffer *pD3DBuffer = nullptr;
                    HRESULT hr = mD3DDevice->CreateBuffer(&d3dDesc, &initData, &pD3DBuffer);
                    if (FAILED(hr))
                    {
                        T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Failed to create vertex buffer ! DX ERROR [%d]", hr);
                        ret = T3D_ERR_D3D11_CREATE_BUFFER;
                        break;
                    }

                    d3dBuffer->D3DBuffer = pD3DBuffer;
                } while (false);
                
                return ret;
            };

            ret = ENQUEUE_UNIQUE_COMMAND(lambda, d3dDesc, d3dBuffer, VertexBufferPtr(buffer));
            if (T3D_FAILED(ret))
            {
                d3dBuffer = nullptr;
                break;
            }
        } while (false);
        
        return d3dBuffer;
    }

    //--------------------------------------------------------------------------
    
    TResult D3D11Context::setVertexBuffers(uint32_t startSlot, const VertexBuffers &buffers, const VertexStrides &strides, const VertexOffsets &offsets)
    {
        auto lambda = [this](uint32_t startSlot, const VertexBuffers &buffers, const VertexStrides &strides, const VertexOffsets &offsets)
        {
            TResult ret = T3D_OK;

            TArray<ID3D11Buffer*> vbuffers(buffers.size());
            for (uint32_t i = 0; i < buffers.size(); ++i)
            {
                const auto &vb = buffers[i];
                vbuffers[i] = smart_pointer_cast<D3D11VertexBuffer>(vb->getRHIResource())->D3DBuffer;
            }

            mD3DDeviceContext->IASetVertexBuffers(startSlot, (UINT)buffers.size(), vbuffers.data(), strides.data(), offsets.data());

            return ret;
        };
        
        return ENQUEUE_UNIQUE_COMMAND(lambda, startSlot, buffers, strides, offsets);
    }

    //--------------------------------------------------------------------------
    
    RHIIndexBufferPtr D3D11Context::createIndexBuffer(IndexBuffer *buffer)
    {
        D3D11IndexBufferPtr d3dBuffer = D3D11IndexBuffer::create();

        do
        {
            D3D11_USAGE d3dUsage;
            uint32_t d3dAccess = 0;

            TResult ret = D3D11Mapping::get(buffer->getUsage(), buffer->getCPUAccessMode(), d3dUsage, d3dAccess);
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Failed to mapping usage & cpu access mode when create index buffer !");
                break;
            }
            
            D3D11_BUFFER_DESC d3dDesc;
            memset(&d3dDesc, 0, sizeof(d3dDesc));
            d3dDesc.Usage = d3dUsage;
            d3dDesc.ByteWidth = (UINT)buffer->getBufferSize();
            d3dDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
            d3dDesc.CPUAccessFlags = d3dAccess;
            
            auto lambda = [this](const D3D11_BUFFER_DESC &d3dDesc, const D3D11IndexBufferPtr &d3dBuffer, const IndexBufferPtr &buffer)
            {
                TResult ret = T3D_OK;
                
                do
                {
                    // 创建顶点缓冲区子资源数据
                    D3D11_SUBRESOURCE_DATA initData;
                    memset(&initData, 0, sizeof(initData));
                    initData.pSysMem = buffer->getBuffer().Data;
                    initData.SysMemPitch = 0;
                    initData.SysMemSlicePitch = 0;

                    ID3D11Buffer *pD3DBuffer = nullptr;
                    HRESULT hr = mD3DDevice->CreateBuffer(&d3dDesc, &initData, &pD3DBuffer);
                    if (FAILED(hr))
                    {
                        T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Failed to create index buffer ! DX ERROR [%d]", hr);
                        ret = T3D_ERR_D3D11_CREATE_BUFFER;
                        break;
                    }

                    d3dBuffer->D3DBuffer = pD3DBuffer;
                } while (false);
                
                return ret;
            };

            ret = ENQUEUE_UNIQUE_COMMAND(lambda, d3dDesc, d3dBuffer, IndexBufferPtr(buffer));
            if (T3D_FAILED(ret))
            {
                d3dBuffer = nullptr;
                break;
            }
        } while (false);
        
        return d3dBuffer;
    }

    //--------------------------------------------------------------------------
    
    TResult D3D11Context::setIndexBuffer(IndexBuffer *buffer)
    {
        D3D11IndexBufferPtr d3dIndexBuffer = smart_pointer_cast<D3D11IndexBuffer>(buffer->getRHIResource());
        DXGI_FORMAT d3dFormat = D3D11Mapping::get(buffer->getIndexType());
        
        auto lambda = [this](DXGI_FORMAT d3dFormat, const D3D11IndexBufferPtr &d3dIndexBuffer)
        {
            mD3DDeviceContext->IASetIndexBuffer(d3dIndexBuffer->D3DBuffer, d3dFormat, 0);
            return T3D_OK;
        };
        
        return ENQUEUE_UNIQUE_COMMAND(lambda, d3dFormat, d3dIndexBuffer);
    }

    //--------------------------------------------------------------------------
    
    RHIConstantBufferPtr D3D11Context::createConstantBuffer(ConstantBuffer *buffer)
    {
        D3D11ConstantBufferPtr d3dBuffer = D3D11ConstantBuffer::create();

        do
        {
            D3D11_USAGE d3dUsage;
            uint32_t d3dAccess = 0;

            TResult ret = D3D11Mapping::get(buffer->getUsage(), buffer->getCPUAccessMode(), d3dUsage, d3dAccess);
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Failed to mapping usage & cpu access mode when create index buffer !");
                break;
            }
            
            D3D11_BUFFER_DESC d3dDesc;
            memset(&d3dDesc, 0, sizeof(d3dDesc));
            d3dDesc.Usage = d3dUsage;
            d3dDesc.ByteWidth = (UINT)buffer->getBufferSize();
            d3dDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            d3dDesc.CPUAccessFlags = d3dAccess;
            
            auto lambda = [this](const D3D11_BUFFER_DESC &d3dDesc, const D3D11ConstantBufferPtr &d3dBuffer, const ConstantBufferPtr &buffer)
            {
                TResult ret = T3D_OK;
                
                do
                {
                    // 创建顶点缓冲区子资源数据
                    D3D11_SUBRESOURCE_DATA initData;
                    memset(&initData, 0, sizeof(initData));
                    initData.pSysMem = buffer->getBuffer().Data;
                    initData.SysMemPitch = 0;
                    initData.SysMemSlicePitch = 0;

                    ID3D11Buffer *pD3DBuffer = nullptr;
                    HRESULT hr = mD3DDevice->CreateBuffer(&d3dDesc, &initData, &pD3DBuffer);
                    if (FAILED(hr))
                    {
                        T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Failed to create constant buffer ! DX ERROR [%d]", hr);
                        ret = T3D_ERR_D3D11_CREATE_BUFFER;
                        break;
                    }

                    d3dBuffer->D3DBuffer = pD3DBuffer;
                } while (false);

                return ret;
            };

            ret = ENQUEUE_UNIQUE_COMMAND(lambda, d3dDesc, d3dBuffer, ConstantBufferPtr(buffer));
            if (T3D_FAILED(ret))
            {
                d3dBuffer = nullptr;
                break;
            }
        } while (false);
        
        return d3dBuffer;
    }

    //--------------------------------------------------------------------------
    
    RHIPixelBuffer1DPtr D3D11Context::createPixelBuffer1D(PixelBuffer1D *buffer)
    {
        D3D11PixelBuffer1DPtr d3dBuffer = D3D11PixelBuffer1D::create();
        return d3dBuffer;
    }

    //--------------------------------------------------------------------------
    
    RHIPixelBuffer2DPtr D3D11Context::createPixelBuffer2D(PixelBuffer2D *buffer)
    {
        D3D11PixelBuffer2DPtr d3dBuffer = D3D11PixelBuffer2D::create();

        do
        {
            const auto &desc = buffer->getDescriptor();
            
            D3D11_USAGE d3dUsage;
            uint32_t d3dAccess = 0;
            TResult ret = D3D11Mapping::get(buffer->getUsage(), buffer->getCPUAccessMode(), d3dUsage, d3dAccess);
            
            // 创建2D纹理描述符
            D3D11_TEXTURE2D_DESC d3dDesc;
            memset(&d3dDesc, 0, sizeof(d3dDesc));
            d3dDesc.Width = desc.width;
            d3dDesc.Height = desc.height;
            d3dDesc.MipLevels = desc.mipmaps;
            d3dDesc.ArraySize = desc.arraySize;
            d3dDesc.Format = D3D11Mapping::get(desc.format);
            d3dDesc.SampleDesc.Count = desc.sampleDesc.Count;
            d3dDesc.SampleDesc.Quality = desc.sampleDesc.Quality;
            d3dDesc.Usage = d3dUsage;
            d3dDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            d3dDesc.CPUAccessFlags = d3dAccess;
            d3dDesc.MiscFlags = 0;

            // 创建着色器资源视图描述符
            D3D11_SHADER_RESOURCE_VIEW_DESC d3dSRVDesc;
            memset(&d3dSRVDesc, 0, sizeof(d3dSRVDesc));
            d3dSRVDesc.Format = D3D11Mapping::get(desc.format);
            d3dSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            d3dSRVDesc.Texture2D.MipLevels = desc.mipmaps;
            d3dSRVDesc.Texture2D.MostDetailedMip = 0;

            auto lambda = [this](const D3D11_TEXTURE2D_DESC &d3dDesc, const D3D11_SHADER_RESOURCE_VIEW_DESC &d3dSRVDesc, const D3D11PixelBuffer2DPtr &d3dBuffer, const PixelBuffer2DPtr &buffer)
            {
                TResult ret = T3D_OK;

                do
                {
                    // 创建2D纹理子资源数据
                    D3D11_SUBRESOURCE_DATA initDataDesc = {};
                    initDataDesc.pSysMem = buffer->getBuffer().Data;
                    initDataDesc.SysMemPitch = static_cast<uint32_t>(buffer->getBuffer().DataSize) / buffer->getDescriptor().height;
                    initDataDesc.SysMemSlicePitch = 0;

                    ID3D11Texture2D *pD3DTex2D = nullptr;
                    HRESULT hr = mD3DDevice->CreateTexture2D(&d3dDesc, &initDataDesc, &pD3DTex2D);
                    if (FAILED(hr))
                    {
                        T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Failed to create texture2d ! DX ERROR [%d]", hr);
                        ret = T3D_ERR_D3D11_CREATE_TEXTURE2D;
                        break;
                    }
                    
                    // 创建着色器资源视图
                    ID3D11ShaderResourceView *pD3DSRView = nullptr;
                    hr = mD3DDevice->CreateShaderResourceView(pD3DTex2D, &d3dSRVDesc, &pD3DSRView);
                    if (FAILED(hr))
                    {
                        T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Failed to create shader resource view for 2D texture ! DX ERROR [%d]", hr);
                        ret = T3D_ERR_D3D11_CREATE_SHADER_RESOURCE_VIEW;
                        break;
                    }

                    d3dBuffer->D3DTexture = pD3DTex2D;
                    d3dBuffer->D3DSRView = pD3DSRView;
                } while (false);
                
                return ret;
            };

            ret = ENQUEUE_UNIQUE_COMMAND(lambda, d3dDesc, d3dSRVDesc, d3dBuffer, buffer);
            if (T3D_FAILED(ret))
            {
                d3dBuffer = nullptr;
                break;
            }
        } while (false);
        
        
        return d3dBuffer;
    }

    //--------------------------------------------------------------------------
    
    RHIPixelBuffer3DPtr D3D11Context::createPixelBuffer3D(PixelBuffer3D *buffer)
    {
        return nullptr;
    }

    //--------------------------------------------------------------------------
    
    RHIShaderPtr D3D11Context::createVertexShader(ShaderVariant *shader)
    {
        D3D11VertexShaderPtr d3dShader = D3D11VertexShader::create();

        do
        {
            auto lambda = [this](const ShaderVariantPtr &shader, const D3D11VertexShaderPtr &d3dShader)
            {
                TResult ret = T3D_OK;
                
                do
                {
                    size_t bytecodeLength = 0;
                    const char *bytecode = shader->getBytesCode(bytecodeLength);
                    ID3D11VertexShader *pD3DShader = nullptr;
                    HRESULT hr = mD3DDevice->CreateVertexShader(bytecode, bytecodeLength, nullptr, &pD3DShader);
                    if (FAILED(hr))
                    {
                        T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Failed to create vertex shader ! DX ERROR [%d]", hr);
                        ret = T3D_ERR_D3D11_CREATE_VERTEX_SHADER;
                        break;
                    }

                    d3dShader->D3DShader = pD3DShader;
                } while (false);

                return ret;
            };

            TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, ShaderVariantPtr(shader), d3dShader);
            if (T3D_FAILED(ret))
            {
                d3dShader = nullptr;
                break;
            }
        } while (false);
        
        return d3dShader;
    }

    //--------------------------------------------------------------------------
    
    TResult D3D11Context::setVertexShader(ShaderVariant *shader)
    {
        D3D11VertexShaderPtr d3dShader = static_cast<D3D11VertexShader*>(shader->getRHIShader());
        
        auto lambda = [this](const D3D11VertexShaderPtr &d3dShader)
        {
            mD3DDeviceContext->VSSetShader(d3dShader->D3DShader, nullptr, 0);
            return T3D_OK;
        };
        
        return ENQUEUE_UNIQUE_COMMAND(lambda, d3dShader);
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::setVSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers)
    {
        return setConstantBuffers(&ID3D11DeviceContext::VSSetConstantBuffers, startSlot, buffers);
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::setVSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers)
    {
        return setPixelBuffers(&ID3D11DeviceContext::VSSetShaderResources, startSlot, buffers);
    }
    
    //--------------------------------------------------------------------------
    
    TResult D3D11Context::setVSSamplers(uint32_t startSlot, const Samplers &samplers)
    {
        return setSamplers(&ID3D11DeviceContext::VSSetSamplers, startSlot, samplers);
    }

    //--------------------------------------------------------------------------
    
    RHIShaderPtr D3D11Context::createPixelShader(ShaderVariant *shader)
    {
        D3D11PixelShaderPtr d3dShader = D3D11PixelShader::create();

        do
        {
            auto lambda = [this](const ShaderVariantPtr &shader, const D3D11PixelShaderPtr &d3dShader)
            {
                TResult ret = T3D_OK;
                
                do
                {
                    size_t bytecodeLength = 0;
                    const char *bytecode = shader->getBytesCode(bytecodeLength);
                    ID3D11PixelShader *pD3DShader = nullptr;
                    HRESULT hr = mD3DDevice->CreatePixelShader(bytecode, bytecodeLength, nullptr, &pD3DShader);
                    if (FAILED(hr))
                    {
                        T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Failed to create pixel shader ! DX ERROR [%d]", hr);
                        ret = T3D_ERR_D3D11_CREATE_PIXEL_SHADER;
                        break;
                    }

                    d3dShader->D3DShader = pD3DShader;
                } while (false);

                return ret;
            };

            TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, ShaderVariantPtr(shader), d3dShader);
            if (T3D_FAILED(ret))
            {
                d3dShader = nullptr;
                break;
            }
        } while (false);
        
        return d3dShader;
    }

    //--------------------------------------------------------------------------
    
    TResult D3D11Context::setPixelShader(ShaderVariant *shader)
    {
        D3D11PixelShaderPtr d3dShader = static_cast<D3D11PixelShader*>(shader->getRHIShader());
        
        auto lambda = [this](const D3D11PixelShaderPtr &d3dShader)
        {
            mD3DDeviceContext->PSSetShader(d3dShader->D3DShader, nullptr, 0);
            return T3D_OK;
        };
        
        return ENQUEUE_UNIQUE_COMMAND(lambda, d3dShader);
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::setPSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers)
    {
        return setConstantBuffers(&ID3D11DeviceContext::PSSetConstantBuffers, startSlot, buffers);
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::setPSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers)
    {
        return setPixelBuffers(&ID3D11DeviceContext::PSSetShaderResources, startSlot, buffers);
    }
    
    //--------------------------------------------------------------------------
    
    TResult D3D11Context::setPSSamplers(uint32_t startSlot, const Samplers &samplers)
    {
        return setSamplers(&ID3D11DeviceContext::PSSetSamplers, startSlot, samplers);
    }

    //--------------------------------------------------------------------------
    RHIShaderPtr D3D11Context::createHullShader(ShaderVariant *shader)
    {
        D3D11HullShaderPtr d3dShader = D3D11HullShader::create();

        do
        {
            auto lambda = [this](const ShaderVariantPtr &shader, const D3D11HullShaderPtr &d3dShader)
            {
                TResult ret = T3D_OK;
                
                do
                {
                    size_t bytecodeLength = 0;
                    const char *bytecode = shader->getBytesCode(bytecodeLength);
                    ID3D11HullShader *pD3DShader = nullptr;
                    HRESULT hr = mD3DDevice->CreateHullShader(bytecode, bytecodeLength, nullptr, &pD3DShader);
                    if (FAILED(hr))
                    {
                        T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Failed to create hull shader ! DX ERROR [%d]", hr);
                        ret = T3D_ERR_D3D11_CREATE_HULL_SHADER;
                        break;
                    }

                    d3dShader->D3DShader = pD3DShader;
                } while (false);

                return ret;
            };

            TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, ShaderVariantPtr(shader), d3dShader);
            if (T3D_FAILED(ret))
            {
                d3dShader = nullptr;
                break;
            }
        } while (false);
        
        return d3dShader;
    }

    //--------------------------------------------------------------------------
    
    TResult D3D11Context::setHullShader(ShaderVariant *shader)
    {
        D3D11HullShaderPtr d3dShader = static_cast<D3D11HullShader*>(shader->getRHIShader());
        
        auto lambda = [this](const D3D11HullShaderPtr &d3dShader)
        {
            mD3DDeviceContext->HSSetShader(d3dShader->D3DShader, nullptr, 0);
            return T3D_OK;
        };
        
        return ENQUEUE_UNIQUE_COMMAND(lambda, d3dShader);
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::setHSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers)
    {
        return setConstantBuffers(&ID3D11DeviceContext::HSSetConstantBuffers, startSlot, buffers);
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::setHSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers)
    {
        return setPixelBuffers(&ID3D11DeviceContext::HSSetShaderResources, startSlot, buffers);
    }
    
    //--------------------------------------------------------------------------
    
    TResult D3D11Context::setHSSamplers(uint32_t startSlot, const Samplers &samplers)
    {
        return setSamplers(&ID3D11DeviceContext::HSSetSamplers, startSlot, samplers);
    }

    //--------------------------------------------------------------------------
    
    RHIShaderPtr D3D11Context::createDomainShader(ShaderVariant *shader)
    {
        D3D11DomainShaderPtr d3dShader = D3D11DomainShader::create();

        do
        {
            auto lambda = [this](const ShaderVariantPtr &shader, const D3D11DomainShaderPtr &d3dShader)
            {
                TResult ret = T3D_OK;
                
                do
                {
                    size_t bytecodeLength = 0;
                    const char *bytecode = shader->getBytesCode(bytecodeLength);
                    ID3D11DomainShader *pD3DShader = nullptr;
                    HRESULT hr = mD3DDevice->CreateDomainShader(bytecode, bytecodeLength, nullptr, &pD3DShader);
                    if (FAILED(hr))
                    {
                        T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Failed to create domain shader ! DX ERROR [%d]", hr);
                        ret = T3D_ERR_D3D11_CREATE_DOMAIN_SHADER;
                        break;
                    }

                    d3dShader->D3DShader = pD3DShader;
                } while (false);

                return ret;
            };

            TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, ShaderVariantPtr(shader), d3dShader);
            if (T3D_FAILED(ret))
            {
                d3dShader = nullptr;
                break;
            }
        } while (false);
        
        return d3dShader;
    }

    //--------------------------------------------------------------------------
    
    TResult D3D11Context::setDomainShader(ShaderVariant *shader)
    {
        D3D11DomainShaderPtr d3dShader = static_cast<D3D11DomainShader*>(shader->getRHIShader());
        
        auto lambda = [this](const D3D11DomainShaderPtr &d3dShader)
        {
            mD3DDeviceContext->DSSetShader(d3dShader->D3DShader, nullptr, 0);
            return T3D_OK;
        };
        
        return ENQUEUE_UNIQUE_COMMAND(lambda, d3dShader);
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::setDSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers)
    {
        return setConstantBuffers(&ID3D11DeviceContext::DSSetConstantBuffers, startSlot, buffers);
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::setDSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers)
    {
        return setPixelBuffers(&ID3D11DeviceContext::DSSetShaderResources, startSlot, buffers);
    }
    
    //--------------------------------------------------------------------------
    
    TResult D3D11Context::setDSSamplers(uint32_t startSlot, const Samplers &samplers)
    {
        return setSamplers(&ID3D11DeviceContext::DSSetSamplers, startSlot, samplers);
    }

    //--------------------------------------------------------------------------
    
    RHIShaderPtr D3D11Context::createGeometryShader(ShaderVariant *shader)
    {
        D3D11GeometryShaderPtr d3dShader = D3D11GeometryShader::create();

        do
        {
            auto lambda = [this](const ShaderVariantPtr &shader, const D3D11GeometryShaderPtr &d3dShader)
            {
                TResult ret = T3D_OK;
                
                do
                {
                    size_t bytecodeLength = 0;
                    const char *bytecode = shader->getBytesCode(bytecodeLength);
                    ID3D11GeometryShader *pD3DShader = nullptr;
                    HRESULT hr = mD3DDevice->CreateGeometryShader(bytecode, bytecodeLength, nullptr, &pD3DShader);
                    if (FAILED(hr))
                    {
                        T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Failed to create geometry shader ! DX ERROR [%d]", hr);
                        ret = T3D_ERR_D3D11_CREATE_GEOMETRY_SHADER;
                        break;
                    }

                    d3dShader->D3DShader = pD3DShader;
                } while (false);

                return ret;
            };

            TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, ShaderVariantPtr(shader), d3dShader);
            if (T3D_FAILED(ret))
            {
                d3dShader = nullptr;
                break;
            }
        } while (false);
        
        return d3dShader;
    }

    //--------------------------------------------------------------------------
    
    TResult D3D11Context::setGeometryShader(ShaderVariant *shader)
    {
        D3D11GeometryShaderPtr d3dShader = static_cast<D3D11GeometryShader*>(shader->getRHIShader());
                
        auto lambda = [this](const D3D11GeometryShaderPtr &d3dShader)
        {
            mD3DDeviceContext->GSSetShader(d3dShader->D3DShader, nullptr, 0);
            return T3D_OK;
        };
        
        return ENQUEUE_UNIQUE_COMMAND(lambda, d3dShader);
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::setGSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers)
    {
        return setConstantBuffers(&ID3D11DeviceContext::GSSetConstantBuffers, startSlot, buffers);
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::setGSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers)
    {
        return setPixelBuffers(&ID3D11DeviceContext::GSSetShaderResources, startSlot, buffers);
    }
    
    //--------------------------------------------------------------------------
    
    TResult D3D11Context::setGSSamplers(uint32_t startSlot, const Samplers &samplers)
    {
        return setSamplers(&ID3D11DeviceContext::GSSetSamplers, startSlot, samplers);;
    }

    //--------------------------------------------------------------------------
    
    RHIShaderPtr D3D11Context::createComputeShader(ShaderVariant *shader)
    {
        D3D11ComputeShaderPtr d3dShader = D3D11ComputeShader::create();

        do
        {
            auto lambda = [this](const ShaderVariantPtr &shader, const D3D11ComputeShaderPtr &d3dShader)
            {
                TResult ret = T3D_OK;
                
                do
                {
                    size_t bytecodeLength = 0;
                    const char *bytecode = shader->getBytesCode(bytecodeLength);
                    ID3D11ComputeShader *pD3DShader = nullptr;
                    HRESULT hr = mD3DDevice->CreateComputeShader(bytecode, bytecodeLength, nullptr, &pD3DShader);
                    if (FAILED(hr))
                    {
                        T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Failed to create compute shader ! DX ERROR [%d]", hr);
                        ret = T3D_ERR_D3D11_CREATE_COMPUTE_SHADER;
                        break;
                    }

                    d3dShader->D3DShader = pD3DShader;
                } while (false);

                return ret;
            };

            TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, ShaderVariantPtr(shader), d3dShader);
            if (T3D_FAILED(ret))
            {
                d3dShader = nullptr;
                break;
            }
        } while (false);
        
        return d3dShader;
    }

    //--------------------------------------------------------------------------
    
    TResult D3D11Context::setComputeShader(ShaderVariant *shader)
    {
        D3D11ComputeShaderPtr d3dShader = static_cast<D3D11ComputeShader*>(shader->getRHIShader());
                
        auto lambda = [this](const D3D11ComputeShaderPtr &d3dShader)
        {
            mD3DDeviceContext->CSSetShader(d3dShader->D3DShader, nullptr, 0);
            return T3D_OK;
        };
        
        return ENQUEUE_UNIQUE_COMMAND(lambda, d3dShader);
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::setCSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers)
    {
        return setConstantBuffers(&ID3D11DeviceContext::CSSetConstantBuffers, startSlot, buffers);
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::setCSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers)
    {
        return setPixelBuffers(&ID3D11DeviceContext::CSSetShaderResources, startSlot, buffers);
    }
    
    //--------------------------------------------------------------------------
    
    TResult D3D11Context::setCSSamplers(uint32_t startSlot, const Samplers &samplers)
    {
        return setSamplers(&ID3D11DeviceContext::CSSetSamplers, startSlot, samplers);
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

#if defined (T3D_DEBUG)
            UINT shaderCompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_OPTIMIZATION_LEVEL0;
#else
            UINT shaderCompileFlags = 0;
#endif

            String sourceName = shader->getPass()->getTechnique()->getShader()->getName();
            sourceName = shader->getShaderKeyword().getName() + (!shader->getShaderKeyword().getName().empty() ? "-" : "") + profile + "-" + sourceName;

            ID3DBlob *shaderBlob = nullptr;
            ID3DBlob *errorBlob = nullptr;
            HRESULT hr = D3DCompile(bytes, bytesLength, sourceName.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", profile.c_str(), shaderCompileFlags, 0, &shaderBlob, &errorBlob);
            if (FAILED(hr))
            {
                ret = T3D_ERR_D3D11_COMPILE_SHADER;
                String error;
                if (errorBlob != nullptr)
                {
                    error.assign(static_cast<const char *>(errorBlob->GetBufferPointer()), errorBlob->GetBufferSize());
                }
                T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Compile shader failed ! (Keyword:%s, Target:%s, ) DX ERROR [%d] (%s)", shader->getShaderKeyword().getName().c_str(), profile.c_str(), hr, error.c_str());
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

    TResult D3D11Context::reflectShaderAllBindings(ShaderVariant *shader, ShaderConstantParams &constantParams, ShaderSamplerParams &samplerParams)
    {
        TResult ret = T3D_OK;

        do
        {
            auto getOriginalName = [](const String &cbufferName, const String &cname, String &originalCBufferName, String &originalCName)
            {
                if (cbufferName.empty() || cname.empty())
                {
                    return false;
                }

                String::size_type offset = 0, count = 0;
                
                if (cbufferName == "type_Globals")
                {
                    // 全局常量缓冲区，全局常量缓冲区的常量命名是以 "_Globals" 开头的
                    offset = 4;
                    count =  cbufferName.size() - offset;
                }
                else
                {
                    // 非全局的，这个命名规则不一样
                    offset = 5;
                    count =  cbufferName.size() - offset;
                }

                // 提取原始的常量缓冲区名称
                originalCBufferName = cbufferName.substr(offset, count);

                // 提取原始的常量名称
                offset = originalCBufferName.size() + 1;
                count = cname.size() - offset;
                originalCName = cname.substr(offset, count);

                return true;
            };
            
            // 创建 shader 字节码 D3D11 对象
            size_t bytesLength = 0;
            const char *bytes = shader->getBytesCode(bytesLength);
            ID3DBlob *pShaderBlob = nullptr;
            HRESULT hr = D3DCreateBlob(bytesLength, &pShaderBlob);
            if (FAILED(hr))
            {
                ret = T3D_ERR_D3D11_CREATE_BLOB;
                T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Create blob with shader code failed ! DX ERROR [%d]", hr);
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
                T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Reflect shader failed ! DX ERROR [%d]", hr);
                D3D_SAFE_RELEASE(pShaderBlob);
                break;
            }

            // 获取 shader 信息
            D3D11_SHADER_DESC shaderDesc;
            hr = pReflection->GetDesc(&shaderDesc);
            if (FAILED(hr))
            {
                ret = T3D_ERR_D3D11_GET_SHADER_DESC;
                T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Get shader description failed ! DX ERROR [%d]", hr);
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
                        // if (strncmp(bindDesc.Name, TINY3D_CBUFFER_PER_DRAW, TINY3D_CBUFFER_PER_DRAW_LEN) == 0
                        //     || strncmp(bindDesc.Name, TINY3D_CBUFFER_PER_FRAME, TINY3D_CBUFFER_PER_FRAME_LEN) == 0)
                        // {
                        //     // 这里跳开两个 cbuffer ，这两个 cbuffer 内部使用，不反射给外部使用
                        //     break;
                        // }

                        // ShaderConstantBinding constBinding;
                        // constBinding.name = bindDesc.Name;
                        // constBinding.binding = bindDesc.BindPoint;

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

                            // 常量类型
                            ID3D11ShaderReflectionType *d3dSRType = pVariableReflection->GetType();
                            D3D11_SHADER_TYPE_DESC d3dSRTypeDesc;
                            d3dSRType->GetDesc(&d3dSRTypeDesc);

                            String cbufferName, cname;
                            bool rval = getOriginalName(bindDesc.Name, variableDesc.Name, cbufferName, cname);
                            T3D_ASSERT(rval);
                            ShaderConstantParamPtr param = ShaderConstantParam::create(cbufferName, cname, bindDesc.BindPoint, variableDesc.Size, variableDesc.StartOffset, D3D11Mapping::get(d3dSRTypeDesc.Type, d3dSRTypeDesc.Rows, d3dSRTypeDesc.Columns, d3dSRTypeDesc.Elements));
                            constantParams.emplace(param->getName(), param);
                            
                            // ShaderVariableBinding varBinding;
                            // varBinding.name = variableDesc.Name;
                            // varBinding.offset = variableDesc.StartOffset;
                            // varBinding.size = variableDesc.Size;
                            // varBinding.type = D3D11Mapping::get(d3dSRTypeDesc.Type, d3dSRTypeDesc.Rows, d3dSRTypeDesc.Columns);
                            // size += varBinding.size;
                            // constBinding.variables.emplace(varBinding.name, varBinding);

                            T3D_LOG_DEBUG(LOG_TAG_D3D11RENDERER, "Shader reflection - cbuffer name : %s, variable name : %s, type : %u, size : %u, offset : %u", param->getCBufferName().c_str(), param->getName().c_str(), param->getDataType(), param->getDataSize(), param->getDataOffset());
                        }

                        // constBinding.size = size;
                        // constantBindings.emplace(constBinding.name, constBinding);
                    }
                    break;
                case D3D_SIT_TEXTURE:   // 纹理
                    {
                        String name = bindDesc.Name;

                        // auto itr = texSamplerBindings.find(name);
                        // if (itr == texSamplerBindings.end())
                        // {
                        //     // 没有，则新建一个
                        //     ShaderTexSamplerBinding texSamplerBinding;
                        //     texSamplerBinding.texBinding.name = name;
                        //     texSamplerBinding.texBinding.binding = bindDesc.BindPoint;
                        //     texSamplerBinding.texBinding.bindingCount = bindDesc.BindCount;
                        //     texSamplerBinding.texBinding.texType = D3D11Mapping::get(bindDesc.Dimension);
                        //
                        //     texSamplerBindings.emplace(name, texSamplerBinding);
                        //
                        //     T3D_LOG_DEBUG(LOG_TAG_D3D11RENDERER, "Shader reflection - New (name:%s). texture name : %s, binding point : %d, binding count : %d, texture type : %d",
                        //         name.c_str(), texSamplerBinding.texBinding.name.c_str(), texSamplerBinding.texBinding.binding, texSamplerBinding.texBinding.bindingCount, texSamplerBinding.texBinding.texType);
                        // }
                        // else
                        // {
                        //     // 已有，用已有的
                        //     ShaderTexSamplerBinding &texSamplerBinding = itr->second;
                        //     texSamplerBinding.texBinding.name = name;
                        //     texSamplerBinding.texBinding.binding = bindDesc.BindPoint;
                        //     texSamplerBinding.texBinding.bindingCount = bindDesc.BindCount;
                        //     texSamplerBinding.texBinding.texType = D3D11Mapping::get(bindDesc.Dimension);
                        //     T3D_LOG_DEBUG(LOG_TAG_D3D11RENDERER, "Shader reflection - Already exists (name:%s). texture name : %s, binding point : %d, binding count : %d, texture type : %d",
                        //         name.c_str(), texSamplerBinding.texBinding.name.c_str(), texSamplerBinding.texBinding.binding, texSamplerBinding.texBinding.bindingCount, texSamplerBinding.texBinding.texType);
                        // }

                        ShaderSamplerParamPtr param;
                        const auto itr = samplerParams.find(name);
                        if (itr == samplerParams.end())
                        {
                            // 没有，新建一个
                            param = ShaderSamplerParam::create(name);
                            samplerParams.emplace(name, param);
                        }
                        else
                        {
                            // 已有，更新信息
                            param = itr->second;
                        }

                        T3D_ASSERT(param != nullptr);

                        param->setTexBinding(bindDesc.BindPoint);
                        param->setTextureType(D3D11Mapping::get(bindDesc.Dimension));

                        T3D_LOG_DEBUG(LOG_TAG_D3D11RENDERER, "Shader reflection - Name:%s, texture binding point : %d, texture type : %d", param->getName().c_str(), param->getTexBinding(), param->getTextureType());
                    }
                    break;
                case D3D_SIT_SAMPLER:   // 纹理采样器
                    {
                        String name = bindDesc.Name;

                        if (!StringUtil::startsWith(name, "sampler"))
                        {
                            // sampler 一定要以 sampler_ 开头
                            ret = T3D_ERR_D3D11_INVALID_SHADER_SAMPLER_NAME;
                            T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Invalid shader sampler name (%s) ! It must start with 'sampler_' !", name.c_str());
                            break;
                        }

                        String key = name.substr(7);
                        // auto itr = texSamplerBindings.find(key);
                        // if (itr == texSamplerBindings.end())
                        // {
                        //     // 没有，则新建一个
                        //     ShaderTexSamplerBinding texSamplerBinding;
                        //     texSamplerBinding.samplerBinding.name = name;
                        //     texSamplerBinding.samplerBinding.binding = bindDesc.BindPoint;
                        //
                        //     texSamplerBindings.emplace(key, texSamplerBinding);
                        //     T3D_LOG_DEBUG(LOG_TAG_D3D11RENDERER, "Shader reflection - New (name:%s, key:%s). sampler name : %s, binding point : %d",
                        //         name.c_str(), key.c_str(), texSamplerBinding.samplerBinding.name.c_str(), texSamplerBinding.samplerBinding.binding);
                        // }
                        // else
                        // {
                        //     // 已有，用已有的
                        //     ShaderTexSamplerBinding &texSamplerBinding = itr->second;
                        //     texSamplerBinding.samplerBinding.name = name;
                        //     texSamplerBinding.samplerBinding.binding = bindDesc.BindPoint;
                        //     T3D_LOG_DEBUG(LOG_TAG_D3D11RENDERER, "Shader reflection - Already exists (name:%s, key:%s). sampler name : %s, binding point : %d",
                        //         name.c_str(), key.c_str(), texSamplerBinding.samplerBinding.name.c_str(), texSamplerBinding.samplerBinding.binding);
                        // }

                        ShaderSamplerParamPtr param;
                        const auto itr = samplerParams.find(key);
                        if (itr == samplerParams.end())
                        {
                            // 没有，新建一个
                            param = ShaderSamplerParam::create(key);
                            samplerParams.emplace(key, param);
                        }
                        else
                        {
                            // 已有，更新信息
                            param = itr->second;
                        }

                        T3D_ASSERT(param != nullptr);

                        param->setSamplerBinding(bindDesc.BindPoint);

                        T3D_LOG_DEBUG(LOG_TAG_D3D11RENDERER, "Shader reflection - Name:%s, sampler binding point : %d", param->getName().c_str(), param->getSamplerBinding());
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

    TResult D3D11Context::setPrimitiveType(PrimitiveType primitive)
    {
        D3D11_PRIMITIVE_TOPOLOGY topology = D3D11Mapping::get(primitive);
        
        auto lambda = [this](D3D11_PRIMITIVE_TOPOLOGY topology)
        {
            mD3DDeviceContext->IASetPrimitiveTopology(topology);
            return T3D_OK;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, topology);
    }

    //--------------------------------------------------------------------------
    
    TResult D3D11Context::render(uint32_t indexCount, uint32_t startIndex, uint32_t baseVertex)
    {
        auto lambda = [this](uint32_t indexCount, uint32_t startIndex, uint32_t baseVertex)
        {
            mD3DDeviceContext->DrawIndexed(indexCount, startIndex, baseVertex);
            return T3D_OK;
        };
        
        return ENQUEUE_UNIQUE_COMMAND(lambda, indexCount, startIndex, baseVertex);
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::render(uint32_t vertexCount, uint32_t startVertex)
    {
        auto lambda = [this](uint32_t vertexCount, uint32_t startVertex)
        {
            mD3DDeviceContext->Draw(vertexCount, startVertex);
            return T3D_OK;
        };
        
        return ENQUEUE_UNIQUE_COMMAND(lambda, vertexCount, startVertex);
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::reset()
    {
        mCurrentRenderWindow = nullptr;
        mCurrentRenderTexture = nullptr;

        auto lambda = [this]()
        { 
            // mD3DDeviceContext->OMSetRenderTargets(0, nullptr, nullptr);
            // mD3DDeviceContext->OMSetBlendState(nullptr, nullptr, -1);
            // mD3DDeviceContext->OMSetDepthStencilState(nullptr, 0);
            // mD3DDeviceContext->RSSetState(nullptr);
            // mD3DDeviceContext->VSSetSamplers(0, 0, nullptr);

            // Restore modified DX state
            mD3DDeviceContext->OMSetRenderTargets(1, &mBackupState.RenderTargetView, mBackupState.DepthStencilView);
            D3D_SAFE_RELEASE(mBackupState.RenderTargetView);
            D3D_SAFE_RELEASE(mBackupState.DepthStencilView);
            mD3DDeviceContext->RSSetScissorRects(mBackupState.ScissorRectsCount, mBackupState.ScissorRects);
            mD3DDeviceContext->RSSetViewports(mBackupState.ViewportsCount, mBackupState.Viewports);
            mD3DDeviceContext->RSSetState(mBackupState.RS); if (mBackupState.RS) mBackupState.RS->Release();
            mD3DDeviceContext->OMSetBlendState(mBackupState.BlendState, mBackupState.BlendFactor, mBackupState.SampleMask);
            D3D_SAFE_RELEASE(mBackupState.BlendState);
            mD3DDeviceContext->OMSetDepthStencilState(mBackupState.DepthStencilState, mBackupState.StencilRef);
            D3D_SAFE_RELEASE(mBackupState.DepthStencilState);
            mD3DDeviceContext->PSSetShaderResources(0, 1, &mBackupState.PSShaderResource);
            D3D_SAFE_RELEASE(mBackupState.PSShaderResource);
            mD3DDeviceContext->PSSetSamplers(0, 1, &mBackupState.PSSampler);
            D3D_SAFE_RELEASE(mBackupState.PSSampler);
            mD3DDeviceContext->PSSetShader(mBackupState.PS, mBackupState.PSInstances, mBackupState.PSInstancesCount);
            D3D_SAFE_RELEASE(mBackupState.PS);
            for (UINT i = 0; i < mBackupState.PSInstancesCount; i++)
                D3D_SAFE_RELEASE(mBackupState.PSInstances[i]);
            mD3DDeviceContext->VSSetShader(mBackupState.VS, mBackupState.VSInstances, mBackupState.VSInstancesCount);
            D3D_SAFE_RELEASE(mBackupState.VS);
            mD3DDeviceContext->VSSetConstantBuffers(0, 1, &mBackupState.VSConstantBuffer);
            D3D_SAFE_RELEASE(mBackupState.VSConstantBuffer);
            mD3DDeviceContext->GSSetShader(mBackupState.GS, mBackupState.GSInstances, mBackupState.GSInstancesCount);
            D3D_SAFE_RELEASE(mBackupState.GS);
            for (UINT i = 0; i < mBackupState.VSInstancesCount; i++)
                D3D_SAFE_RELEASE(mBackupState.VSInstances[i]);
            mD3DDeviceContext->IASetPrimitiveTopology(mBackupState.PrimitiveTopology);
            mD3DDeviceContext->IASetIndexBuffer(mBackupState.IndexBuffer, mBackupState.IndexBufferFormat, mBackupState.IndexBufferOffset);
            D3D_SAFE_RELEASE(mBackupState.IndexBuffer);
            mD3DDeviceContext->IASetVertexBuffers(0, 1, &mBackupState.VertexBuffer, &mBackupState.VertexBufferStride, &mBackupState.VertexBufferOffset);
            D3D_SAFE_RELEASE(mBackupState.VertexBuffer);
            mD3DDeviceContext->IASetInputLayout(mBackupState.InputLayout);
            D3D_SAFE_RELEASE(mBackupState.InputLayout);
            return T3D_OK;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda);
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::blit(RenderTarget *src, RenderTarget *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::blit(Texture *src, RenderTarget *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)
    {
        TResult ret = T3D_OK;

        switch (dst->getType())
        {
        case RenderTarget::Type::E_RT_WINDOW:
            {
                D3D11RenderWindow *pDst = static_cast<D3D11RenderWindow*>(dst->getRenderWindow()->getRHIRenderWindow());
                auto lambda = [this](const TexturePtr &pSrc, const D3D11RenderWindowPtr &pDst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 &dstOffset)
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
                            if (pD3DPixelBuffer->D3DResolveTex != nullptr)
                            {
                                DXGI_FORMAT d3dFormat = D3D11Mapping::get(pTex2D->getPixelFormat());
                                mD3DDeviceContext->ResolveSubresource(pD3DPixelBuffer->D3DResolveTex, 0, pD3DPixelBuffer->D3DTexture, 0, d3dFormat);
                                pD3DSrc = pD3DPixelBuffer->D3DResolveTex;
                            }
                            else
                            {
                                pD3DSrc = pD3DPixelBuffer->D3DTexture;
                            }
                        }
                        break;
                    }
                    if (size == Vector3::ZERO)
                    {
                        // 复制全部
                        ret = blitAll(pD3DSrc, pDst->D3DBackBuffer);
                    }
                    else
                    {
                        // 按区域复制
                        ret = blitRegion(pD3DSRV, pDst->D3DRTView, pDst->D3DDSView, srcOffset, size, dstOffset);
                    }

                    return ret;
                };

                ret = ENQUEUE_UNIQUE_COMMAND(lambda, TexturePtr(src), D3D11RenderWindowPtr(pDst), srcOffset, size, dstOffset);
            }
            break;
        case RenderTarget::Type::E_RT_TEXTURE:
            {
                D3D11PixelBuffer2D *pD3DPixelBuffer = static_cast<D3D11PixelBuffer2D*>(dst->getRenderTexture()->getPixelBuffer()->getRHIResource().get());

                auto lambda = [this](const TexturePtr &pSrc, const D3D11PixelBuffer2DPtr &pDst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 &dstOffset)
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
                            if (pD3DPixelBuffer->D3DResolveTex != nullptr)
                            {
                                DXGI_FORMAT d3dFormat = D3D11Mapping::get(pTex2D->getPixelFormat());
                                mD3DDeviceContext->ResolveSubresource(pD3DPixelBuffer->D3DResolveTex, 0, pD3DPixelBuffer->D3DTexture, 0, d3dFormat);
                                pD3DSrc = pD3DPixelBuffer->D3DResolveTex;
                            }
                            else
                            {
                                pD3DSrc = pD3DPixelBuffer->D3DTexture;
                            }
                        }
                        break;
                    }
                    if (size == Vector3::ZERO)
                    {
                        // 复制全部
                        ret = blitAll(pD3DSrc, pDst->D3DTexture);
                    }
                    else
                    {
                        // 按区域复制
                        ret = blitRegion(pD3DSRV, pDst->D3DRTView, pDst->D3DDSView, srcOffset, size, dstOffset);
                    }
                    
                    return ret;
                };

                ret = ENQUEUE_UNIQUE_COMMAND(lambda, TexturePtr(src), D3D11PixelBuffer2DPtr(pD3DPixelBuffer), srcOffset, size, dstOffset);
            }
            break;
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::blit(RenderTarget *src, Texture *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::blit(Texture *src, Texture *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::copyBuffer(RenderBuffer *src, RenderBuffer *dst, size_t srcOffset, size_t size, size_t dstOffset)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::writeBuffer(RenderBuffer *renderBuffer, const Buffer &buffer, bool discardWholeBuffer)
    {
        TResult ret = T3D_OK;

        do
        {
            if (renderBuffer->getUsage() != Usage::kDynamic)
            {
                ret = T3D_ERR_D3D11_INVALID_USAGE;
                T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Usage must be kDynamic when write buffer ! Usage [%d]", renderBuffer->getUsage());
                break;
            }
            
            auto lambda = [this](const RenderBufferPtr &renderBuffer, Buffer &buffer, bool discardWholeBuffer)
            {
                TResult ret = T3D_OK;
                
                do
                {
                    ID3D11Resource *pD3DResource = nullptr;
                    
                    switch (renderBuffer->getRHIResource()->getResourceType())
                    {
                    case RenderResource::Type::kVertexBuffer:   /// 顶点缓冲
                        pD3DResource = smart_pointer_cast<D3D11VertexBuffer>(renderBuffer->getRHIResource())->D3DBuffer;
                        break;
                    case RenderResource::Type::kIndexBuffer:    /// 索引缓冲
                        pD3DResource = smart_pointer_cast<D3D11IndexBuffer>(renderBuffer->getRHIResource())->D3DBuffer;
                        break;
                    case RenderResource::Type::kPixelBuffer1D: /// 像素缓冲
                        pD3DResource = smart_pointer_cast<D3D11PixelBuffer1D>(renderBuffer->getRHIResource())->D3DTexture;
                        break;
                    case RenderResource::Type::kPixelBuffer2D:
                        pD3DResource = smart_pointer_cast<D3D11PixelBuffer2D>(renderBuffer->getRHIResource())->D3DTexture;
                        break;
                    case RenderResource::Type::kPixelBuffer3D:
                        pD3DResource = smart_pointer_cast<D3D11PixelBuffer3D>(renderBuffer->getRHIResource())->D3DTexture;
                        break;
                    case RenderResource::Type::kPixelBufferCubemap:
                        pD3DResource = smart_pointer_cast<D3D11PixelBuffer2D>(renderBuffer->getRHIResource())->D3DTexture;
                        break;
                    case RenderResource::Type::kConstantBuffer: /// 常量缓冲
                        pD3DResource = smart_pointer_cast<D3D11ConstantBuffer>(renderBuffer->getRHIResource())->D3DBuffer;
                        break;
                    }

                    D3D11_MAP d3dMapType = discardWholeBuffer ? D3D11_MAP_WRITE_DISCARD : D3D11_MAP_WRITE_NO_OVERWRITE;
                    D3D11_MAPPED_SUBRESOURCE d3dMapData;
                    memset(&d3dMapData, 0, sizeof(d3dMapData));

                    // map buffer
                    HRESULT hr = mD3DDeviceContext->Map(pD3DResource, 0, d3dMapType, 0, &d3dMapData);
                    if (FAILED(hr))
                    {
                        T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Failed to map data when write buffer ! DX ERROR [%d]", hr);
                        ret = T3D_ERR_D3D11_MAP_RESOURCE;
                        break;
                    }

                    // write data
                    memcpy(d3dMapData.pData, buffer.Data, buffer.DataSize);
                    
                    // unmap buffer
                    mD3DDeviceContext->Unmap(pD3DResource, 0);

                    //buffer.release();
                } while (false);

                return ret;
            };

            ret = ENQUEUE_UNIQUE_COMMAND(lambda, RenderBufferPtr(renderBuffer), buffer, discardWholeBuffer);
            if (T3D_FAILED(ret))
            {
                break;
            }
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::blitRegion(ID3D11ShaderResourceView *pD3DSRV, ID3D11RenderTargetView *pD3DRTView, ID3D11DepthStencilView *pD3DDSView, const Vector3 &srcOffset, const Vector3 &size, const Vector3 &dstOffset)
    {
        // current render target
        ID3D11RenderTargetView *pCurRTV = nullptr;
        ID3D11DepthStencilView *pCurDSV = nullptr;
        mD3DDeviceContext->OMGetRenderTargets(1, &pCurRTV, &pCurDSV);

        // current viewport
        UINT numOfViewports = 1;
        D3D11_VIEWPORT originalVP;
        mD3DDeviceContext->RSGetViewports(&numOfViewports, &originalVP);

        // current blend state
        ID3D11BlendState *pCurBState = nullptr;
        float curBlendFactor[4];
        UINT curSampleMask = 0;
        mD3DDeviceContext->OMGetBlendState(&pCurBState, curBlendFactor, &curSampleMask);
        
        // current depth & stencil state
        ID3D11DepthStencilState *pCurDSState = nullptr;
        UINT curStencilRef = 0;
        mD3DDeviceContext->OMGetDepthStencilState(&pCurDSState, &curStencilRef);

        // current rasterizer state
        ID3D11RasterizerState *pCurRState = nullptr;
        mD3DDeviceContext->RSGetState(&pCurRState);

        // set render target
        // mD3DDeviceContext->OMSetRenderTargets(1, &pDst->D3DRTView, pDst->D3DDSView);
        mD3DDeviceContext->OMSetRenderTargets(1, &pD3DRTView, pD3DDSView);

        // blend state
        mD3DDeviceContext->OMSetBlendState(mBlitBState, nullptr, -1);

        // depth stencil state
        mD3DDeviceContext->OMSetDepthStencilState(mBlitDSState, 0);
        
        // rasterizer state
        mD3DDeviceContext->RSSetState(mBlitRState);

        // set viewport
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
        
        // 设置顶点和像素着色器
        mD3DDeviceContext->VSSetShader(mBlitVS, nullptr, 0);
        mD3DDeviceContext->PSSetShader(mBlitPS, nullptr, 0);

        // 绘制全屏四边形
        mD3DDeviceContext->Draw(4, 0);

        // reset all
        mD3DDeviceContext->PSSetSamplers(0, 0, nullptr);
        ID3D11ShaderResourceView *pNullRSV = nullptr;
        mD3DDeviceContext->PSSetShaderResources(0, 1, &pNullRSV);
        mD3DDeviceContext->VSSetShader(nullptr, nullptr, 0);
        mD3DDeviceContext->PSSetShader(nullptr, nullptr, 0);
        mD3DDeviceContext->OMSetRenderTargets(1, &pCurRTV, pCurDSV);
        mD3DDeviceContext->RSSetViewports(1, &originalVP);
        mD3DDeviceContext->OMSetBlendState(pCurBState, curBlendFactor, curSampleMask);
        mD3DDeviceContext->OMSetDepthStencilState(pCurDSState, curStencilRef);
        mD3DDeviceContext->RSSetState(pCurRState);

        D3D_SAFE_RELEASE(pCurDSV);
        D3D_SAFE_RELEASE(pCurRTV);
        D3D_SAFE_RELEASE(pCurBState);
        D3D_SAFE_RELEASE(pCurDSState);
        D3D_SAFE_RELEASE(pCurRState);
        
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::blitAll(ID3D11Resource *pD3DSrc, ID3D11Resource *pD3DDst)
    {        
        mD3DDeviceContext->CopyResource(pD3DDst, pD3DSrc);
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
    
    TResult D3D11Context::setSamplers(SetSamplerState setSamplerState, uint32_t startSlot, const Samplers &samplers)
    {
        auto lambda = [this](SetSamplerState setSamplerState, uint32_t startSlot, const Samplers &samplers)
        {
            using D3D11Samplers = TArray<ID3D11SamplerState*>;
            D3D11Samplers d3dSamplers(samplers.size());
            for (uint32_t i = 0; i< samplers.size(); ++i)
            {
                d3dSamplers[i] = ((D3D11SamplerState*)samplers[i]->getRHIState().get())->D3DSamplerState;
            }

            (mD3DDeviceContext->*setSamplerState)(startSlot, (UINT)d3dSamplers.size(), d3dSamplers.data());
            return T3D_OK;
        };
        
        return ENQUEUE_UNIQUE_COMMAND(lambda, setSamplerState, startSlot, samplers);
    }
    
    //--------------------------------------------------------------------------

    TResult D3D11Context::setPixelBuffers(SetShaderResources setShaderResources, uint32_t startSlot, const PixelBuffers &buffers)
    {
        auto lambda = [this](SetShaderResources setShaderResources, uint32_t startSlot, const PixelBuffers &buffers)
        {
            TArray<ID3D11ShaderResourceView*> d3dSRViews(buffers.size());
        
            for (uint32_t i = 0 ; i < buffers.size(); ++i)
            {
                const auto &buffer = buffers[i];
                
                switch (buffer->getRHIResource()->getResourceType())
                {
                case RHIResource::ResourceType::kPixelBuffer1D:
                    d3dSRViews[i] = smart_pointer_cast<D3D11PixelBuffer1D>(buffers[i]->getRHIResource())->D3DSRView;
                    break;
                case RHIResource::ResourceType::kPixelBuffer2D:
                    d3dSRViews[i] = smart_pointer_cast<D3D11PixelBuffer2D>(buffers[i]->getRHIResource())->D3DSRView;
                    break;
                case RHIResource::ResourceType::kPixelBuffer3D:
                    d3dSRViews[i] = smart_pointer_cast<D3D11PixelBuffer3D>(buffers[i]->getRHIResource())->D3DSRView;
                    break;
                case RHIResource::ResourceType::kPixelBufferCubemap:
                    d3dSRViews[i] = smart_pointer_cast<D3D11PixelBuffer2D>(buffers[i]->getRHIResource())->D3DSRView;
                    break;
                }
            }
            
            (mD3DDeviceContext->*setShaderResources)(startSlot, (UINT)d3dSRViews.size(), d3dSRViews.data());
            return T3D_OK;
        };
        
        return ENQUEUE_UNIQUE_COMMAND(lambda, setShaderResources, startSlot, buffers);
    }
    
    //--------------------------------------------------------------------------

    TResult D3D11Context::setConstantBuffers(SetConstantBuffers setConstantBuffers, uint32_t startSlot, const ConstantBuffers &buffers)
    {
        auto lambda = [this](SetConstantBuffers setConstantBuffers, uint32_t startSlot, const ConstantBuffers &buffers)
        {
            TArray<ID3D11Buffer*> cbuffers(buffers.size());
            for (uint32_t i = 0; i < buffers.size(); ++i)
            {
                const auto &cb = buffers[i];
                cbuffers[i] = smart_pointer_cast<D3D11ConstantBuffer>(cb->getRHIResource())->D3DBuffer;
            }

            (mD3DDeviceContext->*setConstantBuffers)(startSlot, (UINT)cbuffers.size(), cbuffers.data());
            return T3D_OK;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, setConstantBuffers, startSlot, buffers);
    }

    //--------------------------------------------------------------------------
}

