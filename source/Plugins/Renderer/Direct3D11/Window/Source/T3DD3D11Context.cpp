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
        D3D11ContextPtr ctx = T3D_NEW D3D11Context();
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
        mCurrentRenderTarget = nullptr;

        // Unbind all pipeline state to release internal references held by
        // the DeviceContext, then flush to ensure GPU has finished processing.
        if (mD3DDeviceContext != nullptr)
        {
            mD3DDeviceContext->ClearState();
            mD3DDeviceContext->Flush();
        }

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

        // 创建顶点缓冲区。用 DYNAMIC 而不是 IMMUTABLE，因为 blitRegion 需要
        // 每次按源矩形改写这 4 个顶点的 UV 才能支持 srcOffset
        D3D11_BUFFER_DESC bd;
        memset(&bd, 0, sizeof(bd));
        bd.Usage = D3D11_USAGE_DYNAMIC;
        bd.ByteWidth = sizeof(BlitVertex) * 4;
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
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
                hr = renderWindow->D3DSwapChain->Present(renderWindow->PresentSyncInterval, 0);
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
                    uMSAAQuality = 0;
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

                // Present 的 SyncInterval：1 表示等一次垂直回扫，0 表示立即呈现
                pD3DRenderWindow->PresentSyncInterval = desc.IsVsync ? 1 : 0;
            
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
            return buildRenderTextureResources(buffer.get(), d3dPixelBuffer.get());
        };

        TResult ret = ENQUEUE_UNIQUE_COMMAND(lambda, PixelBuffer2DPtr(buffer), D3D11PixelBuffer2DPtr(d3dPixelBuffer));
        if (T3D_FAILED(ret))
        {
            d3dPixelBuffer = nullptr;
        }
        
        return d3dPixelBuffer;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::buildRenderTextureResources(PixelBuffer2D *buffer, D3D11PixelBuffer2D *d3dPixelBuffer)
    {
        TResult ret = T3D_OK;

        do
        {
            if (buffer == nullptr || d3dPixelBuffer == nullptr)
            {
                T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "buildRenderTextureResources: buffer is null (dangling pointer?)");
                ret = T3D_ERR_INVALID_POINTER;
                break;
            }

            // 获取支持的 MSAA
            UINT uMSAACount = buffer->getDescriptor().sampleDesc.Count;
            UINT uMSAAQuality = buffer->getDescriptor().sampleDesc.Quality;

            DXGI_FORMAT format = D3D11Mapping::get(buffer->getDescriptor().format);

            if (uMSAACount == 0)
            {
                uMSAACount = 1;
                uMSAAQuality = 0;
            }
            else
            {
                UINT uNumQuality = 0;
                HRESULT hr = mD3DDevice->CheckMultisampleQualityLevels(format, uMSAACount, &uNumQuality);
                if (FAILED(hr))
                {
                    ret = T3D_ERR_D3D11_CHECK_MULTISAMPLE;
                    T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Failed to check multiple sample quality levels ! DX ERROR [%d] Count=%u", hr, uMSAACount);
                    break;
                }
                
                uMSAAQuality = uNumQuality - 1;
            }
            
            bool isColorRT = true;
            if (buffer->getDescriptor().format >= PixelFormat::E_PF_D24_UNORM_S8_UINT
                && buffer->getDescriptor().format <= PixelFormat::E_PF_D16_UNORM)
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
                UINT uBindFlags = D3D11_BIND_DEPTH_STENCIL;
                DXGI_FORMAT d3dTexFormat = DXGI_FORMAT_UNKNOWN;
                DXGI_FORMAT d3dDSVFormat = DXGI_FORMAT_UNKNOWN;
                DXGI_FORMAT d3dSRVFormat = DXGI_FORMAT_UNKNOWN;
                D3D11_SRV_DIMENSION srvDimension = D3D11_SRV_DIMENSION_UNKNOWN;
   
                if (buffer->getDescriptor().shaderReadable)
                {
                    // 纹理需要在 shader 中读取
                    uBindFlags |= D3D11_BIND_SHADER_RESOURCE;

                    switch (buffer->getDescriptor().format)
                    {
                    case PixelFormat::E_PF_D24_UNORM_S8_UINT:
                        d3dTexFormat = DXGI_FORMAT_R24G8_TYPELESS;
                        d3dSRVFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
                        break;
                    case PixelFormat::E_PF_D32_FLOAT_S8X24_UINT:
                        d3dTexFormat = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
                        d3dSRVFormat = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
                        break;
                    case PixelFormat::E_PF_D32_FLOAT:
                        d3dTexFormat = DXGI_FORMAT_R32_TYPELESS;
                        d3dSRVFormat = DXGI_FORMAT_R32_FLOAT;
                        break;
                    case PixelFormat::E_PF_D16_UNORM:
                        d3dTexFormat = DXGI_FORMAT_R16_TYPELESS;
                        d3dSRVFormat = DXGI_FORMAT_R16_UNORM;
                        break;
                    default:
                        T3D_ASSERT(false);
                        break;
                    }

                    // 给后续创建 SRV 用
                    d3dDSVFormat = D3D11Mapping::get(buffer->getDescriptor().format);

                    if (uMSAACount > 1)
                    {
                        srvDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
                    }
                    else
                    {
                        srvDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
                    }
                }
                else
                {
                    // 纹理不需要在 shader 中读取，后续不创建 SRV
                    d3dTexFormat = D3D11Mapping::get(buffer->getDescriptor().format);
                    d3dDSVFormat = d3dTexFormat;
                }
                
                // 创建 depth & stencil 纹理
                D3D11_TEXTURE2D_DESC depthStencilDesc = D3D11Mapping::get(buffer->getDescriptor());
                depthStencilDesc.SampleDesc.Count = uMSAACount;
                depthStencilDesc.SampleDesc.Quality = uMSAAQuality;
                depthStencilDesc.Format = d3dTexFormat;
                depthStencilDesc.Usage = D3D11Mapping::get(buffer->getUsage());
                depthStencilDesc.BindFlags = uBindFlags;
                depthStencilDesc.CPUAccessFlags = D3D11Mapping::get(buffer->getCPUAccessMode());
                depthStencilDesc.MiscFlags = 0;
                
                HRESULT hr = mD3DDevice->CreateTexture2D(&depthStencilDesc, nullptr, &d3dPixelBuffer->D3DTexture);
                if (FAILED(hr))
                {
                    // 错误
                    ret = T3D_ERR_D3D11_CREATE_TEXTURE2D;
                    T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Failed to craeteDepthStencilTexture when create render texture for depth & stencil ! DX ERROR [%d]", hr);
                    break;
                }

                D3D11_DSV_DIMENSION dsvDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
                if (uMSAACount > 1)
                {
                    dsvDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
                }
   
                // 创建 DSV
                D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
                memset(&dsvDesc, 0, sizeof(dsvDesc));
                dsvDesc.Format = d3dDSVFormat;
                dsvDesc.ViewDimension = dsvDimension;
                dsvDesc.Texture2D.MipSlice = 0;
                hr = mD3DDevice->CreateDepthStencilView(d3dPixelBuffer->D3DTexture, &dsvDesc, &d3dPixelBuffer->D3DDSView);
                if (FAILED(hr))
                {
                    // 错误
                    ret = T3D_ERR_D3D11_CREATE_DEPTH_STENCIL_VIEW;
                    T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Failed to create DSV when create render texture for depth & stencil ! DX ERROR [%d]", hr);
                    break;
                }

                if (buffer->getDescriptor().shaderReadable)
                {
                    // 创建 SRV
                    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
                    memset(&srvDesc, 0, sizeof(srvDesc));
                    srvDesc.Format = d3dSRVFormat;
                    srvDesc.ViewDimension = srvDimension;
                    srvDesc.Texture2D.MostDetailedMip = 0;
                    srvDesc.Texture2D.MipLevels = 1;
                    hr = mD3DDevice->CreateShaderResourceView(d3dPixelBuffer->D3DTexture, &srvDesc, &d3dPixelBuffer->D3DSRView);
                    if (FAILED(hr))
                    {
                        // 错误
                        ret = T3D_ERR_D3D11_CREATE_SHADER_RESOURCE_VIEW;
                        T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Failed to create SRV when create render texture for depth & stencil ! DX ERROR [%d]", hr);
                        break;
                    }
                }
                
            }
        } while (false);

        if (T3D_FAILED(ret) && d3dPixelBuffer != nullptr)
        {
            // 半成品资源不能留给上层，否则后面无从判断哪些视图是有效的
            D3D_SAFE_RELEASE(d3dPixelBuffer->D3DSRView);
            D3D_SAFE_RELEASE(d3dPixelBuffer->D3DRTView);
            D3D_SAFE_RELEASE(d3dPixelBuffer->D3DDSView);
            D3D_SAFE_RELEASE(d3dPixelBuffer->D3DResolveTex);
            D3D_SAFE_RELEASE(d3dPixelBuffer->D3DTexture);
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    void D3D11Context::releaseRenderTextureResources(D3D11PixelBuffer2D *d3dBuffer)
    {
        if (d3dBuffer == nullptr)
        {
            return;
        }

        // 释放前必须把管线上可能残留的绑定解掉，否则 D3D11 会保留内部引用，
        // 资源不会真正销毁，重建后还可能读到旧内容
        ID3D11RenderTargetView *nullRTViews[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = {};
        mD3DDeviceContext->OMSetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, nullRTViews, nullptr);

        ID3D11ShaderResourceView *nullSRViews[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = {};
        mD3DDeviceContext->VSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, nullSRViews);
        mD3DDeviceContext->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, nullSRViews);
        mD3DDeviceContext->CSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, nullSRViews);

        D3D_SAFE_RELEASE(d3dBuffer->D3DSRView);
        D3D_SAFE_RELEASE(d3dBuffer->D3DRTView);
        D3D_SAFE_RELEASE(d3dBuffer->D3DDSView);
        D3D_SAFE_RELEASE(d3dBuffer->D3DResolveTex);
        D3D_SAFE_RELEASE(d3dBuffer->D3DTexture);
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
            
            // 深度模板缓冲的尺寸与采样数必须和 BackBuffer 完全一致，否则
            // OMSetRenderTargets 会因 RTV/DSV 不匹配被 D3D11 拒绝。
            // ResizeBuffers(0, 0, 0, ...) 让 DXGI 自己取窗口客户区尺寸，
            // 实际尺寸可能与传入的 w/h 不同，所以这里从 BackBuffer 反查而不是信参数。
            D3D11_TEXTURE2D_DESC backBufferDesc;
            pD3DRenderWindow->D3DBackBuffer->GetDesc(&backBufferDesc);

            D3D11_TEXTURE2D_DESC d3dTexDesc;
            memset(&d3dTexDesc, 0, sizeof(d3dTexDesc));
            d3dTexDesc.Width = backBufferDesc.Width;
            d3dTexDesc.Height = backBufferDesc.Height;
            d3dTexDesc.MipLevels = 1;
            d3dTexDesc.ArraySize = 1;
            d3dTexDesc.Format = D3D11Mapping::get(pD3DRenderWindow->DepthStencilFormat);
            d3dTexDesc.SampleDesc = backBufferDesc.SampleDesc;
            d3dTexDesc.Usage = D3D11_USAGE_DEFAULT;
            d3dTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
            d3dTexDesc.CPUAccessFlags = 0;
            d3dTexDesc.MiscFlags = 0;

            hr = mD3DDevice->CreateTexture2D(&d3dTexDesc, nullptr, &pD3DRenderWindow->D3DDSBuffer);
            if (FAILED(hr))
            {
                ret = T3D_ERR_D3D11_CREATE_TEXTURE2D;
                T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Create depth stencil buffer failed ! DX ERROR [%d]", hr);
                break;
            }

            D3D11_DEPTH_STENCIL_VIEW_DESC d3dDSVDesc;
            memset(&d3dDSVDesc, 0, sizeof(d3dDSVDesc));
            d3dDSVDesc.Format = d3dTexDesc.Format;
            d3dDSVDesc.ViewDimension = (d3dTexDesc.SampleDesc.Count > 1) ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
            d3dDSVDesc.Texture2D.MipSlice = 0;

            hr = mD3DDevice->CreateDepthStencilView(pD3DRenderWindow->D3DDSBuffer, &d3dDSVDesc, &pD3DRenderWindow->D3DDSView);
            if (FAILED(hr))
            {
                D3D_SAFE_RELEASE(pD3DRenderWindow->D3DDSBuffer);
                ret = T3D_ERR_D3D11_CREATE_DEPTH_STENCIL_VIEW;
                T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Create depth stencil view failed ! DX ERROR [%d]", hr);
                break;
            }
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
        
        return ENQUEUE_UNIQUE_COMMAND(lambda, D3D11RenderWindowPtr(pD3DRenderWindow), w, h);
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::resizeRenderTexture(RenderTexture *rt, uint32_t w, uint32_t h)
    {
        if (rt == nullptr || w == 0 || h == 0)
        {
            T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "resizeRenderTexture : invalid render texture or size [%u x %u] !", w, h);
            return T3D_ERR_INVALID_PARAM;
        }

        PixelBuffer2D *pixelBuffer = static_cast<PixelBuffer2D *>(rt->getPixelBuffer());
        if (pixelBuffer == nullptr || pixelBuffer->getRHIResource() == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "resizeRenderTexture : render texture [%s] has no RHI resource !", rt->getName().c_str());
            return T3D_ERR_INVALID_POINTER;
        }

        // 调用方（RenderTexture::resize）已经把描述改成新尺寸，这里只做一致性校验
        if (pixelBuffer->getDescriptor().width != w || pixelBuffer->getDescriptor().height != h)
        {
            T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "resizeRenderTexture : descriptor [%u x %u] does not match requested size [%u x %u] !",
                pixelBuffer->getDescriptor().width, pixelBuffer->getDescriptor().height, w, h);
            return T3D_ERR_INVALID_PARAM;
        }

        D3D11PixelBuffer2D *d3dPixelBuffer = static_cast<D3D11PixelBuffer2D *>(pixelBuffer->getRHIResource().get());

        auto lambda = [this](const PixelBuffer2DPtr &pixelBuffer, const D3D11PixelBuffer2DPtr &d3dPixelBuffer)
        {
            releaseRenderTextureResources(d3dPixelBuffer.get());
            return buildRenderTextureResources(pixelBuffer.get(), d3dPixelBuffer.get());
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, PixelBuffer2DPtr(pixelBuffer), D3D11PixelBuffer2DPtr(d3dPixelBuffer));
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::resizeRenderTarget(RenderTarget *rt, uint32_t w, uint32_t h)
    {
        if (rt == nullptr || w == 0 || h == 0)
        {
            T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "resizeRenderTarget : invalid render target or size [%u x %u] !", w, h);
            return T3D_ERR_INVALID_PARAM;
        }

        if (rt->getType() == RenderTarget::Type::E_RT_WINDOW)
        {
            // 窗口型的颜色附件是 SwapChain BackBuffer，只能靠 ResizeBuffers 重建
            D3D11RenderWindow *pD3DRenderWindow = static_cast<D3D11RenderWindow *>(rt->getRenderWindow()->getRHIRenderWindow());
            if (pD3DRenderWindow == nullptr)
            {
                T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "resizeRenderTarget : render window has no RHI resource !");
                return T3D_ERR_INVALID_POINTER;
            }

            return resizeRenderWindow(pD3DRenderWindow, w, h);
        }

        // 纹理型逐个重建颜色附件与深度模板附件，任何一个失败都直接返回
        const uint32_t numOfTextures = rt->getNumOfRenderTextures();
        for (uint32_t i = 0; i < numOfTextures; ++i)
        {
            TResult ret = rt->getRenderTexture(i)->resize(w, h);
            if (T3D_FAILED(ret))
            {
                return ret;
            }
        }

        RenderTexturePtr depthStencil = rt->getDepthStencil();
        if (depthStencil != nullptr)
        {
            TResult ret = depthStencil->resize(w, h);
            if (T3D_FAILED(ret))
            {
                return ret;
            }
        }

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    void D3D11Context::backupRenderState()
    {
        // 裁剪矩形和视口
        mD3DDeviceContext->RSGetScissorRects(&mBackupState.ScissorRectsCount, mBackupState.ScissorRects);
        mD3DDeviceContext->RSGetViewports(&mBackupState.ViewportsCount, mBackupState.Viewports);

        // 渲染目标和深度模版
        mD3DDeviceContext->OMGetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, mBackupState.RenderTargetViews, &mBackupState.DepthStencilView);

        // 所有着色器资源（PS、VS、CS、HS、DS、GS）
        mD3DDeviceContext->PSGetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, mBackupState.PSShaderResources);
        mD3DDeviceContext->VSGetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, mBackupState.VSShaderResources);
        mD3DDeviceContext->GSGetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, mBackupState.GSShaderResources);
        mD3DDeviceContext->HSGetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, mBackupState.HSShaderResources);
        mD3DDeviceContext->DSGetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, mBackupState.DSShaderResources);
        mD3DDeviceContext->CSGetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, mBackupState.CSShaderResources);

        // 所有着色器
        mD3DDeviceContext->VSGetShader(&mBackupState.VS, mBackupState.VSInstances, &mBackupState.VSInstancesCount);
        mD3DDeviceContext->PSGetShader(&mBackupState.PS, mBackupState.PSInstances, &mBackupState.PSInstancesCount);
        mD3DDeviceContext->GSGetShader(&mBackupState.GS, mBackupState.GSInstances, &mBackupState.GSInstancesCount);
        mD3DDeviceContext->HSGetShader(&mBackupState.HS, mBackupState.HSInstances, &mBackupState.HSInstancesCount);
        mD3DDeviceContext->DSGetShader(&mBackupState.DS, mBackupState.DSInstances, &mBackupState.DSInstancesCount);
        mD3DDeviceContext->CSGetShader(&mBackupState.CS, mBackupState.CSInstances, &mBackupState.CSInstancesCount);

        // 所有常量缓冲区
        mD3DDeviceContext->VSGetConstantBuffers(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, mBackupState.VSConstantBuffers);
        mD3DDeviceContext->PSGetConstantBuffers(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, mBackupState.PSConstantBuffers);
        mD3DDeviceContext->GSGetConstantBuffers(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, mBackupState.GSConstantBuffers);
        mD3DDeviceContext->HSGetConstantBuffers(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, mBackupState.HSConstantBuffers);
        mD3DDeviceContext->DSGetConstantBuffers(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, mBackupState.DSConstantBuffers);
        mD3DDeviceContext->CSGetConstantBuffers(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, mBackupState.CSConstantBuffers);

        // 所有采样器
        mD3DDeviceContext->VSGetSamplers(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT, mBackupState.VSSamplers);
        mD3DDeviceContext->PSGetSamplers(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT, mBackupState.PSSamplers);
        mD3DDeviceContext->GSGetSamplers(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT, mBackupState.GSSamplers);
        mD3DDeviceContext->HSGetSamplers(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT, mBackupState.HSSamplers);
        mD3DDeviceContext->DSGetSamplers(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT, mBackupState.DSSamplers);
        mD3DDeviceContext->CSGetSamplers(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT, mBackupState.CSSamplers);

        // 输入布局
        mD3DDeviceContext->IAGetInputLayout(&mBackupState.InputLayout);

        // 顶点缓冲区和索引缓冲区
        mD3DDeviceContext->IAGetVertexBuffers(0, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT, mBackupState.VertexBuffers, mBackupState.VertexBufferStrides, mBackupState.VertexBufferOffsets);
        mD3DDeviceContext->IAGetIndexBuffer(&mBackupState.IndexBuffer, &mBackupState.IndexBufferFormat, &mBackupState.IndexBufferOffset);

        // 拓扑结构
        mD3DDeviceContext->IAGetPrimitiveTopology(&mBackupState.PrimitiveTopology);

        // 混合状态、深度模版状态、光栅化状态
        mD3DDeviceContext->OMGetBlendState(&mBackupState.BlendState, mBackupState.BlendFactor, &mBackupState.SampleMask);
        mD3DDeviceContext->OMGetDepthStencilState(&mBackupState.DepthStencilState, &mBackupState.StencilRef);
        mD3DDeviceContext->RSGetState(&mBackupState.RasterizerState);

        // 备份完成后，解绑所有 SRV 和 RTV，避免后续 OMSetRenderTargets 时
        // 新的 RTV 与残留的 SRV 指向同一资源导致 D3D11 HAZARD 警告。
        // reset() 还原时会把备份的状态设回去。
        ID3D11ShaderResourceView* nullSRVs[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = {};
        mD3DDeviceContext->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, nullSRVs);
        mD3DDeviceContext->VSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, nullSRVs);

        ID3D11RenderTargetView* nullRTVs[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = {};
        mD3DDeviceContext->OMSetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, nullRTVs, nullptr);
    }

    //--------------------------------------------------------------------------
    
    TResult D3D11Context::setRenderTarget(RenderWindow *renderWindow, RenderTexture *depthStencil)
    {
        D3D11RenderWindow *pD3DRenderWindow = static_cast<D3D11RenderWindow*>(renderWindow->getRHIRenderWindow());
        D3D11PixelBuffer2D *pD3DDepthStencil = nullptr;
        if (depthStencil != nullptr)
        {
            pD3DDepthStencil = static_cast<D3D11PixelBuffer2D*>(depthStencil->getPixelBuffer()->getRHIResource().get());
        }
        auto lambda = [this](const D3D11RenderWindowPtr &pD3DRenderWindow, const D3D11PixelBuffer2DPtr &pD3DDepthStencil)
        {
            backupRenderState();
            // 外挂的 RenderTexture 优先，没有时退回窗口自带的深度模板附件
            ID3D11DepthStencilView *pD3DDSView = (pD3DDepthStencil != nullptr) ? pD3DDepthStencil->D3DDSView : pD3DRenderWindow->D3DDSView;
            mD3DDeviceContext->OMSetRenderTargets(1, &pD3DRenderWindow->D3DRTView, pD3DDSView);

            return T3D_OK;
        };
        
        return ENQUEUE_UNIQUE_COMMAND(lambda, D3D11RenderWindowPtr(pD3DRenderWindow), D3D11PixelBuffer2DPtr(pD3DDepthStencil));
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::setRenderTarget(const RenderTexturePtr *renderTextures, uint32_t numOfTextures, RenderTexture *depthStencil)
    {
        RenderTextures textures(numOfTextures, nullptr);
        for (uint32_t i = 0; i < numOfTextures; ++i)
        {
            textures[i] = renderTextures[i];
        }
        D3D11PixelBuffer2D *pD3DDepthStencil = nullptr;
        if (depthStencil != nullptr)
        {
            pD3DDepthStencil = static_cast<D3D11PixelBuffer2D*>(depthStencil->getPixelBuffer()->getRHIResource().get());
        }
        auto lambda = [this](const RenderTextures &textures, const D3D11PixelBuffer2DPtr &pD3DDepthStencil)
        {
            backupRenderState();
            TArray<ID3D11RenderTargetView*> pD3DRTViews(textures.size());
   
            for (size_t i = 0; i < textures.size(); ++i)
            {
                pD3DRTViews[i] = static_cast<D3D11PixelBuffer2D*>(textures[i]->getPixelBuffer()->getRHIResource().get())->D3DRTView;
            }
            
            UINT uNumOfRTViews = static_cast<UINT>(pD3DRTViews.size());
            ID3D11RenderTargetView * const * ppD3DRTViews = nullptr;
            if (uNumOfRTViews > 0)
            {
                ppD3DRTViews = &pD3DRTViews[0];
            }
            
            if (pD3DDepthStencil != nullptr)
            {
                mD3DDeviceContext->OMSetRenderTargets(uNumOfRTViews, ppD3DRTViews, pD3DDepthStencil->D3DDSView);
            }
            else
            {
                mD3DDeviceContext->OMSetRenderTargets(uNumOfRTViews, ppD3DRTViews, nullptr);
            }
            
            return T3D_OK;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, textures, D3D11PixelBuffer2DPtr(pD3DDepthStencil));
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::setRenderTarget(RenderTarget *renderTarget)
    {
        TResult ret = T3D_OK;

        switch (renderTarget->getType())
        {
        case RenderTarget::Type::E_RT_WINDOW:
            {
                ret = setRenderTarget(renderTarget->getRenderWindow(), renderTarget->getDepthStencil());
            }
            break;
        case RenderTarget::Type::E_RT_TEXTURE:
            {
                ret = setRenderTarget(renderTarget->getRenderTextures(), renderTarget->getNumOfRenderTextures(), renderTarget->getDepthStencil());
            }
            break;
        default:
            T3D_ASSERT(false);
            break;
        }

        if (T3D_SUCCEEDED(ret))
        {
            mCurrentRenderTarget = renderTarget;
        }
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::resetRenderTarget()
    {
        mCurrentRenderTarget = nullptr;

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
        
        if (mCurrentRenderTarget != nullptr)
        {
            if (mCurrentRenderTarget->getType() == RenderTarget::Type::E_RT_WINDOW)
            {
                // 渲染窗口
                width = static_cast<Real>(mCurrentRenderTarget->getRenderWindow()->getDescriptor().Width);
                height = static_cast<Real>(mCurrentRenderTarget->getRenderWindow()->getDescriptor().Height);
            }
            else
            {
                // 渲染纹理
                if (mCurrentRenderTarget->getNumOfRenderTextures() > 0)
                {
                    // 获取颜色纹理大小
                    width = static_cast<Real>(mCurrentRenderTarget->getRenderTexture()->getWidth());
                    height = static_cast<Real>(mCurrentRenderTarget->getRenderTexture()->getHeight());
                }
                else if (mCurrentRenderTarget->getDepthStencil() != nullptr)
                {
                    // 只有深度纹理
                    width = static_cast<Real>(mCurrentRenderTarget->getDepthStencil()->getWidth());
                    height = static_cast<Real>(mCurrentRenderTarget->getDepthStencil()->getHeight());
                }
                else
                {
                    T3D_LOG_WARNING(LOG_TAG_D3D11RENDERER, "D3D11Context::setViewport: no color texture and depth stencil texture !");
                    return T3D_OK;
                }
            }
        }
        else
        {
            T3D_LOG_WARNING(LOG_TAG_D3D11RENDERER, "D3D11Context::setViewport: no render target");
            return T3D_OK;
        }
        
        auto lambda = [this](const Viewport &vp, Real width, Real height)
        {
            D3D11_VIEWPORT d3dViewport = {};
            d3dViewport.TopLeftX = vp.Left * width;
            d3dViewport.TopLeftY = vp.Top * height;
            d3dViewport.Width = vp.Width * width;
            d3dViewport.Height = vp.Height * height;
            d3dViewport.MinDepth = 0.0f;
            d3dViewport.MaxDepth = 1.0f;
            mD3DDeviceContext->RSSetViewports(1, &d3dViewport);
            return T3D_OK;
        };
        
        return ENQUEUE_UNIQUE_COMMAND(lambda, viewport, width, height);
    }
    
    //--------------------------------------------------------------------------

    TResult D3D11Context::setScissorRect(int32_t x, int32_t y, uint32_t width, uint32_t height)
    {
        auto lambda = [this](int32_t x, int32_t y, uint32_t width, uint32_t height)
        {
            D3D11_RECT rect;
            rect.left = x;
            rect.top = y;
            rect.right = x + static_cast<LONG>(width);
            rect.bottom = y + static_cast<LONG>(height);
            mD3DDeviceContext->RSSetScissorRects(1, &rect);
            return T3D_OK;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, x, y, width, height);
    }
    
    //--------------------------------------------------------------------------
    
    TResult D3D11Context::clearColor(const ColorRGB &color)
    {
        TResult ret = T3D_OK;

        if (mCurrentRenderTarget == nullptr)
            return ret;

        if (mCurrentRenderTarget->getType() == RenderTarget::Type::E_RT_WINDOW)
        {
            ret = clearColor(mCurrentRenderTarget->getRenderWindow(), color);
        }
        else
        {
            if (mCurrentRenderTarget->getNumOfRenderTextures() > 0)
            {
                ret = clearColor(mCurrentRenderTarget->getRenderTextures(), mCurrentRenderTarget->getNumOfRenderTextures(), color);
            }
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

    TResult D3D11Context::clearColor(const RenderTexturePtr *textures, uint32_t numOfTextures, const ColorRGB &color)
    {
        using D3D11PixelBuffers = TArray<D3D11PixelBuffer2DPtr>;
        D3D11PixelBuffers pixelBuffers(T3D_MAX_RENDER_TARGET);
        for (uint32_t i = 0; i < numOfTextures; ++i)
        {
            pixelBuffers[i] = smart_pointer_cast<D3D11PixelBuffer2D>(textures[i]->getPixelBuffer()->getRHIResource());
        }
        
        auto lambda = [this](const D3D11PixelBuffers &pD3DPixelBuffers, uint32_t numOfTextures, const ColorRGB &color)
        {
            float clr[4];
            clr[0] = color.red();
            clr[1] = color.green();
            clr[2] = color.blue();
            clr[3] = 1.0f;
            for (uint32_t i = 0; i < numOfTextures; ++i)
            {
                mD3DDeviceContext->ClearRenderTargetView(pD3DPixelBuffers[i]->D3DRTView, clr);
            }
            return T3D_OK;
        };
        return ENQUEUE_UNIQUE_COMMAND(lambda, pixelBuffers, numOfTextures, color);
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::clearDepth(Real depth)
    {
        return clearDepthStencilView(D3D11_CLEAR_DEPTH, depth, 0);
    }

    //--------------------------------------------------------------------------
    
    TResult D3D11Context::clearDepthStencil(Real depth, uint32_t stencil)
    {
        return clearDepthStencilView(D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depth, static_cast<uint8_t>(stencil));
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::clearDepthStencilView(uint32_t clearFlags, Real depth, uint8_t stencil)
    {
        if (mCurrentRenderTarget == nullptr)
        {
            return T3D_OK;
        }

        if (mCurrentRenderTarget->getDepthStencil() != nullptr)
        {
            D3D11PixelBuffer2D *pD3DPixelBuffer = static_cast<D3D11PixelBuffer2D*>(mCurrentRenderTarget->getDepthStencil()->getPixelBuffer()->getRHIResource().get());
            auto lambda = [this](const D3D11PixelBuffer2DPtr &pD3DPixelBuffer, uint32_t clearFlags, const Real &depth, uint8_t stencil)
            {
                mD3DDeviceContext->ClearDepthStencilView(pD3DPixelBuffer->D3DDSView, clearFlags, depth, stencil);
                return T3D_OK;
            };
            return ENQUEUE_UNIQUE_COMMAND(lambda, D3D11PixelBuffer2DPtr(pD3DPixelBuffer), clearFlags, depth, stencil);
        }

        if (mCurrentRenderTarget->getType() == RenderTarget::Type::E_RT_WINDOW)
        {
            // 没挂外部深度纹理时，清除窗口自带的深度模板附件
            D3D11RenderWindow *pD3DRenderWindow = static_cast<D3D11RenderWindow*>(mCurrentRenderTarget->getRenderWindow()->getRHIRenderWindow());
            auto lambda = [this](const D3D11RenderWindowPtr &pD3DRenderWindow, uint32_t clearFlags, const Real &depth, uint8_t stencil)
            {
                if (pD3DRenderWindow->D3DDSView != nullptr)
                {
                    mD3DDeviceContext->ClearDepthStencilView(pD3DRenderWindow->D3DDSView, clearFlags, depth, stencil);
                }
                return T3D_OK;
            };
            return ENQUEUE_UNIQUE_COMMAND(lambda, D3D11RenderWindowPtr(pD3DRenderWindow), clearFlags, depth, stencil);
        }

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::clearDepthStencil(RenderTexture *texture, const Real &depth, uint8_t stencil)
    {
        D3D11PixelBuffer2D *pD3DPixelBuffer = static_cast<D3D11PixelBuffer2D*>(texture->getPixelBuffer()->getRHIResource().get());
        auto lambda = [this](const D3D11PixelBuffer2DPtr &pD3DPixelBuffer, const Real &depth, uint8_t stencil)
        {
            mD3DDeviceContext->ClearDepthStencilView(pD3DPixelBuffer->D3DDSView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, depth, stencil);
            return T3D_OK;
        };
        return ENQUEUE_UNIQUE_COMMAND(lambda, D3D11PixelBuffer2DPtr(pD3DPixelBuffer), depth, stencil);
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
        d3dDesc.Filter = D3D11Mapping::get(desc.MinFilter,desc.MagFilter, desc.MipFilter, desc.IsComparison);
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
                    D3D11_SUBRESOURCE_DATA *pInitData = nullptr;

                    if (buffer->getBuffer().Data != nullptr)
                    {
                        initData.pSysMem = buffer->getBuffer().Data;
                        initData.SysMemPitch = 0;
                        initData.SysMemSlicePitch = 0;
                        pInitData = &initData;
                    }

                    ID3D11Buffer *pD3DBuffer = nullptr;
                    HRESULT hr = mD3DDevice->CreateBuffer(&d3dDesc, pInitData, &pD3DBuffer);
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
                    // 创建索引缓冲区子资源数据
                    D3D11_SUBRESOURCE_DATA initData;
                    memset(&initData, 0, sizeof(initData));
                    D3D11_SUBRESOURCE_DATA *pInitData = nullptr;

                    if (buffer->getBuffer().Data != nullptr)
                    {
                        initData.pSysMem = buffer->getBuffer().Data;
                        initData.SysMemPitch = 0;
                        initData.SysMemSlicePitch = 0;
                        pInitData = &initData;
                    }

                    ID3D11Buffer *pD3DBuffer = nullptr;
                    HRESULT hr = mD3DDevice->CreateBuffer(&d3dDesc, pInitData, &pD3DBuffer);
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
    
    TResult D3D11Context::buildSubresourceData(const uint8_t *data, size_t dataSize, PixelFormat format, uint32_t width, uint32_t height, uint32_t depth, uint32_t mipLevels, uint32_t arraySize, TArray<D3D11_SUBRESOURCE_DATA> &outSubresources)
    {
        outSubresources.clear();

        if (data == nullptr || dataSize == 0)
        {
            // 不上传初始数据，调用方给 CreateTextureXD 传 nullptr
            return T3D_OK;
        }

        const uint32_t bpp = Image::getBPP(format) / 8;
        if (bpp == 0)
        {
            T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "buildSubresourceData : unsupported pixel format [%d] !", format);
            return T3D_ERR_INVALID_PARAM;
        }

        mipLevels = std::max<uint32_t>(1, mipLevels);
        arraySize = std::max<uint32_t>(1, arraySize);

        // 先累加一遍需要的字节数，不足就直接拒绝，避免 D3D11 读到野内存
        size_t required = 0;
        for (uint32_t level = 0; level < mipLevels; ++level)
        {
            const size_t mipW = std::max<uint32_t>(1, width >> level);
            const size_t mipH = std::max<uint32_t>(1, height >> level);
            const size_t mipD = std::max<uint32_t>(1, depth >> level);
            required += mipW * mipH * mipD * bpp;
        }
        required *= arraySize;

        if (required > dataSize)
        {
            T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "buildSubresourceData : data is not enough ! expected [%zu] actual [%zu]", required, dataSize);
            return T3D_ERR_INVALID_PARAM;
        }

        outSubresources.resize(static_cast<size_t>(arraySize) * mipLevels);

        const uint8_t *cursor = data;
        for (uint32_t slice = 0; slice < arraySize; ++slice)
        {
            for (uint32_t level = 0; level < mipLevels; ++level)
            {
                const uint32_t mipW = std::max<uint32_t>(1, width >> level);
                const uint32_t mipH = std::max<uint32_t>(1, height >> level);
                const uint32_t mipD = std::max<uint32_t>(1, depth >> level);
                const uint32_t rowPitch = mipW * bpp;
                const uint32_t slicePitch = rowPitch * mipH;

                // D3D11 的子资源索引恒为 arraySlice * mipLevels + mipLevel
                D3D11_SUBRESOURCE_DATA &sub = outSubresources[static_cast<size_t>(slice) * mipLevels + level];
                sub.pSysMem = cursor;
                sub.SysMemPitch = rowPitch;
                sub.SysMemSlicePitch = slicePitch;

                cursor += static_cast<size_t>(slicePitch) * mipD;
            }
        }

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    /**
     * \brief 深拷贝一份 CPU 像素数据，并把子资源数组里的 pSysMem 重定向到副本
     * \param [in] src : 调用方的原始数据，subresources 里的指针都落在这块内存内
     * \param [in,out] subresources : 待重定向的子资源数组，为空时不做任何事
     * \param [out] owned : 副本，由 RHI 线程上的 lambda 在末尾 release
     * \remarks 多线程模式下命令延后执行，原始数据届时可能已被上层释放，故必须复制
     */
    static void cloneSubresourceData(const Buffer &src, TArray<D3D11_SUBRESOURCE_DATA> &subresources, Buffer &owned)
    {
        if (subresources.empty() || src.Data == nullptr || src.DataSize == 0)
        {
            return;
        }

        owned.DataSize = src.DataSize;
        owned.Data = T3D_POD_NEW_ARRAY(uint8_t, src.DataSize);
        memcpy(owned.Data, src.Data, src.DataSize);

        for (auto &sub : subresources)
        {
            const ptrdiff_t offset = static_cast<const uint8_t *>(sub.pSysMem) - src.Data;
            sub.pSysMem = owned.Data + offset;
        }
    }

    //--------------------------------------------------------------------------

    RHIPixelBuffer1DPtr D3D11Context::createPixelBuffer1D(PixelBuffer1D *buffer)
    {
        const auto &desc = buffer->getDescriptor();

        if (desc.width == 0)
        {
            T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "createPixelBuffer1D : invalid size [%u] !", desc.width);
            return nullptr;
        }

        D3D11_USAGE d3dUsage;
        uint32_t d3dAccess = 0;
        if (T3D_FAILED(D3D11Mapping::get(buffer->getUsage(), buffer->getCPUAccessMode(), d3dUsage, d3dAccess)))
        {
            T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "createPixelBuffer1D : invalid usage [%d] / access mode [%u] !", buffer->getUsage(), buffer->getCPUAccessMode());
            return nullptr;
        }

        // MipLevels = 0 在 D3D11 里是「自动生成完整 mip 链」的特殊语义，与引擎语义不一致
        const uint32_t mipLevels = std::max<uint32_t>(1, desc.mipmaps);
        const uint32_t arraySize = std::max<uint32_t>(1, desc.arraySize);

        TArray<D3D11_SUBRESOURCE_DATA> subresources;
        if (T3D_FAILED(buildSubresourceData(desc.buffer.Data, desc.buffer.DataSize, desc.format, desc.width, 1, 1, mipLevels, arraySize, subresources)))
        {
            return nullptr;
        }

        Buffer ownedBuffer;
        cloneSubresourceData(desc.buffer, subresources, ownedBuffer);

        D3D11_TEXTURE1D_DESC d3dDesc = D3D11Mapping::get(desc);
        d3dDesc.MipLevels = mipLevels;
        d3dDesc.ArraySize = arraySize;
        d3dDesc.Usage = d3dUsage;
        d3dDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        d3dDesc.CPUAccessFlags = d3dAccess;
        d3dDesc.MiscFlags = 0;

        D3D11_SHADER_RESOURCE_VIEW_DESC d3dSRVDesc;
        memset(&d3dSRVDesc, 0, sizeof(d3dSRVDesc));
        d3dSRVDesc.Format = d3dDesc.Format;
        if (arraySize > 1)
        {
            d3dSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1DARRAY;
            d3dSRVDesc.Texture1DArray.MostDetailedMip = 0;
            d3dSRVDesc.Texture1DArray.MipLevels = mipLevels;
            d3dSRVDesc.Texture1DArray.FirstArraySlice = 0;
            d3dSRVDesc.Texture1DArray.ArraySize = arraySize;
        }
        else
        {
            d3dSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
            d3dSRVDesc.Texture1D.MostDetailedMip = 0;
            d3dSRVDesc.Texture1D.MipLevels = mipLevels;
        }

        D3D11PixelBuffer1DPtr d3dBuffer = D3D11PixelBuffer1D::create();

        auto lambda = [this](const D3D11_TEXTURE1D_DESC &d3dDesc, const D3D11_SHADER_RESOURCE_VIEW_DESC &d3dSRVDesc, const D3D11PixelBuffer1DPtr &d3dBuffer, TArray<D3D11_SUBRESOURCE_DATA> &subresources, Buffer &ownedBuffer)
        {
            TResult ret = T3D_OK;

            do
            {
                ID3D11Texture1D *pD3DTex1D = nullptr;
                HRESULT hr = mD3DDevice->CreateTexture1D(&d3dDesc, subresources.empty() ? nullptr : subresources.data(), &pD3DTex1D);
                if (FAILED(hr))
                {
                    T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Failed to create texture1d ! DX ERROR [%d]", hr);
                    ret = T3D_ERR_D3D11_CREATE_TEXTURE1D;
                    break;
                }

                ID3D11ShaderResourceView *pD3DSRView = nullptr;
                hr = mD3DDevice->CreateShaderResourceView(pD3DTex1D, &d3dSRVDesc, &pD3DSRView);
                if (FAILED(hr))
                {
                    D3D_SAFE_RELEASE(pD3DTex1D);
                    T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Failed to create shader resource view for 1D texture ! DX ERROR [%d]", hr);
                    ret = T3D_ERR_D3D11_CREATE_SHADER_RESOURCE_VIEW;
                    break;
                }

                d3dBuffer->D3DTexture = pD3DTex1D;
                d3dBuffer->D3DSRView = pD3DSRView;
            } while (false);

            ownedBuffer.release();

            return ret;
        };

        ENQUEUE_UNIQUE_COMMAND(lambda, d3dDesc, d3dSRVDesc, d3dBuffer, subresources, ownedBuffer);

        return d3dBuffer;
    }

    //--------------------------------------------------------------------------
    
    RHIPixelBuffer2DPtr D3D11Context::createPixelBuffer2D(PixelBuffer2D *buffer)
    {
        const auto &desc = buffer->getDescriptor();

        if (desc.width == 0 || desc.height == 0)
        {
            T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "createPixelBuffer2D : invalid size [%u x %u] !", desc.width, desc.height);
            return nullptr;
        }

        D3D11_USAGE d3dUsage;
        uint32_t d3dAccess = 0;
        if (T3D_FAILED(D3D11Mapping::get(buffer->getUsage(), buffer->getCPUAccessMode(), d3dUsage, d3dAccess)))
        {
            T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "createPixelBuffer2D : invalid usage [%d] / access mode [%u] !", buffer->getUsage(), buffer->getCPUAccessMode());
            return nullptr;
        }

        const uint32_t mipLevels = std::max<uint32_t>(1, desc.mipmaps);
        const uint32_t arraySize = std::max<uint32_t>(1, desc.arraySize);

        TArray<D3D11_SUBRESOURCE_DATA> subresources;
        if (T3D_FAILED(buildSubresourceData(desc.buffer.Data, desc.buffer.DataSize, desc.format, desc.width, desc.height, 1, mipLevels, arraySize, subresources)))
        {
            return nullptr;
        }

        Buffer ownedBuffer;
        cloneSubresourceData(desc.buffer, subresources, ownedBuffer);

        D3D11_TEXTURE2D_DESC d3dDesc = D3D11Mapping::get(desc);
        d3dDesc.MipLevels = mipLevels;
        d3dDesc.ArraySize = arraySize;
        d3dDesc.SampleDesc.Count = std::max<uint32_t>(1, desc.sampleDesc.Count);
        d3dDesc.Usage = d3dUsage;
        d3dDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        d3dDesc.CPUAccessFlags = d3dAccess;
        d3dDesc.MiscFlags = 0;

        D3D11_SHADER_RESOURCE_VIEW_DESC d3dSRVDesc;
        memset(&d3dSRVDesc, 0, sizeof(d3dSRVDesc));
        d3dSRVDesc.Format = d3dDesc.Format;
        if (arraySize > 1)
        {
            d3dSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
            d3dSRVDesc.Texture2DArray.MostDetailedMip = 0;
            d3dSRVDesc.Texture2DArray.MipLevels = mipLevels;
            d3dSRVDesc.Texture2DArray.FirstArraySlice = 0;
            d3dSRVDesc.Texture2DArray.ArraySize = arraySize;
        }
        else
        {
            d3dSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            d3dSRVDesc.Texture2D.MostDetailedMip = 0;
            d3dSRVDesc.Texture2D.MipLevels = mipLevels;
        }

        D3D11PixelBuffer2DPtr d3dBuffer = D3D11PixelBuffer2D::create();

        auto lambda = [this](const D3D11_TEXTURE2D_DESC &d3dDesc, const D3D11_SHADER_RESOURCE_VIEW_DESC &d3dSRVDesc, const D3D11PixelBuffer2DPtr &d3dBuffer, TArray<D3D11_SUBRESOURCE_DATA> &subresources, Buffer &ownedBuffer)
        {
            TResult ret = T3D_OK;

            do
            {
                ID3D11Texture2D *pD3DTex2D = nullptr;
                HRESULT hr = mD3DDevice->CreateTexture2D(&d3dDesc, subresources.empty() ? nullptr : subresources.data(), &pD3DTex2D);
                if (FAILED(hr))
                {
                    T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Failed to create texture2d ! DX ERROR [%d]", hr);
                    ret = T3D_ERR_D3D11_CREATE_TEXTURE2D;
                    break;
                }
                
                ID3D11ShaderResourceView *pD3DSRView = nullptr;
                hr = mD3DDevice->CreateShaderResourceView(pD3DTex2D, &d3dSRVDesc, &pD3DSRView);
                if (FAILED(hr))
                {
                    D3D_SAFE_RELEASE(pD3DTex2D);
                    T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Failed to create shader resource view for 2D texture ! DX ERROR [%d]", hr);
                    ret = T3D_ERR_D3D11_CREATE_SHADER_RESOURCE_VIEW;
                    break;
                }

                d3dBuffer->D3DTexture = pD3DTex2D;
                d3dBuffer->D3DSRView = pD3DSRView;
            } while (false);

            ownedBuffer.release();

            return ret;
        };

        ENQUEUE_UNIQUE_COMMAND(lambda, d3dDesc, d3dSRVDesc, d3dBuffer, subresources, ownedBuffer);

        return d3dBuffer;
    }

    //--------------------------------------------------------------------------
    
    RHIPixelBuffer3DPtr D3D11Context::createPixelBuffer3D(PixelBuffer3D *buffer)
    {
        const auto &desc = buffer->getDescriptor();

        if (desc.width == 0 || desc.height == 0 || desc.depth == 0)
        {
            T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "createPixelBuffer3D : invalid size [%u x %u x %u] !", desc.width, desc.height, desc.depth);
            return nullptr;
        }

        D3D11_USAGE d3dUsage;
        uint32_t d3dAccess = 0;
        if (T3D_FAILED(D3D11Mapping::get(buffer->getUsage(), buffer->getCPUAccessMode(), d3dUsage, d3dAccess)))
        {
            T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "createPixelBuffer3D : invalid usage [%d] / access mode [%u] !", buffer->getUsage(), buffer->getCPUAccessMode());
            return nullptr;
        }

        const uint32_t mipLevels = std::max<uint32_t>(1, desc.mipmaps);

        // 3D 纹理不支持数组，arraySize 恒为 1
        TArray<D3D11_SUBRESOURCE_DATA> subresources;
        if (T3D_FAILED(buildSubresourceData(desc.buffer.Data, desc.buffer.DataSize, desc.format, desc.width, desc.height, desc.depth, mipLevels, 1, subresources)))
        {
            return nullptr;
        }

        Buffer ownedBuffer;
        cloneSubresourceData(desc.buffer, subresources, ownedBuffer);

        D3D11_TEXTURE3D_DESC d3dDesc = D3D11Mapping::get(desc);
        d3dDesc.MipLevels = mipLevels;
        d3dDesc.Usage = d3dUsage;
        d3dDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        d3dDesc.CPUAccessFlags = d3dAccess;
        d3dDesc.MiscFlags = 0;

        D3D11_SHADER_RESOURCE_VIEW_DESC d3dSRVDesc;
        memset(&d3dSRVDesc, 0, sizeof(d3dSRVDesc));
        d3dSRVDesc.Format = d3dDesc.Format;
        d3dSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
        d3dSRVDesc.Texture3D.MostDetailedMip = 0;
        d3dSRVDesc.Texture3D.MipLevels = mipLevels;

        D3D11PixelBuffer3DPtr d3dBuffer = D3D11PixelBuffer3D::create();

        auto lambda = [this](const D3D11_TEXTURE3D_DESC &d3dDesc, const D3D11_SHADER_RESOURCE_VIEW_DESC &d3dSRVDesc, const D3D11PixelBuffer3DPtr &d3dBuffer, TArray<D3D11_SUBRESOURCE_DATA> &subresources, Buffer &ownedBuffer)
        {
            TResult ret = T3D_OK;

            do
            {
                ID3D11Texture3D *pD3DTex3D = nullptr;
                HRESULT hr = mD3DDevice->CreateTexture3D(&d3dDesc, subresources.empty() ? nullptr : subresources.data(), &pD3DTex3D);
                if (FAILED(hr))
                {
                    T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Failed to create texture3d ! DX ERROR [%d]", hr);
                    ret = T3D_ERR_D3D11_CREATE_TEXTURE3D;
                    break;
                }

                ID3D11ShaderResourceView *pD3DSRView = nullptr;
                hr = mD3DDevice->CreateShaderResourceView(pD3DTex3D, &d3dSRVDesc, &pD3DSRView);
                if (FAILED(hr))
                {
                    D3D_SAFE_RELEASE(pD3DTex3D);
                    T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Failed to create shader resource view for 3D texture ! DX ERROR [%d]", hr);
                    ret = T3D_ERR_D3D11_CREATE_SHADER_RESOURCE_VIEW;
                    break;
                }

                d3dBuffer->D3DTexture = pD3DTex3D;
                d3dBuffer->D3DSRView = pD3DSRView;
            } while (false);

            ownedBuffer.release();

            return ret;
        };

        ENQUEUE_UNIQUE_COMMAND(lambda, d3dDesc, d3dSRVDesc, d3dBuffer, subresources, ownedBuffer);

        return d3dBuffer;
    }

    //--------------------------------------------------------------------------

    RHIPixelBufferCubemapPtr D3D11Context::createPixelBufferCubemap(PixelBufferCubemap *buffer)
    {
        const auto &desc = buffer->getDescriptor();

        if (desc.width == 0 || desc.height == 0)
        {
            T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "createPixelBufferCubemap : invalid size [%u x %u] !", desc.width, desc.height);
            return nullptr;
        }

        D3D11_USAGE d3dUsage;
        uint32_t d3dAccess = 0;
        if (T3D_FAILED(D3D11Mapping::get(buffer->getUsage(), buffer->getCPUAccessMode(), d3dUsage, d3dAccess)))
        {
            T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "createPixelBufferCubemap : invalid usage [%d] / access mode [%u] !", buffer->getUsage(), buffer->getCPUAccessMode());
            return nullptr;
        }

        const uint32_t mipLevels = std::max<uint32_t>(1, desc.mipmaps);
        // desc.arraySize 存的是总面数，除以 6 得到立方体个数
        const uint32_t cubeCount = std::max<uint32_t>(1, desc.arraySize / PixelBufferCubemap::FACE_COUNT);
        const uint32_t faceCount = cubeCount * PixelBufferCubemap::FACE_COUNT;

        // helper 产出的顺序（外层 array slice、内层 mip）正好匹配 D3D11 对 cubemap
        // 的 face * mipLevels + mip 布局，也匹配 +X,-X,+Y,-Y,+Z,-Z 的面序
        TArray<D3D11_SUBRESOURCE_DATA> subresources;
        if (T3D_FAILED(buildSubresourceData(desc.buffer.Data, desc.buffer.DataSize, desc.format, desc.width, desc.height, 1, mipLevels, faceCount, subresources)))
        {
            return nullptr;
        }

        Buffer ownedBuffer;
        cloneSubresourceData(desc.buffer, subresources, ownedBuffer);

        D3D11_TEXTURE2D_DESC d3dDesc;
        memset(&d3dDesc, 0, sizeof(d3dDesc));
        d3dDesc.Width = desc.width;
        d3dDesc.Height = desc.height;
        d3dDesc.MipLevels = mipLevels;
        d3dDesc.ArraySize = faceCount;
        d3dDesc.Format = D3D11Mapping::get(desc.format);
        // D3D11_RESOURCE_MISC_TEXTURECUBE 不能与 MSAA 组合，采样数只能是 1
        d3dDesc.SampleDesc.Count = 1;
        d3dDesc.SampleDesc.Quality = 0;
        d3dDesc.Usage = d3dUsage;
        d3dDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        d3dDesc.CPUAccessFlags = d3dAccess;
        d3dDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

        D3D11_SHADER_RESOURCE_VIEW_DESC d3dSRVDesc;
        memset(&d3dSRVDesc, 0, sizeof(d3dSRVDesc));
        d3dSRVDesc.Format = d3dDesc.Format;
        if (cubeCount > 1)
        {
            d3dSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
            d3dSRVDesc.TextureCubeArray.MostDetailedMip = 0;
            d3dSRVDesc.TextureCubeArray.MipLevels = mipLevels;
            d3dSRVDesc.TextureCubeArray.First2DArrayFace = 0;
            d3dSRVDesc.TextureCubeArray.NumCubes = cubeCount;
        }
        else
        {
            d3dSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
            d3dSRVDesc.TextureCube.MostDetailedMip = 0;
            d3dSRVDesc.TextureCube.MipLevels = mipLevels;
        }

        D3D11PixelBufferCubemapPtr d3dBuffer = D3D11PixelBufferCubemap::create();

        auto lambda = [this](const D3D11_TEXTURE2D_DESC &d3dDesc, const D3D11_SHADER_RESOURCE_VIEW_DESC &d3dSRVDesc, const D3D11PixelBufferCubemapPtr &d3dBuffer, TArray<D3D11_SUBRESOURCE_DATA> &subresources, Buffer &ownedBuffer)
        {
            TResult ret = T3D_OK;

            do
            {
                ID3D11Texture2D *pD3DTex2D = nullptr;
                HRESULT hr = mD3DDevice->CreateTexture2D(&d3dDesc, subresources.empty() ? nullptr : subresources.data(), &pD3DTex2D);
                if (FAILED(hr))
                {
                    T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Failed to create cubemap texture ! DX ERROR [%d]", hr);
                    ret = T3D_ERR_D3D11_CREATE_TEXTURE2D;
                    break;
                }

                ID3D11ShaderResourceView *pD3DSRView = nullptr;
                hr = mD3DDevice->CreateShaderResourceView(pD3DTex2D, &d3dSRVDesc, &pD3DSRView);
                if (FAILED(hr))
                {
                    D3D_SAFE_RELEASE(pD3DTex2D);
                    T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "Failed to create shader resource view for cubemap ! DX ERROR [%d]", hr);
                    ret = T3D_ERR_D3D11_CREATE_SHADER_RESOURCE_VIEW;
                    break;
                }

                d3dBuffer->D3DTexture = pD3DTex2D;
                d3dBuffer->D3DSRView = pD3DSRView;
            } while (false);

            ownedBuffer.release();

            return ret;
        };

        ENQUEUE_UNIQUE_COMMAND(lambda, d3dDesc, d3dSRVDesc, d3dBuffer, subresources, ownedBuffer);

        return d3dBuffer;
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
        if (shader == nullptr)
        {
            auto lambda = [this]()
            {
                mD3DDeviceContext->VSSetShader(nullptr, nullptr, 0);
                return T3D_OK;
            };
            return ENQUEUE_UNIQUE_COMMAND(lambda);
        }

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
        if (shader == nullptr)
        {
            auto lambda = [this]()
            {
                mD3DDeviceContext->PSSetShader(nullptr, nullptr, 0);
                return T3D_OK;
            };
            return ENQUEUE_UNIQUE_COMMAND(lambda);
        }

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
        if (shader == nullptr)
        {
            auto lambda = [this]()
            {
                mD3DDeviceContext->HSSetShader(nullptr, nullptr, 0);
                return T3D_OK;
            };
            return ENQUEUE_UNIQUE_COMMAND(lambda);
        }

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
        if (shader == nullptr)
        {
            auto lambda = [this]()
            {
                mD3DDeviceContext->DSSetShader(nullptr, nullptr, 0);
                return T3D_OK;
            };
            return ENQUEUE_UNIQUE_COMMAND(lambda);
        }

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
        if (shader == nullptr)
        {
            auto lambda = [this]()
            {
                mD3DDeviceContext->GSSetShader(nullptr, nullptr, 0);
                return T3D_OK;
            };
            return ENQUEUE_UNIQUE_COMMAND(lambda);
        }

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
        if (shader == nullptr)
        {
            auto lambda = [this]()
            {
                mD3DDeviceContext->CSSetShader(nullptr, nullptr, 0);
                return T3D_OK;
            };
            return ENQUEUE_UNIQUE_COMMAND(lambda);
        }

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
        mCurrentRenderTarget = nullptr;

        auto lambda = [this]()
        {
            // 还原裁剪状态
            mD3DDeviceContext->RSSetScissorRects(mBackupState.ScissorRectsCount, mBackupState.ScissorRects);

            // 还原视口状态
            mD3DDeviceContext->RSSetViewports(mBackupState.ViewportsCount, mBackupState.Viewports);

            // 还原所有渲染目标和深度模板
            mD3DDeviceContext->OMSetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, mBackupState.RenderTargetViews, mBackupState.DepthStencilView);
            D3D_SAFE_RELEASE_ARRAY(mBackupState.RenderTargetViews);
            D3D_SAFE_RELEASE(mBackupState.DepthStencilView);

            // 还原所有着色器资源（PS、VS、GS、HS、DS、CS）
            mD3DDeviceContext->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, mBackupState.PSShaderResources);
            D3D_SAFE_RELEASE_ARRAY(mBackupState.PSShaderResources);
            mD3DDeviceContext->VSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, mBackupState.VSShaderResources);
            D3D_SAFE_RELEASE_ARRAY(mBackupState.VSShaderResources);
            mD3DDeviceContext->GSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, mBackupState.GSShaderResources);
            D3D_SAFE_RELEASE_ARRAY(mBackupState.GSShaderResources);
            mD3DDeviceContext->HSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, mBackupState.HSShaderResources);
            D3D_SAFE_RELEASE_ARRAY(mBackupState.HSShaderResources);
            mD3DDeviceContext->DSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, mBackupState.DSShaderResources);
            D3D_SAFE_RELEASE_ARRAY(mBackupState.DSShaderResources);
            mD3DDeviceContext->CSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, mBackupState.CSShaderResources);
            D3D_SAFE_RELEASE_ARRAY(mBackupState.CSShaderResources);

            // 还原所有着色器
            mD3DDeviceContext->VSSetShader(mBackupState.VS, mBackupState.VSInstances, mBackupState.VSInstancesCount);
            D3D_SAFE_RELEASE(mBackupState.VS);
            D3D_SAFE_RELEASE_ARRAY(mBackupState.VSInstances);
            mD3DDeviceContext->PSSetShader(mBackupState.PS, mBackupState.PSInstances, mBackupState.PSInstancesCount);
            D3D_SAFE_RELEASE(mBackupState.PS);
            D3D_SAFE_RELEASE_ARRAY(mBackupState.PSInstances);
            mD3DDeviceContext->GSSetShader(mBackupState.GS, mBackupState.GSInstances, mBackupState.GSInstancesCount);
            D3D_SAFE_RELEASE(mBackupState.GS);
            D3D_SAFE_RELEASE_ARRAY(mBackupState.GSInstances);
            mD3DDeviceContext->HSSetShader(mBackupState.HS, mBackupState.HSInstances, mBackupState.HSInstancesCount);
            D3D_SAFE_RELEASE(mBackupState.HS);
            D3D_SAFE_RELEASE_ARRAY(mBackupState.HSInstances);
            mD3DDeviceContext->DSSetShader(mBackupState.DS, mBackupState.DSInstances, mBackupState.DSInstancesCount);
            D3D_SAFE_RELEASE(mBackupState.DS);
            D3D_SAFE_RELEASE_ARRAY(mBackupState.DSInstances);
            mD3DDeviceContext->CSSetShader(mBackupState.CS, mBackupState.CSInstances, mBackupState.CSInstancesCount);
            D3D_SAFE_RELEASE(mBackupState.CS);
            D3D_SAFE_RELEASE_ARRAY(mBackupState.CSInstances);

            // 还原所有常量缓冲区
            mD3DDeviceContext->VSSetConstantBuffers(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, mBackupState.VSConstantBuffers);
            D3D_SAFE_RELEASE_ARRAY(mBackupState.VSConstantBuffers);
            mD3DDeviceContext->PSSetConstantBuffers(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, mBackupState.PSConstantBuffers);
            D3D_SAFE_RELEASE_ARRAY(mBackupState.PSConstantBuffers);
            mD3DDeviceContext->GSSetConstantBuffers(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, mBackupState.GSConstantBuffers);
            D3D_SAFE_RELEASE_ARRAY(mBackupState.GSConstantBuffers);
            mD3DDeviceContext->HSSetConstantBuffers(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, mBackupState.HSConstantBuffers);
            D3D_SAFE_RELEASE_ARRAY(mBackupState.HSConstantBuffers);
            mD3DDeviceContext->DSSetConstantBuffers(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, mBackupState.DSConstantBuffers);
            D3D_SAFE_RELEASE_ARRAY(mBackupState.DSConstantBuffers);
            mD3DDeviceContext->CSSetConstantBuffers(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, mBackupState.CSConstantBuffers);
            D3D_SAFE_RELEASE_ARRAY(mBackupState.CSConstantBuffers);

            // 还原所有采样器
            mD3DDeviceContext->PSSetSamplers(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT, mBackupState.PSSamplers);
            D3D_SAFE_RELEASE_ARRAY(mBackupState.PSSamplers);
            mD3DDeviceContext->VSSetSamplers(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT, mBackupState.VSSamplers);
            D3D_SAFE_RELEASE_ARRAY(mBackupState.VSSamplers);
            mD3DDeviceContext->GSSetSamplers(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT, mBackupState.GSSamplers);
            D3D_SAFE_RELEASE_ARRAY(mBackupState.GSSamplers);
            mD3DDeviceContext->HSSetSamplers(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT, mBackupState.HSSamplers);
            D3D_SAFE_RELEASE_ARRAY(mBackupState.HSSamplers);
            mD3DDeviceContext->DSSetSamplers(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT, mBackupState.DSSamplers);
            D3D_SAFE_RELEASE_ARRAY(mBackupState.DSSamplers);
            mD3DDeviceContext->CSSetSamplers(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT, mBackupState.CSSamplers);
            D3D_SAFE_RELEASE_ARRAY(mBackupState.CSSamplers);

            // 还原输入布局
            mD3DDeviceContext->IASetInputLayout(mBackupState.InputLayout);
            D3D_SAFE_RELEASE(mBackupState.InputLayout);

            // 还原顶点缓冲区和索引缓冲区
            mD3DDeviceContext->IASetVertexBuffers(0, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT, mBackupState.VertexBuffers, mBackupState.VertexBufferStrides, mBackupState.VertexBufferOffsets);
            D3D_SAFE_RELEASE_ARRAY(mBackupState.VertexBuffers);
            mD3DDeviceContext->IASetIndexBuffer(mBackupState.IndexBuffer, mBackupState.IndexBufferFormat, mBackupState.IndexBufferOffset);
            D3D_SAFE_RELEASE(mBackupState.IndexBuffer);

            // 还原拓扑为默认（通常是三角形列表）
            mD3DDeviceContext->IASetPrimitiveTopology(mBackupState.PrimitiveTopology);

            // 还原光栅化状态、混合状态、深度模板状态为默认（如果你有默认状态对象）
            mD3DDeviceContext->RSSetState(mBackupState.RasterizerState);
            D3D_SAFE_RELEASE(mBackupState.RasterizerState);
            mD3DDeviceContext->OMSetBlendState(mBackupState.BlendState, mBackupState.BlendFactor, mBackupState.SampleMask);
            D3D_SAFE_RELEASE(mBackupState.BlendState);
            mD3DDeviceContext->OMSetDepthStencilState(mBackupState.DepthStencilState, mBackupState.StencilRef);
            D3D_SAFE_RELEASE(mBackupState.DepthStencilState);
            
            return T3D_OK;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda);
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::blit(RenderTarget *src, RenderTarget *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)
    {
        if (src == nullptr || dst == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "blit : source or destination render target is null !");
            return T3D_ERR_INVALID_POINTER;
        }

        auto lambda = [this](const RenderTargetPtr &pSrc, const RenderTargetPtr &pDst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 &dstOffset)
        {
            BlitEndpoint srcEndpoint;
            TResult ret = resolveBlitEndpoint(pSrc.get(), true, srcEndpoint);
            if (T3D_FAILED(ret))
            {
                return ret;
            }

            BlitEndpoint dstEndpoint;
            ret = resolveBlitEndpoint(pDst.get(), false, dstEndpoint);
            if (T3D_FAILED(ret))
            {
                return ret;
            }

            return doBlit(srcEndpoint, dstEndpoint, srcOffset, size, dstOffset);
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, RenderTargetPtr(src), RenderTargetPtr(dst), srcOffset, size, dstOffset);
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::blit(Texture *src, RenderTarget *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)
    {
        if (src == nullptr || dst == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "blit : source texture or destination render target is null !");
            return T3D_ERR_INVALID_POINTER;
        }

        auto lambda = [this](const TexturePtr &pSrc, const RenderTargetPtr &pDst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 &dstOffset)
        {
            BlitEndpoint srcEndpoint;
            TResult ret = resolveBlitEndpoint(pSrc.get(), true, srcEndpoint);
            if (T3D_FAILED(ret))
            {
                return ret;
            }

            BlitEndpoint dstEndpoint;
            ret = resolveBlitEndpoint(pDst.get(), false, dstEndpoint);
            if (T3D_FAILED(ret))
            {
                return ret;
            }

            return doBlit(srcEndpoint, dstEndpoint, srcOffset, size, dstOffset);
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, TexturePtr(src), RenderTargetPtr(dst), srcOffset, size, dstOffset);
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::blit(RenderTarget *src, Texture *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)
    {
        if (src == nullptr || dst == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "blit : source render target or destination texture is null !");
            return T3D_ERR_INVALID_POINTER;
        }

        auto lambda = [this](const RenderTargetPtr &pSrc, const TexturePtr &pDst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 &dstOffset)
        {
            BlitEndpoint srcEndpoint;
            TResult ret = resolveBlitEndpoint(pSrc.get(), true, srcEndpoint);
            if (T3D_FAILED(ret))
            {
                return ret;
            }

            BlitEndpoint dstEndpoint;
            ret = resolveBlitEndpoint(pDst.get(), false, dstEndpoint);
            if (T3D_FAILED(ret))
            {
                return ret;
            }

            return doBlit(srcEndpoint, dstEndpoint, srcOffset, size, dstOffset);
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, RenderTargetPtr(src), TexturePtr(dst), srcOffset, size, dstOffset);
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::blit(Texture *src, Texture *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)
    {
        if (src == nullptr || dst == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "blit : source or destination texture is null !");
            return T3D_ERR_INVALID_POINTER;
        }

        auto lambda = [this](const TexturePtr &pSrc, const TexturePtr &pDst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 &dstOffset)
        {
            BlitEndpoint srcEndpoint;
            TResult ret = resolveBlitEndpoint(pSrc.get(), true, srcEndpoint);
            if (T3D_FAILED(ret))
            {
                return ret;
            }

            BlitEndpoint dstEndpoint;
            ret = resolveBlitEndpoint(pDst.get(), false, dstEndpoint);
            if (T3D_FAILED(ret))
            {
                return ret;
            }

            return doBlit(srcEndpoint, dstEndpoint, srcOffset, size, dstOffset);
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, TexturePtr(src), TexturePtr(dst), srcOffset, size, dstOffset);
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::copyBuffer(RenderBuffer *src, RenderBuffer *dst, size_t srcOffset, size_t size, size_t dstOffset)
    {
        if (src == nullptr || dst == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "copyBuffer : source or destination buffer is null !");
            return T3D_ERR_INVALID_POINTER;
        }

        if (src->getRHIResource() == nullptr || dst->getRHIResource() == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "copyBuffer : source or destination buffer has no RHI resource !");
            return T3D_ERR_INVALID_POINTER;
        }

        auto isLinearBuffer = [](const RHIResourcePtr &resource)
        {
            const RHIResource::ResourceType type = resource->getResourceType();
            return type == RHIResource::ResourceType::kVertexBuffer
                || type == RHIResource::ResourceType::kIndexBuffer
                || type == RHIResource::ResourceType::kConstantBuffer;
        };

        if (!isLinearBuffer(src->getRHIResource()) || !isLinearBuffer(dst->getRHIResource()))
        {
            // 纹理类资源请走 blit，那边才有格式/采样数的兼容性判断
            T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "copyBuffer : only vertex / index / constant buffer are supported !");
            return T3D_ERR_D3D11_UNSUPPORTED_OPERATION;
        }

        if (dst->getUsage() == Usage::kImmutable)
        {
            T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "copyBuffer : destination buffer is immutable !");
            return T3D_ERR_D3D11_INVALID_USAGE;
        }

        auto lambda = [this](const RenderBufferPtr &pSrc, const RenderBufferPtr &pDst, size_t srcOffset, size_t size, size_t dstOffset) -> TResult
        {
            ID3D11Resource *pD3DSrcRes = getD3DResource(pSrc.get());
            ID3D11Resource *pD3DDstRes = getD3DResource(pDst.get());

            if (pD3DSrcRes == nullptr || pD3DDstRes == nullptr)
            {
                T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "copyBuffer : failed to retrieve underlying D3D11 buffer !");
                return T3D_ERR_INVALID_POINTER;
            }

            if (pD3DSrcRes == pD3DDstRes)
            {
                T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "copyBuffer : source and destination are the same buffer !");
                return T3D_ERR_D3D11_INCOMPATIBLE_COPY;
            }

            // 以 GPU 侧的真实 ByteWidth 做边界校验，比引擎侧 CPU 镜像的大小可靠
            ID3D11Buffer *pD3DSrc = static_cast<ID3D11Buffer *>(pD3DSrcRes);
            ID3D11Buffer *pD3DDst = static_cast<ID3D11Buffer *>(pD3DDstRes);

            D3D11_BUFFER_DESC srcDesc;
            pD3DSrc->GetDesc(&srcDesc);
            D3D11_BUFFER_DESC dstDesc;
            pD3DDst->GetDesc(&dstDesc);

            // size 为 0 表示复制源缓冲从 srcOffset 起的剩余部分
            const size_t copySize = (size == 0) ? (srcDesc.ByteWidth - std::min<size_t>(srcOffset, srcDesc.ByteWidth)) : size;

            if (copySize == 0)
            {
                return T3D_OK;
            }

            if (srcOffset + copySize > srcDesc.ByteWidth || dstOffset + copySize > dstDesc.ByteWidth)
            {
                T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "copyBuffer : out of range ! src [%zu + %zu / %u] dst [%zu + %zu / %u]", srcOffset, copySize, srcDesc.ByteWidth, dstOffset, copySize, dstDesc.ByteWidth);
                return T3D_ERR_INVALID_PARAM;
            }

            if (srcOffset == 0 && dstOffset == 0 && copySize == srcDesc.ByteWidth && copySize == dstDesc.ByteWidth)
            {
                mD3DDeviceContext->CopyResource(pD3DDst, pD3DSrc);
            }
            else
            {
                // 线性缓冲的 D3D11_BOX 只有 X 方向有意义，其余维度必须填成 0..1
                D3D11_BOX box;
                box.left = static_cast<UINT>(srcOffset);
                box.right = static_cast<UINT>(srcOffset + copySize);
                box.top = 0;
                box.bottom = 1;
                box.front = 0;
                box.back = 1;

                mD3DDeviceContext->CopySubresourceRegion(pD3DDst, 0, static_cast<UINT>(dstOffset), 0, 0, pD3DSrc, 0, &box);
            }

            return T3D_OK;
        };

        return ENQUEUE_UNIQUE_COMMAND(lambda, RenderBufferPtr(src), RenderBufferPtr(dst), srcOffset, size, dstOffset);
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

            // 多线程模式下，Buffer::Data 指向的内存可能在 RHI 线程执行前被释放
            // （栈变量超出作用域或堆内存被 caller 主动 delete）。
            // 因此必须深拷贝数据，让 command 对象持有独立的副本。
            Buffer ownedBuffer;
            ownedBuffer.DataSize = buffer.DataSize;
            ownedBuffer.Data = T3D_POD_NEW_ARRAY(uint8_t, buffer.DataSize);
            memcpy(ownedBuffer.Data, buffer.Data, buffer.DataSize);

            auto lambda = [this](const RenderBufferPtr &renderBuffer, Buffer &buffer, bool discardWholeBuffer)
            {
                TResult ret = T3D_OK;
                
                do
                {
                    ID3D11Resource *pD3DResource = nullptr;
                    
                    switch (renderBuffer->getRHIResource()->getResourceType())
                    {
                    case RHIResource::ResourceType::kVertexBuffer:   /// 顶点缓冲
                        pD3DResource = smart_pointer_cast<D3D11VertexBuffer>(renderBuffer->getRHIResource())->D3DBuffer;
                        break;
                    case RHIResource::ResourceType::kIndexBuffer:    /// 索引缓冲
                        pD3DResource = smart_pointer_cast<D3D11IndexBuffer>(renderBuffer->getRHIResource())->D3DBuffer;
                        break;
                    case RHIResource::ResourceType::kPixelBuffer1D: /// 像素缓冲
                        pD3DResource = smart_pointer_cast<D3D11PixelBuffer1D>(renderBuffer->getRHIResource())->D3DTexture;
                        break;
                    case RHIResource::ResourceType::kPixelBuffer2D:
                        pD3DResource = smart_pointer_cast<D3D11PixelBuffer2D>(renderBuffer->getRHIResource())->D3DTexture;
                        break;
                    case RHIResource::ResourceType::kPixelBuffer3D:
                        pD3DResource = smart_pointer_cast<D3D11PixelBuffer3D>(renderBuffer->getRHIResource())->D3DTexture;
                        break;
                    case RHIResource::ResourceType::kPixelBufferCubemap:
                        pD3DResource = smart_pointer_cast<D3D11PixelBufferCubemap>(renderBuffer->getRHIResource())->D3DTexture;
                        break;
                    case RHIResource::ResourceType::kConstantBuffer: /// 常量缓冲
                        pD3DResource = smart_pointer_cast<D3D11ConstantBuffer>(renderBuffer->getRHIResource())->D3DBuffer;
                        break;
                    default:
                        T3D_ASSERT(false);
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
                } while (false);

                // 释放深拷贝的数据（无论 Map 成功与否都需要释放）
                buffer.release();

                return ret;
            };

            ret = ENQUEUE_UNIQUE_COMMAND(lambda, RenderBufferPtr(renderBuffer), ownedBuffer, discardWholeBuffer);
            
            if (T3D_FAILED(ret))
            {
                break;
            }
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::blitRegion(ID3D11ShaderResourceView *pD3DSRV, ID3D11RenderTargetView *pD3DRTView, ID3D11DepthStencilView *pD3DDSView, uint32_t srcWidth, uint32_t srcHeight, const Vector3 &srcOffset, const Vector3 &srcSize, const Vector3 &dstOffset, const Vector3 &dstSize)
    {
        if (pD3DSRV == nullptr || pD3DRTView == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "blitRegion : source SRV or destination RTV is null !");
            return T3D_ERR_INVALID_POINTER;
        }

        if (srcWidth == 0 || srcHeight == 0)
        {
            T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "blitRegion : invalid source size [%u x %u] !", srcWidth, srcHeight);
            return T3D_ERR_INVALID_PARAM;
        }

        // 把源矩形归一化成 UV，写进动态顶点缓冲，四边形位置固定占满整个 viewport，
        // 目标矩形由下面的 viewport 决定
        const Real u0 = srcOffset.x() / static_cast<Real>(srcWidth);
        const Real v0 = srcOffset.y() / static_cast<Real>(srcHeight);
        const Real u1 = (srcOffset.x() + srcSize.x()) / static_cast<Real>(srcWidth);
        const Real v1 = (srcOffset.y() + srcSize.y()) / static_cast<Real>(srcHeight);

        BlitVertex vertices[4] =
        {
            { Vector3(-1.0f, 1.0f, 0.5f), Vector2(u0, v0) },
            { Vector3(1.0f, 1.0f, 0.5f), Vector2(u1, v0) },
            { Vector3(-1.0f, -1.0f, 0.5f), Vector2(u0, v1) },
            { Vector3(1.0f, -1.0f, 0.5f), Vector2(u1, v1) }
        };

        D3D11_MAPPED_SUBRESOURCE mappedVB;
        memset(&mappedVB, 0, sizeof(mappedVB));
        HRESULT hrMap = mD3DDeviceContext->Map(mBlitVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedVB);
        if (FAILED(hrMap))
        {
            T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "blitRegion : failed to map blit vertex buffer ! DX ERROR [%d]", hrMap);
            return T3D_ERR_D3D11_MAP_RESOURCE;
        }
        memcpy(mappedVB.pData, vertices, sizeof(vertices));
        mD3DDeviceContext->Unmap(mBlitVB, 0);

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
        viewport.Width = dstSize.x();
        viewport.Height = dstSize.y();
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

    void D3D11Context::describeD3DResource(ID3D11Resource *resource, BlitEndpoint &out)
    {
        if (resource == nullptr)
        {
            return;
        }

        D3D11_RESOURCE_DIMENSION dimension = D3D11_RESOURCE_DIMENSION_UNKNOWN;
        resource->GetType(&dimension);

        switch (dimension)
        {
        case D3D11_RESOURCE_DIMENSION_TEXTURE1D:
            {
                D3D11_TEXTURE1D_DESC desc;
                static_cast<ID3D11Texture1D *>(resource)->GetDesc(&desc);
                out.Width = desc.Width;
                out.Height = 1;
                out.Depth = 1;
                out.Format = desc.Format;
                out.SampleCount = 1;
            }
            break;
        case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
            {
                D3D11_TEXTURE2D_DESC desc;
                static_cast<ID3D11Texture2D *>(resource)->GetDesc(&desc);
                out.Width = desc.Width;
                out.Height = desc.Height;
                out.Depth = 1;
                out.Format = desc.Format;
                out.SampleCount = desc.SampleDesc.Count;
            }
            break;
        case D3D11_RESOURCE_DIMENSION_TEXTURE3D:
            {
                D3D11_TEXTURE3D_DESC desc;
                static_cast<ID3D11Texture3D *>(resource)->GetDesc(&desc);
                out.Width = desc.Width;
                out.Height = desc.Height;
                out.Depth = desc.Depth;
                out.Format = desc.Format;
                out.SampleCount = 1;
            }
            break;
        default:
            break;
        }
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::resolveBlitEndpoint(Texture *tex, bool asSource, BlitEndpoint &out)
    {
        if (tex == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "resolveBlitEndpoint : texture is null !");
            return T3D_ERR_INVALID_POINTER;
        }

        PixelBuffer *pixelBuffer = tex->getPixelBuffer();
        if (pixelBuffer == nullptr || pixelBuffer->getRHIResource() == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "resolveBlitEndpoint : texture [%s] has no RHI resource !", tex->getName().c_str());
            return T3D_ERR_INVALID_POINTER;
        }

        if (!asSource && pixelBuffer->getUsage() == Usage::kImmutable)
        {
            T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "resolveBlitEndpoint : texture [%s] is immutable, cannot be a blit destination !", tex->getName().c_str());
            return T3D_ERR_D3D11_INVALID_USAGE;
        }

        RHIResourcePtr rhiResource = pixelBuffer->getRHIResource();

        switch (tex->getTextureType())
        {
        case TEXTURE_TYPE::TT_1D:
            {
                D3D11PixelBuffer1D *d3dBuffer = static_cast<D3D11PixelBuffer1D *>(rhiResource.get());
                out.Resource = d3dBuffer->D3DTexture;
                out.SRView = d3dBuffer->D3DSRView;
            }
            break;
        case TEXTURE_TYPE::TT_2D:
        case TEXTURE_TYPE::TT_2D_ARRAY:
            {
                // 2D 与 2D 数组共用 D3D11PixelBuffer2D，只是 SRV 维度不同
                D3D11PixelBuffer2D *d3dBuffer = static_cast<D3D11PixelBuffer2D *>(rhiResource.get());
                out.Resource = d3dBuffer->D3DTexture;
                out.SRView = d3dBuffer->D3DSRView;
                out.RTView = d3dBuffer->D3DRTView;
                out.DSView = d3dBuffer->D3DDSView;
            }
            break;
        case TEXTURE_TYPE::TT_3D:
            {
                D3D11PixelBuffer3D *d3dBuffer = static_cast<D3D11PixelBuffer3D *>(rhiResource.get());
                out.Resource = d3dBuffer->D3DTexture;
                out.SRView = d3dBuffer->D3DSRView;
            }
            break;
        case TEXTURE_TYPE::TT_CUBE:
        case TEXTURE_TYPE::TT_CUBE_ARRAY:
            {
                // 整资源拷贝可用；区域拷贝的全屏四边形路径需要专门的 cube 采样 shader，
                // 所以这里不给 SRView，doBlit 遇到需要缩放时会明确报错
                D3D11PixelBufferCubemap *d3dBuffer = static_cast<D3D11PixelBufferCubemap *>(rhiResource.get());
                out.Resource = d3dBuffer->D3DTexture;
            }
            break;
        case TEXTURE_TYPE::TT_RENDER_TEXTURE:
            {
                D3D11PixelBuffer2D *d3dBuffer = static_cast<D3D11PixelBuffer2D *>(rhiResource.get());
                out.SRView = d3dBuffer->D3DSRView;
                out.RTView = d3dBuffer->D3DRTView;
                out.DSView = d3dBuffer->D3DDSView;

                if (asSource && d3dBuffer->D3DResolveTex != nullptr)
                {
                    // MSAA 渲染纹理作为源时先解析到非 MSAA 副本，后续路径按普通纹理处理
                    D3D11_TEXTURE2D_DESC msaaDesc;
                    d3dBuffer->D3DTexture->GetDesc(&msaaDesc);
                    mD3DDeviceContext->ResolveSubresource(d3dBuffer->D3DResolveTex, 0, d3dBuffer->D3DTexture, 0, msaaDesc.Format);
                    out.Resource = d3dBuffer->D3DResolveTex;
                }
                else
                {
                    out.Resource = d3dBuffer->D3DTexture;
                }
            }
            break;
        default:
            T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "resolveBlitEndpoint : unsupported texture type [%d] !", tex->getTextureType());
            return T3D_ERR_D3D11_UNSUPPORTED_OPERATION;
        }

        if (out.Resource == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "resolveBlitEndpoint : texture [%s] has no underlying D3D11 texture !", tex->getName().c_str());
            return T3D_ERR_INVALID_POINTER;
        }

        describeD3DResource(out.Resource, out);

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::resolveBlitEndpoint(RenderTarget *rt, bool asSource, BlitEndpoint &out)
    {
        if (rt == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "resolveBlitEndpoint : render target is null !");
            return T3D_ERR_INVALID_POINTER;
        }

        if (rt->getType() == RenderTarget::Type::E_RT_WINDOW)
        {
            D3D11RenderWindow *pD3DRenderWindow = static_cast<D3D11RenderWindow *>(rt->getRenderWindow()->getRHIRenderWindow());
            if (pD3DRenderWindow == nullptr || pD3DRenderWindow->D3DBackBuffer == nullptr)
            {
                T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "resolveBlitEndpoint : render window has no back buffer !");
                return T3D_ERR_INVALID_POINTER;
            }

            out.Resource = pD3DRenderWindow->D3DBackBuffer;
            out.RTView = pD3DRenderWindow->D3DRTView;
            out.DSView = pD3DRenderWindow->D3DDSView;
            // SwapChain 的 BufferUsage 只带 DXGI_USAGE_RENDER_TARGET_OUTPUT，
            // 没有 DXGI_USAGE_SHADER_INPUT，因此 BackBuffer 拿不到 SRV
            out.SRView = nullptr;

            describeD3DResource(out.Resource, out);

            return T3D_OK;
        }

        if (rt->getNumOfRenderTextures() == 0)
        {
            T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "resolveBlitEndpoint : texture render target has no color attachment !");
            return T3D_ERR_INVALID_POINTER;
        }

        // MRT 场景下 blit 语义上只处理 attachment 0
        TResult ret = resolveBlitEndpoint(rt->getRenderTexture().get(), asSource, out);
        if (T3D_FAILED(ret))
        {
            return ret;
        }

        RenderTexturePtr depthStencil = rt->getDepthStencil();
        if (depthStencil != nullptr && depthStencil->getPixelBuffer() != nullptr && depthStencil->getPixelBuffer()->getRHIResource() != nullptr)
        {
            out.DSView = static_cast<D3D11PixelBuffer2D *>(depthStencil->getPixelBuffer()->getRHIResource().get())->D3DDSView;
        }

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    bool D3D11Context::isDirectCopyCompatible(const BlitEndpoint &src, const BlitEndpoint &dst, bool regionCopy) const
    {
        if (src.Format != dst.Format)
        {
            return false;
        }

        if (regionCopy)
        {
            // CopySubresourceRegion 不允许 MSAA 资源参与
            return src.SampleCount == 1 && dst.SampleCount == 1;
        }

        return src.Width == dst.Width && src.Height == dst.Height && src.Depth == dst.Depth && src.SampleCount == dst.SampleCount;
    }

    //--------------------------------------------------------------------------

    TResult D3D11Context::doBlit(const BlitEndpoint &src, const BlitEndpoint &dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 &dstOffset)
    {
        if (src.Resource == nullptr || dst.Resource == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "doBlit : source or destination resource is null !");
            return T3D_ERR_INVALID_POINTER;
        }

        if (src.Resource == dst.Resource)
        {
            T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER, "doBlit : source and destination are the same resource, behaviour is undefined !");
            return T3D_ERR_D3D11_INCOMPATIBLE_COPY;
        }

        if (size == Vector3::ZERO)
        {
            // 整资源传输
            if (isDirectCopyCompatible(src, dst, false))
            {
                mD3DDeviceContext->CopyResource(dst.Resource, src.Resource);
                return T3D_OK;
            }

            if (src.SampleCount > 1 && dst.SampleCount == 1 && src.Width == dst.Width && src.Height == dst.Height)
            {
                mD3DDeviceContext->ResolveSubresource(dst.Resource, 0, src.Resource, 0, dst.Format);
                return T3D_OK;
            }

            if (src.SRView != nullptr && dst.RTView != nullptr)
            {
                // 跨格式或需要缩放，走全屏四边形
                return blitRegion(src.SRView, dst.RTView, dst.DSView, src.Width, src.Height,
                    Vector3::ZERO, Vector3(static_cast<Real>(src.Width), static_cast<Real>(src.Height), 1.0f),
                    Vector3::ZERO, Vector3(static_cast<Real>(dst.Width), static_cast<Real>(dst.Height), 1.0f));
            }
        }
        else
        {
            // 区域传输
            if (isDirectCopyCompatible(src, dst, true))
            {
                D3D11_BOX box;
                box.left = static_cast<UINT>(srcOffset.x());
                box.top = static_cast<UINT>(srcOffset.y());
                box.front = static_cast<UINT>(srcOffset.z());
                box.right = static_cast<UINT>(srcOffset.x() + size.x());
                box.bottom = static_cast<UINT>(srcOffset.y() + size.y());
                box.back = std::max<UINT>(1, static_cast<UINT>(srcOffset.z() + size.z()));

                mD3DDeviceContext->CopySubresourceRegion(dst.Resource, 0,
                    static_cast<UINT>(dstOffset.x()), static_cast<UINT>(dstOffset.y()), static_cast<UINT>(dstOffset.z()),
                    src.Resource, 0, &box);
                return T3D_OK;
            }

            if (src.SRView != nullptr && dst.RTView != nullptr)
            {
                return blitRegion(src.SRView, dst.RTView, dst.DSView, src.Width, src.Height, srcOffset, size, dstOffset, size);
            }
        }

        T3D_LOG_ERROR(LOG_TAG_D3D11RENDERER,
            "doBlit : incompatible endpoints. src[%ux%ux%u fmt=%d samples=%u srv=%s] dst[%ux%ux%u fmt=%d samples=%u rtv=%s]",
            src.Width, src.Height, src.Depth, src.Format, src.SampleCount, src.SRView != nullptr ? "yes" : "no",
            dst.Width, dst.Height, dst.Depth, dst.Format, dst.SampleCount, dst.RTView != nullptr ? "yes" : "no");

        return T3D_ERR_D3D11_INCOMPATIBLE_COPY;
    }

    //--------------------------------------------------------------------------

    ID3D11Resource *D3D11Context::getD3DResource(RenderBuffer *buffer)
    {
        if (buffer == nullptr || buffer->getRHIResource() == nullptr)
        {
            return nullptr;
        }

        const RHIResourcePtr &rhiResource = buffer->getRHIResource();

        switch (rhiResource->getResourceType())
        {
        case RHIResource::ResourceType::kVertexBuffer:
            return smart_pointer_cast<D3D11VertexBuffer>(rhiResource)->D3DBuffer;
        case RHIResource::ResourceType::kIndexBuffer:
            return smart_pointer_cast<D3D11IndexBuffer>(rhiResource)->D3DBuffer;
        case RHIResource::ResourceType::kConstantBuffer:
            return smart_pointer_cast<D3D11ConstantBuffer>(rhiResource)->D3DBuffer;
        case RHIResource::ResourceType::kPixelBuffer1D:
            return smart_pointer_cast<D3D11PixelBuffer1D>(rhiResource)->D3DTexture;
        case RHIResource::ResourceType::kPixelBuffer2D:
            return smart_pointer_cast<D3D11PixelBuffer2D>(rhiResource)->D3DTexture;
        case RHIResource::ResourceType::kPixelBuffer3D:
            return smart_pointer_cast<D3D11PixelBuffer3D>(rhiResource)->D3DTexture;
        case RHIResource::ResourceType::kPixelBufferCubemap:
            return smart_pointer_cast<D3D11PixelBufferCubemap>(rhiResource)->D3DTexture;
        default:
            return nullptr;
        }
    }

    //--------------------------------------------------------------------------
    
    TResult D3D11Context::setSamplers(SetSamplerState setSamplerState, uint32_t startSlot, const Samplers &samplers)
    {
        auto lambda = [this](SetSamplerState setSamplerState, uint32_t startSlot, const Samplers &samplers)
        {
            using D3D11Samplers = TArray<ID3D11SamplerState*>;
            D3D11Samplers d3dSamplers(samplers.size(), nullptr);
            for (uint32_t i = 0; i< samplers.size(); ++i)
            {
                if (samplers[i] != nullptr)
                {
                    d3dSamplers[i] = ((D3D11SamplerState*)samplers[i]->getRHIState().get())->D3DSamplerState;
                }
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
            TArray<ID3D11ShaderResourceView*> d3dSRViews(buffers.size(), nullptr);
        
            for (uint32_t i = 0 ; i < buffers.size(); ++i)
            {
                const auto &buffer = buffers[i];

                if (buffer != nullptr)
                {
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
                        d3dSRViews[i] = smart_pointer_cast<D3D11PixelBufferCubemap>(buffers[i]->getRHIResource())->D3DSRView;
                        break;
                    }    
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

