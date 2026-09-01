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


#include "T3DMetalContext.h"
#include "T3DMetalWindow.h"
#include "T3DMetalRenderBuffer.h"
#include "T3DMetalShader.h"
#include "T3DMetalRenderState.h"
#include "T3DMetalMapping.h"
#include "T3DMetalError.h"

#include "Render/T3DRenderWindow.h"
#include "Render/T3DRenderTarget.h"
#include "Render/T3DRenderTexture.h"
#include "Render/T3DVertexBuffer.h"
#include "Render/T3DIndexBuffer.h"
#include "Render/T3DConstantBuffer.h"
#include "Render/T3DPixelBuffer.h"
#include "Render/T3DVertexDeclaration.h"
#include "Resource/T3DTexture.h"
#include "Material/T3DShaderVariant.h"

#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>

#include <algorithm>
#include <unordered_map>


namespace Tiny3D
{
    namespace
    {
        struct VBSlot
        {
            id<MTLBuffer> buffer {nil};
            uint32_t stride {0};
            uint32_t offset {0};
        };

        bool IsDepthFormat(PixelFormat format)
        {
            switch (format)
            {
            case PixelFormat::E_PF_D16_UNORM:
            case PixelFormat::E_PF_D24_UNORM_S8_UINT:
            case PixelFormat::E_PF_D32_FLOAT:
            case PixelFormat::E_PF_D32_FLOAT_S8X24_UINT:
                return true;
            default:
                return false;
            }
        }

        bool HasStencil(MTLPixelFormat format)
        {
            return format == MTLPixelFormatDepth32Float_Stencil8
                || format == MTLPixelFormatDepth24Unorm_Stencil8;
        }

        uint32_t ClampSampleCount(id<MTLDevice> device, uint32_t count)
        {
            if (device == nil || count <= 1)
            {
                return 1;
            }

            while (count > 1 && ![device supportsTextureSampleCount:count])
            {
                count >>= 1;
            }
            return std::max(1u, count);
        }

        MetalPixelBuffer2D *GetMetalPB2D(Texture *texture)
        {
            if (texture == nullptr || texture->getPixelBuffer() == nullptr)
            {
                return nullptr;
            }

            return static_cast<MetalPixelBuffer2D *>(
                texture->getPixelBuffer()->getRHIResource().get());
        }

        id<MTLTexture> GetNativeTexture(Texture *texture)
        {
            MetalPixelBuffer2D *pb = GetMetalPB2D(texture);
            if (pb == nullptr || pb->getNativeObject() == nullptr)
            {
                return nil;
            }
            return (__bridge id<MTLTexture>)pb->getNativeObject();
        }

        id<MTLTexture> GetBlitSourceTexture(Texture *texture)
        {
            MetalPixelBuffer2D *pb = GetMetalPB2D(texture);
            if (pb == nullptr)
            {
                return nil;
            }
            if (pb->getResolveNative() != nullptr)
            {
                return (__bridge id<MTLTexture>)pb->getResolveNative();
            }
            return GetNativeTexture(texture);
        }
    }

    struct MetalContext::Impl
    {
        id<MTLDevice>               device {nil};
        id<MTLCommandQueue>         queue {nil};
        id<MTLCommandBuffer>        cmd {nil};
        id<MTLRenderCommandEncoder> encoder {nil};
        id<MTLTexture>              windowDepthTex {nil};
        id<MTLTexture>              currentColor {nil};
        id<MTLTexture>              currentResolve {nil};
        id<MTLTexture>              currentDepth {nil};
        id<MTLDepthStencilState>    defaultDSS {nil};

        MetalWindowPtr              window;
        RenderTarget               *currentRT {nullptr};
        MetalBlendStatePtr          blendState;
        MetalRasterizerStatePtr     rasterState;
        MetalDepthStencilStatePtr   depthState;
        MetalVertexDeclarationPtr   vertexDecl;
        MetalVertexShaderPtr        vs;
        MetalPixelShaderPtr         ps;
        MetalIndexBufferPtr         ib;

        VBSlot                      vb[8];
        PrimitiveType               primitive {PrimitiveType::kTriangleList};
        Viewport                    viewport {};
        MTLScissorRect              scissor {};
        bool                        hasScissor {false};

        MTLClearColor               clearColor {0.0, 0.0, 0.0, 1.0};
        double                      clearDepth {1.0};
        uint32_t                    clearStencil {0};
        bool                        pendingColorClear {false};
        bool                        pendingDepthClear {false};
        bool                        pendingStencilClear {false};

        MTLPixelFormat              colorFormat {MTLPixelFormatBGRA8Unorm};
        MTLPixelFormat              depthFormat {MTLPixelFormatInvalid};
        MTLPixelFormat              stencilFormat {MTLPixelFormatInvalid};

        std::unordered_map<uint64_t, id<MTLRenderPipelineState>> psoCache;
    };

    //--------------------------------------------------------------------------

    MetalContextPtr MetalContext::create()
    {
        MetalContextPtr context = T3D_NEW MetalContext();
        if (context != nullptr && T3D_FAILED(context->init()))
        {
            context = nullptr;
        }
        return context;
    }

    //--------------------------------------------------------------------------

    MetalContext::MetalContext()
        : mImpl(new Impl())
    {
    }

    //--------------------------------------------------------------------------

    MetalContext::~MetalContext()
    {
        if (mImpl != nullptr)
        {
            mImpl->encoder = nil;
            mImpl->cmd = nil;
            mImpl->psoCache.clear();
            delete mImpl;
            mImpl = nullptr;
        }
    }

    //--------------------------------------------------------------------------

    TResult MetalContext::init()
    {
        TResult ret = T3D_OK;

        do
        {
            mImpl->device = MTLCreateSystemDefaultDevice();
            if (mImpl->device == nil)
            {
                ret = T3D_ERR_METAL_DEVICE;
                T3D_LOG_ERROR(LOG_TAG_METALRENDERER, "MTLCreateSystemDefaultDevice failed !");
                break;
            }

            mImpl->queue = [mImpl->device newCommandQueue];
            if (mImpl->queue == nil)
            {
                ret = T3D_ERR_METAL_DEVICE;
                T3D_LOG_ERROR(LOG_TAG_METALRENDERER, "Failed to create MTLCommandQueue !");
                break;
            }

            MTLDepthStencilDescriptor *dssDesc = [[MTLDepthStencilDescriptor alloc] init];
            dssDesc.depthCompareFunction = MTLCompareFunctionAlways;
            dssDesc.depthWriteEnabled = NO;
            mImpl->defaultDSS = [mImpl->device newDepthStencilStateWithDescriptor:dssDesc];

            collectInformation();
            fillCapabilities();
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    void MetalContext::fillCapabilities()
    {
        // 实例化与 per-instance step function 是 Metal 核心功能
        mCapabilities.supportsInstancing = true;
        mCapabilities.supportsBaseInstance = true;

        // compute / UAV / indirect 链路尚未接入，保持 false 走上层降级路径
        mCapabilities.supportsCompute = false;
        mCapabilities.supportsUnorderedAccess = false;
        mCapabilities.supportsStructuredBuffer = false;
        mCapabilities.supportsIndirectDraw = false;
        mCapabilities.supportsIndirectDispatch = false;
        mCapabilities.supportsAppendConsumeBuffer = false;
    }

    //--------------------------------------------------------------------------

    void *MetalContext::getNativeDevice() const
    {
        return (mImpl != nullptr) ? (__bridge void *)mImpl->device : nullptr;
    }

    //--------------------------------------------------------------------------

    TResult MetalContext::collectInformation()
    {
        if (mImpl != nullptr && mImpl->device != nil)
        {
            T3D_LOG_INFO(LOG_TAG_METALRENDERER, "Metal device: %s",
                [[mImpl->device name] UTF8String]);
        }
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult MetalContext::setViewProjectionTransform(const Matrix4 &viewMat, const Matrix4 &projMat)
    {
        static Matrix4 conversionMat(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.5f, 0.5f,
            0.0f, 0.0f, 0.0f, 1.0f);

        mViewMatrix = viewMat;
        mProjMatrix = conversionMat * projMat;
        mProjViewMatrix = mProjMatrix * mViewMatrix;
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    RHIRenderTargetPtr MetalContext::createRenderWindow(RenderWindow *renderWindow)
    {
        MetalWindowPtr window = MetalWindow::create(renderWindow, getNativeDevice());
        if (window != nullptr)
        {
            mImpl->window = window;
        }
        return window;
    }

    //--------------------------------------------------------------------------

    void *MetalContext::createMTLBuffer(size_t size, const void *data)
    {
        if (mImpl == nullptr || mImpl->device == nil || size == 0)
        {
            return nullptr;
        }

        id<MTLBuffer> buffer = [mImpl->device newBufferWithLength:size
            options:MTLResourceStorageModeShared];
        if (buffer == nil)
        {
            return nullptr;
        }

        if (data != nullptr)
        {
            memcpy([buffer contents], data, size);
        }

        return (__bridge void *)buffer;
    }

    //--------------------------------------------------------------------------

    void MetalContext::endEncoder()
    {
        if (mImpl != nullptr && mImpl->encoder != nil)
        {
            [mImpl->encoder endEncoding];
            mImpl->encoder = nil;
        }
    }

    //--------------------------------------------------------------------------

    void MetalContext::getCurrentTargetSize(uint32_t &width, uint32_t &height) const
    {
        width = 0;
        height = 0;

        if (mImpl->currentColor != nil)
        {
            width = (uint32_t)mImpl->currentColor.width;
            height = (uint32_t)mImpl->currentColor.height;
            return;
        }

        if (mImpl->currentDepth != nil)
        {
            width = (uint32_t)mImpl->currentDepth.width;
            height = (uint32_t)mImpl->currentDepth.height;
            return;
        }

        if (mImpl->window != nullptr)
        {
            width = mImpl->window->getWidth();
            height = mImpl->window->getHeight();
        }
    }

    //--------------------------------------------------------------------------

    TResult MetalContext::ensureEncoder()
    {
        if (mImpl->encoder != nil)
        {
            return T3D_OK;
        }

        if (mImpl->cmd == nil)
        {
            return T3D_ERR_INVALID_POINTER;
        }

        id<MTLTexture> colorTex = mImpl->currentColor;
        id<MTLTexture> resolveTex = mImpl->currentResolve;
        id<MTLTexture> depthTex = mImpl->currentDepth;

        if (colorTex == nil
            && (mImpl->currentRT == nullptr
                || mImpl->currentRT->getType() == RenderTarget::Type::E_RT_WINDOW))
        {
            if (mImpl->window == nullptr)
            {
                return T3D_ERR_INVALID_POINTER;
            }

            id<CAMetalDrawable> drawable = (__bridge id<CAMetalDrawable>)mImpl->window->getDrawable();
            if (drawable == nil)
            {
                drawable = (__bridge id<CAMetalDrawable>)mImpl->window->acquireDrawable();
            }
            if (drawable == nil)
            {
                T3D_LOG_ERROR(LOG_TAG_METALRENDERER, "nextDrawable returned nil !");
                return T3D_ERR_METAL_DRAWABLE;
            }
            colorTex = drawable.texture;
        }

        if (colorTex == nil && depthTex == nil)
        {
            return T3D_ERR_INVALID_POINTER;
        }

        if (depthTex == nil && mImpl->window != nullptr && mImpl->currentRT != nullptr
            && mImpl->currentRT->getType() == RenderTarget::Type::E_RT_WINDOW)
        {
            const uint32_t w = (uint32_t)colorTex.width;
            const uint32_t h = (uint32_t)colorTex.height;
            if (mImpl->windowDepthTex == nil
                || mImpl->windowDepthTex.width != w
                || mImpl->windowDepthTex.height != h)
            {
                MTLTextureDescriptor *depthDesc = [MTLTextureDescriptor
                    texture2DDescriptorWithPixelFormat:MTLPixelFormatDepth32Float
                    width:w height:h mipmapped:NO];
                depthDesc.storageMode = MTLStorageModePrivate;
                depthDesc.usage = MTLTextureUsageRenderTarget;
                mImpl->windowDepthTex = [mImpl->device newTextureWithDescriptor:depthDesc];
            }
            depthTex = mImpl->windowDepthTex;
        }

        MTLRenderPassDescriptor *pass = [MTLRenderPassDescriptor renderPassDescriptor];
        if (colorTex != nil)
        {
            pass.colorAttachments[0].texture = colorTex;
            pass.colorAttachments[0].loadAction = mImpl->pendingColorClear
                ? MTLLoadActionClear : MTLLoadActionLoad;
            pass.colorAttachments[0].clearColor = mImpl->clearColor;
            if (resolveTex != nil && colorTex.sampleCount > 1)
            {
                pass.colorAttachments[0].storeAction = MTLStoreActionMultisampleResolve;
                pass.colorAttachments[0].resolveTexture = resolveTex;
            }
            else
            {
                pass.colorAttachments[0].storeAction = MTLStoreActionStore;
            }
            mImpl->colorFormat = colorTex.pixelFormat;
        }
        else
        {
            mImpl->colorFormat = MTLPixelFormatInvalid;
        }

        if (depthTex != nil)
        {
            pass.depthAttachment.texture = depthTex;
            pass.depthAttachment.loadAction = mImpl->pendingDepthClear
                ? MTLLoadActionClear : MTLLoadActionLoad;
            pass.depthAttachment.storeAction = MTLStoreActionDontCare;
            pass.depthAttachment.clearDepth = mImpl->clearDepth;
            mImpl->depthFormat = depthTex.pixelFormat;

            if (HasStencil(depthTex.pixelFormat))
            {
                pass.stencilAttachment.texture = depthTex;
                pass.stencilAttachment.loadAction = mImpl->pendingStencilClear
                    ? MTLLoadActionClear : MTLLoadActionLoad;
                pass.stencilAttachment.storeAction = MTLStoreActionDontCare;
                pass.stencilAttachment.clearStencil = mImpl->clearStencil;
                mImpl->stencilFormat = depthTex.pixelFormat;
            }
            else
            {
                mImpl->stencilFormat = MTLPixelFormatInvalid;
            }
        }
        else
        {
            mImpl->depthFormat = MTLPixelFormatInvalid;
            mImpl->stencilFormat = MTLPixelFormatInvalid;
        }

        mImpl->pendingColorClear = false;
        mImpl->pendingDepthClear = false;
        mImpl->pendingStencilClear = false;

        mImpl->encoder = [mImpl->cmd renderCommandEncoderWithDescriptor:pass];
        if (mImpl->encoder == nil)
        {
            return T3D_ERR_METAL_DEVICE;
        }

        applyEncoderState();
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    void MetalContext::applyEncoderState()
    {
        if (mImpl->encoder == nil)
        {
            return;
        }

        uint32_t w = 0;
        uint32_t h = 0;
        getCurrentTargetSize(w, h);
        if (w == 0 || h == 0)
        {
            return;
        }

        MTLViewport vp;
        vp.originX = mImpl->viewport.Left * (double)w;
        vp.originY = mImpl->viewport.Top * (double)h;
        vp.width = std::max<double>(1.0, mImpl->viewport.Width * (double)w);
        vp.height = std::max<double>(1.0, mImpl->viewport.Height * (double)h);
        vp.znear = mImpl->viewport.MinDepth;
        vp.zfar = mImpl->viewport.MaxDepth;
        [mImpl->encoder setViewport:vp];

        if (mImpl->hasScissor)
        {
            [mImpl->encoder setScissorRect:mImpl->scissor];
        }

        if (mImpl->rasterState != nullptr)
        {
            const RasterizerDesc &rd = mImpl->rasterState->getDesc();
            [mImpl->encoder setCullMode:MetalMapCull(rd.CullMode)];
            [mImpl->encoder setFrontFacingWinding:MetalMapWinding(rd.FrontAnticlockwise)];
            [mImpl->encoder setTriangleFillMode:MetalMapFill(rd.FillMode)];
            [mImpl->encoder setDepthBias:rd.DepthBias slopeScale:rd.SlopeScaledDepthBias clamp:rd.DepthBiasClamp];
        }

        id<MTLDepthStencilState> dss = mImpl->defaultDSS;
        uint32_t stencilRef = 0;
        if (mImpl->depthState != nullptr && mImpl->depthState->getNativeObject() != nullptr)
        {
            dss = (__bridge id<MTLDepthStencilState>)mImpl->depthState->getNativeObject();
            stencilRef = mImpl->depthState->getStencilRef();
        }
        [mImpl->encoder setDepthStencilState:dss];
        [mImpl->encoder setStencilReferenceValue:stencilRef];
    }

    //--------------------------------------------------------------------------

    TResult MetalContext::bindCurrentPipeline()
    {
        if (mImpl->vs == nullptr || mImpl->ps == nullptr)
        {
            return T3D_ERR_INVALID_POINTER;
        }

        const uint64_t key =
            ((uint64_t)(uintptr_t)mImpl->vs->getNativeObject() << 1)
            ^ (uint64_t)(uintptr_t)mImpl->ps->getNativeObject()
            ^ ((uint64_t)(uintptr_t)mImpl->blendState.get() << 8)
            ^ ((uint64_t)(uintptr_t)mImpl->vertexDecl.get() << 16)
            ^ ((uint64_t)mImpl->colorFormat << 24)
            ^ ((uint64_t)mImpl->depthFormat << 32)
            ^ ((uint64_t)mImpl->stencilFormat << 40);

        auto it = mImpl->psoCache.find(key);
        id<MTLRenderPipelineState> pso = nil;
        if (it != mImpl->psoCache.end())
        {
            pso = it->second;
        }
        else
        {
            MTLRenderPipelineDescriptor *desc = [[MTLRenderPipelineDescriptor alloc] init];
            desc.vertexFunction = (__bridge id<MTLFunction>)mImpl->vs->getNativeObject();
            desc.fragmentFunction = (__bridge id<MTLFunction>)mImpl->ps->getNativeObject();
            desc.colorAttachments[0].pixelFormat = mImpl->colorFormat;
            desc.depthAttachmentPixelFormat = mImpl->depthFormat;
            desc.stencilAttachmentPixelFormat = mImpl->stencilFormat;

            if (mImpl->blendState != nullptr)
            {
                const BlendDesc::RTBlendDesc &rt = mImpl->blendState->getDesc().RenderTargetStates[0];
                MTLRenderPipelineColorAttachmentDescriptor *ca = desc.colorAttachments[0];
                ca.blendingEnabled = rt.BlendEnable;
                ca.sourceRGBBlendFactor = MetalMapBlendFactor(rt.SrcBlend);
                ca.destinationRGBBlendFactor = MetalMapBlendFactor(rt.DestBlend);
                ca.rgbBlendOperation = MetalMapBlendOp(rt.BlendOp);
                ca.sourceAlphaBlendFactor = MetalMapBlendFactor(rt.SrcBlendAlpha);
                ca.destinationAlphaBlendFactor = MetalMapBlendFactor(rt.DstBlendAlpha);
                ca.alphaBlendOperation = MetalMapBlendOp(rt.BlendOpAlpha);
                ca.writeMask = (MTLColorWriteMask)rt.ColorMask;
            }

            if (mImpl->vertexDecl != nullptr)
            {
                MTLVertexDescriptor *vd = [[MTLVertexDescriptor alloc] init];
                const VertexAttributes &attrs = mImpl->vertexDecl->getAttributes();
                uint32_t strides[8] = {0};
                bool perInstance[8] = {false};
                uint32_t stepRates[8] = {0};
                for (size_t i = 0; i < attrs.size(); ++i)
                {
                    const VertexAttribute &a = attrs[i];
                    const uint32_t slot = a.getSlot();
                    vd.attributes[i].format = MetalMapVertexFormat(a.getType());
                    vd.attributes[i].offset = a.getOffset();
                    vd.attributes[i].bufferIndex = slot;
                    const uint32_t end = a.getOffset() + a.getSize();
                    if (end > strides[slot])
                    {
                        strides[slot] = end;
                    }
                    // 同 slot 的 InputRate 已由 VertexDeclaration::validateInputRates() 保证一致
                    perInstance[slot] = (a.getInputRate() == VertexAttribute::InputRate::kPerInstance);
                    stepRates[slot] = a.getInstanceStepRate();
                }
                for (uint32_t slot = 0; slot < 8; ++slot)
                {
                    if (strides[slot] > 0)
                    {
                        vd.layouts[slot].stride = strides[slot];
                        if (perInstance[slot])
                        {
                            vd.layouts[slot].stepFunction = MTLVertexStepFunctionPerInstance;
                            vd.layouts[slot].stepRate = (stepRates[slot] > 0) ? stepRates[slot] : 1;
                        }
                        else
                        {
                            vd.layouts[slot].stepFunction = MTLVertexStepFunctionPerVertex;
                            vd.layouts[slot].stepRate = 1;
                        }
                    }
                }
                desc.vertexDescriptor = vd;
            }

            NSError *error = nil;
            pso = [mImpl->device newRenderPipelineStateWithDescriptor:desc error:&error];
            if (pso == nil)
            {
                T3D_LOG_ERROR(LOG_TAG_METALRENDERER, "Failed to create PSO: %s",
                    error != nil ? [[error localizedDescription] UTF8String] : "unknown");
                return T3D_ERR_METAL_PSO;
            }
            mImpl->psoCache[key] = pso;
        }

        [mImpl->encoder setRenderPipelineState:pso];
        applyEncoderState();

        for (uint32_t i = 0; i < 8; ++i)
        {
            if (mImpl->vb[i].buffer != nil)
            {
                [mImpl->encoder setVertexBuffer:mImpl->vb[i].buffer
                    offset:mImpl->vb[i].offset atIndex:i];
            }
        }

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    void *MetalContext::compileMSLFunction(ShaderVariant *shader)
    {
        if (shader == nullptr || mImpl->device == nil)
        {
            return nullptr;
        }

        size_t length = 0;
        const char *code = shader->getBytesCode(length);
        if (code == nullptr || length == 0)
        {
            T3D_LOG_ERROR(LOG_TAG_METALRENDERER, "Shader source is empty !");
            return nullptr;
        }

        NSString *src = [[NSString alloc] initWithBytes:code length:length
            encoding:NSUTF8StringEncoding];
        if (src == nil)
        {
            src = [[NSString alloc] initWithBytes:code length:length
                encoding:NSASCIIStringEncoding];
        }

        NSError *error = nil;
        id<MTLLibrary> library = [mImpl->device newLibraryWithSource:src options:nil error:&error];
        if (library == nil)
        {
            T3D_LOG_ERROR(LOG_TAG_METALRENDERER, "newLibraryWithSource failed: %s",
                error != nil ? [[error localizedDescription] UTF8String] : "unknown");
            return nullptr;
        }

        const char *names[] = { "main", "vertex_main", "fragment_main", "vs_main", "ps_main", nullptr };
        id<MTLFunction> function = nil;
        for (int i = 0; names[i] != nullptr && function == nil; ++i)
        {
            function = [library newFunctionWithName:[NSString stringWithUTF8String:names[i]]];
        }

        if (function == nil)
        {
            NSArray<NSString *> *fnNames = [library functionNames];
            if (fnNames.count > 0)
            {
                function = [library newFunctionWithName:fnNames.firstObject];
            }
        }

        if (function == nil)
        {
            T3D_LOG_ERROR(LOG_TAG_METALRENDERER, "No Metal function found in shader library !");
            return nullptr;
        }

        return (__bridge void *)function;
    }

    //--------------------------------------------------------------------------

    RHIPixelBuffer2DPtr MetalContext::createRenderTexture(PixelBuffer2D *buffer)
    {
        MetalPixelBuffer2DPtr metalBuffer = MetalPixelBuffer2D::create();
        if (buffer == nullptr || mImpl->device == nil)
        {
            return metalBuffer;
        }

        const PixelBuffer2DDesc &desc = buffer->getDescriptor();
        const MTLPixelFormat format = MetalMapPixelFormat(desc.format);
        const uint32_t width = std::max(1u, desc.width);
        const uint32_t height = std::max(1u, desc.height);
        const uint32_t samples = ClampSampleCount(mImpl->device, desc.sampleDesc.Count);
        const bool isDepth = IsDepthFormat(desc.format);

        if (samples != desc.sampleDesc.Count && desc.sampleDesc.Count > 1)
        {
            T3D_LOG_WARNING(LOG_TAG_METALRENDERER,
                "MSAA sample count %u is not supported, clamped to %u",
                desc.sampleDesc.Count, samples);
        }

        MTLTextureDescriptor *td = [[MTLTextureDescriptor alloc] init];
        td.pixelFormat = format;
        td.width = width;
        td.height = height;
        td.mipmapLevelCount = 1;
        td.storageMode = MTLStorageModePrivate;
        td.usage = MTLTextureUsageRenderTarget;
        if (!isDepth || desc.shaderReadable)
        {
            td.usage |= MTLTextureUsageShaderRead;
        }

        if (samples > 1)
        {
            td.textureType = MTLTextureType2DMultisample;
            td.sampleCount = samples;
        }
        else
        {
            td.textureType = MTLTextureType2D;
            td.sampleCount = 1;
        }

        id<MTLTexture> tex = [mImpl->device newTextureWithDescriptor:td];
        if (tex == nil)
        {
            T3D_LOG_ERROR(LOG_TAG_METALRENDERER,
                "Failed to create render texture %ux%u format=%d samples=%u",
                width, height, (int)desc.format, samples);
            return nullptr;
        }
        metalBuffer->setNativeObject((__bridge void *)tex);

        // MSAA 颜色目标需要 resolve 纹理，才能 blit 到窗口
        if (samples > 1 && !isDepth)
        {
            MTLTextureDescriptor *resolveDesc = [[MTLTextureDescriptor alloc] init];
            resolveDesc.textureType = MTLTextureType2D;
            resolveDesc.pixelFormat = format;
            resolveDesc.width = width;
            resolveDesc.height = height;
            resolveDesc.mipmapLevelCount = 1;
            resolveDesc.sampleCount = 1;
            resolveDesc.storageMode = MTLStorageModePrivate;
            resolveDesc.usage = MTLTextureUsageRenderTarget | MTLTextureUsageShaderRead;
            id<MTLTexture> resolveTex = [mImpl->device newTextureWithDescriptor:resolveDesc];
            if (resolveTex == nil)
            {
                T3D_LOG_ERROR(LOG_TAG_METALRENDERER,
                    "Failed to create MSAA resolve texture %ux%u", width, height);
                return nullptr;
            }
            metalBuffer->setResolveNative((__bridge void *)resolveTex);
        }

        return metalBuffer;
    }

    //--------------------------------------------------------------------------

    TResult MetalContext::setRenderTarget(RenderTarget *renderTarget)
    {
        endEncoder();

        mImpl->currentRT = renderTarget;
        mImpl->currentColor = nil;
        mImpl->currentResolve = nil;
        mImpl->currentDepth = nil;

        if (renderTarget == nullptr)
        {
            return T3D_OK;
        }

        if (renderTarget->getType() == RenderTarget::Type::E_RT_WINDOW
            && renderTarget->getRenderWindow() != nullptr)
        {
            mImpl->window = static_cast<MetalWindow *>(
                renderTarget->getRenderWindow()->getRHIRenderWindow());
        }
        else if (renderTarget->getNumOfRenderTextures() > 0)
        {
            RenderTexture *color = renderTarget->getRenderTexture();
            mImpl->currentColor = GetNativeTexture(color);
            MetalPixelBuffer2D *pb = GetMetalPB2D(color);
            if (pb != nullptr && pb->getResolveNative() != nullptr)
            {
                mImpl->currentResolve = (__bridge id<MTLTexture>)pb->getResolveNative();
            }
        }

        if (renderTarget->getDepthStencil() != nullptr)
        {
            mImpl->currentDepth = GetNativeTexture(renderTarget->getDepthStencil());
        }

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult MetalContext::resetRenderTarget()
    {
        endEncoder();
        mImpl->currentRT = nullptr;
        mImpl->currentColor = nil;
        mImpl->currentResolve = nil;
        mImpl->currentDepth = nil;
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult MetalContext::setViewport(const Viewport &viewport)
    {
        mImpl->viewport = viewport;
        if (mImpl->encoder != nil)
        {
            applyEncoderState();
        }
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult MetalContext::setScissorRect(int32_t x, int32_t y, uint32_t width, uint32_t height)
    {
        mImpl->scissor.x = (NSUInteger)std::max(0, x);
        mImpl->scissor.y = (NSUInteger)std::max(0, y);
        mImpl->scissor.width = width;
        mImpl->scissor.height = height;
        mImpl->hasScissor = true;
        if (mImpl->encoder != nil)
        {
            [mImpl->encoder setScissorRect:mImpl->scissor];
        }
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult MetalContext::clearColor(const ColorRGB &color)
    {
        mImpl->clearColor = MTLClearColorMake(color.red(), color.green(), color.blue(), 1.0);
        mImpl->pendingColorClear = true;
        endEncoder();
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult MetalContext::clearDepth(Real depth)
    {
        mImpl->clearDepth = depth;
        mImpl->pendingDepthClear = true;
        endEncoder();
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult MetalContext::clearDepthStencil(Real depth, uint32_t stencil)
    {
        mImpl->clearDepth = depth;
        mImpl->clearStencil = stencil;
        mImpl->pendingDepthClear = true;
        mImpl->pendingStencilClear = true;
        endEncoder();
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    RHIBlendStatePtr MetalContext::createBlendState(BlendState *state)
    {
        MetalBlendStatePtr metalState = MetalBlendState::create();
        if (state != nullptr)
        {
            metalState->setDesc(state->getStateDesc());
        }
        return metalState;
    }

    //--------------------------------------------------------------------------

    RHIDepthStencilStatePtr MetalContext::createDepthStencilState(DepthStencilState *state)
    {
        MetalDepthStencilStatePtr metalState = MetalDepthStencilState::create();
        if (state == nullptr || mImpl->device == nil)
        {
            return metalState;
        }

        const DepthStencilDesc &desc = state->getStateDesc();
        MTLDepthStencilDescriptor *dss = [[MTLDepthStencilDescriptor alloc] init];
        dss.depthCompareFunction = desc.DepthTestEnable
            ? MetalMapCompare(desc.DepthFunc) : MTLCompareFunctionAlways;
        dss.depthWriteEnabled = desc.DepthWriteEnable;

        if (desc.StencilEnable)
        {
            MTLStencilDescriptor *front = [[MTLStencilDescriptor alloc] init];
            front.stencilCompareFunction = MetalMapCompare(desc.FrontFace.StencilFunc);
            front.stencilFailureOperation = MetalMapStencilOp(desc.FrontFace.StencilFailOp);
            front.depthFailureOperation = MetalMapStencilOp(desc.FrontFace.StencilDepthFailOp);
            front.depthStencilPassOperation = MetalMapStencilOp(desc.FrontFace.StencilPassOp);
            front.readMask = desc.StencilReadMask;
            front.writeMask = desc.StencilWriteMask;
            dss.frontFaceStencil = front;

            MTLStencilDescriptor *back = [[MTLStencilDescriptor alloc] init];
            back.stencilCompareFunction = MetalMapCompare(desc.BackFace.StencilFunc);
            back.stencilFailureOperation = MetalMapStencilOp(desc.BackFace.StencilFailOp);
            back.depthFailureOperation = MetalMapStencilOp(desc.BackFace.StencilDepthFailOp);
            back.depthStencilPassOperation = MetalMapStencilOp(desc.BackFace.StencilPassOp);
            back.readMask = desc.StencilReadMask;
            back.writeMask = desc.StencilWriteMask;
            dss.backFaceStencil = back;
        }

        id<MTLDepthStencilState> native = [mImpl->device newDepthStencilStateWithDescriptor:dss];
        metalState->setNativeObject((__bridge void *)native);
        metalState->setStencilRef(desc.StencilRef);
        return metalState;
    }

    //--------------------------------------------------------------------------

    RHIRasterizerStatePtr MetalContext::createRasterizerState(RasterizerState *state)
    {
        MetalRasterizerStatePtr metalState = MetalRasterizerState::create();
        if (state != nullptr)
        {
            metalState->setDesc(state->getStateDesc());
        }
        return metalState;
    }

    //--------------------------------------------------------------------------

    RHISamplerStatePtr MetalContext::createSamplerState(SamplerState *state)
    {
        MetalSamplerStatePtr metalState = MetalSamplerState::create();
        if (state == nullptr || mImpl->device == nil)
        {
            return metalState;
        }

        const SamplerDesc &desc = state->getStateDesc();
        MTLSamplerDescriptor *sd = [[MTLSamplerDescriptor alloc] init];
        sd.minFilter = MetalMapFilter(desc.MinFilter);
        sd.magFilter = MetalMapFilter(desc.MagFilter);
        sd.mipFilter = MetalMapMipFilter(desc.MipFilter);
        sd.sAddressMode = MetalMapAddress(desc.AddressU);
        sd.tAddressMode = MetalMapAddress(desc.AddressV);
        sd.rAddressMode = MetalMapAddress(desc.AddressW);
        sd.lodMinClamp = desc.MinLOD;
        sd.lodMaxClamp = desc.MaxLOD;
        sd.maxAnisotropy = std::max(1u, desc.MaxAnisotropy);
        sd.compareFunction = desc.IsComparison ? MetalMapCompare(desc.CompareFunc) : MTLCompareFunctionNever;

        id<MTLSamplerState> native = [mImpl->device newSamplerStateWithDescriptor:sd];
        metalState->setNativeObject((__bridge void *)native);
        return metalState;
    }

    //--------------------------------------------------------------------------

    TResult MetalContext::setBlendState(BlendState *state)
    {
        mImpl->blendState = (state != nullptr)
            ? smart_pointer_cast<MetalBlendState>(state->getRHIResource())
            : nullptr;
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult MetalContext::setDepthStencilState(DepthStencilState *state)
    {
        mImpl->depthState = (state != nullptr)
            ? smart_pointer_cast<MetalDepthStencilState>(state->getRHIResource())
            : nullptr;
        if (mImpl->encoder != nil)
        {
            applyEncoderState();
        }
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult MetalContext::setRasterizerState(RasterizerState *state)
    {
        mImpl->rasterState = (state != nullptr)
            ? smart_pointer_cast<MetalRasterizerState>(state->getRHIResource())
            : nullptr;
        if (mImpl->encoder != nil)
        {
            applyEncoderState();
        }
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    RHIVertexDeclarationPtr MetalContext::createVertexDeclaration(VertexDeclaration *decl)
    {
        MetalVertexDeclarationPtr metalDecl = MetalVertexDeclaration::create();
        if (decl != nullptr)
        {
            metalDecl->setAttributes(decl->getAttributes());
        }
        return metalDecl;
    }

    //--------------------------------------------------------------------------

    TResult MetalContext::setVertexDeclaration(VertexDeclaration *decl)
    {
        mImpl->vertexDecl = (decl != nullptr)
            ? smart_pointer_cast<MetalVertexDeclaration>(decl->getRHIResource())
            : nullptr;
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    RHIVertexBufferPtr MetalContext::createVertexBuffer(VertexBuffer *buffer)
    {
        MetalVertexBufferPtr metalBuffer = MetalVertexBuffer::create();
        if (buffer == nullptr)
        {
            return nullptr;
        }

        void *native = createMTLBuffer(buffer->getBufferSize(), buffer->getBuffer().Data);
        if (native == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_METALRENDERER, "Failed to create MTLBuffer for vertex buffer !");
            return nullptr;
        }
        metalBuffer->setNativeObject(native);
        return metalBuffer;
    }

    //--------------------------------------------------------------------------

    TResult MetalContext::setVertexBuffers(uint32_t startSlot, const VertexBuffers &buffers,
        const VertexStrides &strides, const VertexOffsets &offsets)
    {
        for (size_t i = 0; i < buffers.size(); ++i)
        {
            const uint32_t slot = startSlot + (uint32_t)i;
            if (slot >= 8 || buffers[i] == nullptr)
            {
                continue;
            }

            MetalVertexBufferPtr vb = smart_pointer_cast<MetalVertexBuffer>(buffers[i]->getRHIResource());
            mImpl->vb[slot].buffer = (vb != nullptr)
                ? (__bridge id<MTLBuffer>)vb->getNativeObject() : nil;
            mImpl->vb[slot].stride = (i < strides.size()) ? strides[i] : 0;
            mImpl->vb[slot].offset = (i < offsets.size()) ? offsets[i] : 0;

            if (mImpl->encoder != nil && mImpl->vb[slot].buffer != nil)
            {
                [mImpl->encoder setVertexBuffer:mImpl->vb[slot].buffer
                    offset:mImpl->vb[slot].offset atIndex:slot];
            }
        }
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    RHIIndexBufferPtr MetalContext::createIndexBuffer(IndexBuffer *buffer)
    {
        MetalIndexBufferPtr metalBuffer = MetalIndexBuffer::create();
        if (buffer == nullptr)
        {
            return nullptr;
        }

        void *native = createMTLBuffer(buffer->getBufferSize(), buffer->getBuffer().Data);
        if (native == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_METALRENDERER, "Failed to create MTLBuffer for index buffer !");
            return nullptr;
        }
        metalBuffer->setNativeObject(native);
        metalBuffer->setIndexType(buffer->getIndexType());
        return metalBuffer;
    }

    //--------------------------------------------------------------------------

    TResult MetalContext::setIndexBuffer(IndexBuffer *buffer)
    {
        mImpl->ib = (buffer != nullptr)
            ? smart_pointer_cast<MetalIndexBuffer>(buffer->getRHIResource())
            : nullptr;
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    RHIConstantBufferPtr MetalContext::createConstantBuffer(ConstantBuffer *buffer)
    {
        MetalConstantBufferPtr metalBuffer = MetalConstantBuffer::create();
        if (buffer == nullptr)
        {
            return nullptr;
        }

        void *native = createMTLBuffer(buffer->getBufferSize(), buffer->getBuffer().Data);
        if (native == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_METALRENDERER, "Failed to create MTLBuffer for constant buffer !");
            return nullptr;
        }
        metalBuffer->setNativeObject(native);
        return metalBuffer;
    }

    //--------------------------------------------------------------------------

    static void uploadTexture2D(id<MTLTexture> tex, const Buffer &buffer, PixelFormat format)
    {
        if (tex == nil || buffer.Data == nullptr || buffer.DataSize == 0)
        {
            return;
        }

        const uint32_t bpp = MetalBytesPerPixel(format);
        const NSUInteger bpr = (NSUInteger)tex.width * bpp;
        MTLRegion region = MTLRegionMake2D(0, 0, tex.width, tex.height);
        [tex replaceRegion:region mipmapLevel:0 withBytes:buffer.Data bytesPerRow:bpr];
    }

    RHIPixelBuffer1DPtr MetalContext::createPixelBuffer1D(PixelBuffer1D *buffer)
    {
        MetalPixelBuffer1DPtr metalBuffer = MetalPixelBuffer1D::create();
        if (buffer == nullptr || mImpl->device == nil)
        {
            return metalBuffer;
        }

        const PixelBuffer1DDesc &desc = buffer->getDescriptor();
        MTLTextureDescriptor *td = [MTLTextureDescriptor
            texture2DDescriptorWithPixelFormat:MetalMapPixelFormat(desc.format)
            width:std::max(1u, desc.width) height:1 mipmapped:desc.mipmaps > 1];
        td.textureType = MTLTextureType1D;
        td.usage = MTLTextureUsageShaderRead;
        id<MTLTexture> tex = [mImpl->device newTextureWithDescriptor:td];
        metalBuffer->setNativeObject((__bridge void *)tex);
        return metalBuffer;
    }

    //--------------------------------------------------------------------------

    RHIPixelBuffer2DPtr MetalContext::createPixelBuffer2D(PixelBuffer2D *buffer)
    {
        MetalPixelBuffer2DPtr metalBuffer = MetalPixelBuffer2D::create();
        if (buffer == nullptr || mImpl->device == nil)
        {
            return metalBuffer;
        }

        const PixelBuffer2DDesc &desc = buffer->getDescriptor();
        MTLTextureDescriptor *td = [MTLTextureDescriptor
            texture2DDescriptorWithPixelFormat:MetalMapPixelFormat(desc.format)
            width:std::max(1u, desc.width) height:std::max(1u, desc.height)
            mipmapped:desc.mipmaps > 1];
        td.usage = MTLTextureUsageShaderRead;
        td.storageMode = MTLStorageModeShared;
        if (IsDepthFormat(desc.format))
        {
            td.usage = MTLTextureUsageRenderTarget | MTLTextureUsageShaderRead;
            td.storageMode = MTLStorageModePrivate;
        }
        id<MTLTexture> tex = [mImpl->device newTextureWithDescriptor:td];
        if (tex == nil)
        {
            T3D_LOG_ERROR(LOG_TAG_METALRENDERER, "Failed to create MTLTexture2D !");
            return nullptr;
        }
        if (td.storageMode != MTLStorageModePrivate)
        {
            uploadTexture2D(tex, buffer->getBuffer(), desc.format);
        }
        metalBuffer->setNativeObject((__bridge void *)tex);
        return metalBuffer;
    }

    //--------------------------------------------------------------------------

    RHIPixelBuffer3DPtr MetalContext::createPixelBuffer3D(PixelBuffer3D *buffer)
    {
        MetalPixelBuffer3DPtr metalBuffer = MetalPixelBuffer3D::create();
        if (buffer == nullptr || mImpl->device == nil)
        {
            return metalBuffer;
        }

        const PixelBuffer3DDesc &desc = buffer->getDescriptor();
        MTLTextureDescriptor *td = [MTLTextureDescriptor
            texture2DDescriptorWithPixelFormat:MetalMapPixelFormat(desc.format)
            width:std::max(1u, desc.width) height:std::max(1u, desc.height) mipmapped:NO];
        td.textureType = MTLTextureType3D;
        td.depth = std::max(1u, desc.depth);
        td.usage = MTLTextureUsageShaderRead;
        id<MTLTexture> tex = [mImpl->device newTextureWithDescriptor:td];
        metalBuffer->setNativeObject((__bridge void *)tex);
        return metalBuffer;
    }

    //--------------------------------------------------------------------------

    RHIPixelBufferCubemapPtr MetalContext::createPixelBufferCubemap(PixelBufferCubemap *buffer)
    {
        MetalPixelBufferCubemapPtr metalBuffer = MetalPixelBufferCubemap::create();
        if (buffer == nullptr || mImpl->device == nil)
        {
            return metalBuffer;
        }

        const PixelBuffer2DDesc &desc = buffer->getDescriptor();
        MTLTextureDescriptor *td = [MTLTextureDescriptor
            textureCubeDescriptorWithPixelFormat:MetalMapPixelFormat(desc.format)
            size:std::max(1u, desc.width) mipmapped:desc.mipmaps > 1];
        td.usage = MTLTextureUsageShaderRead;
        id<MTLTexture> tex = [mImpl->device newTextureWithDescriptor:td];
        metalBuffer->setNativeObject((__bridge void *)tex);
        return metalBuffer;
    }

    //--------------------------------------------------------------------------

    RHIShaderPtr MetalContext::createVertexShader(ShaderVariant *shader)
    {
        MetalVertexShaderPtr metalShader = MetalVertexShader::create();
        void *fn = compileMSLFunction(shader);
        if (fn == nullptr)
        {
            return nullptr;
        }
        metalShader->setNativeObject(fn);
        return metalShader;
    }

    //--------------------------------------------------------------------------

    TResult MetalContext::setVertexShader(ShaderVariant *shader)
    {
        mImpl->vs = (shader != nullptr)
            ? static_cast<MetalVertexShader *>(shader->getRHIShader())
            : nullptr;
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    static void bindConstantBuffers(id<MTLRenderCommandEncoder> encoder, bool vertex,
        uint32_t startSlot, const ConstantBuffers &buffers)
    {
        if (encoder == nil)
        {
            return;
        }

        for (size_t i = 0; i < buffers.size(); ++i)
        {
            if (buffers[i] == nullptr)
            {
                continue;
            }
            void *native = buffers[i]->getRHIResource() != nullptr
                ? buffers[i]->getRHIResource()->getNativeObject() : nullptr;
            if (native == nullptr)
            {
                continue;
            }
            id<MTLBuffer> buf = (__bridge id<MTLBuffer>)native;
            const uint32_t index = startSlot + (uint32_t)i;
            if (vertex)
            {
                [encoder setVertexBuffer:buf offset:0 atIndex:index];
            }
            else
            {
                [encoder setFragmentBuffer:buf offset:0 atIndex:index];
            }
        }
    }

    static void bindTextures(id<MTLRenderCommandEncoder> encoder, bool vertex,
        uint32_t startSlot, const PixelBuffers &buffers)
    {
        if (encoder == nil)
        {
            return;
        }

        for (size_t i = 0; i < buffers.size(); ++i)
        {
            if (buffers[i] == nullptr || buffers[i]->getRHIResource() == nullptr)
            {
                continue;
            }
            id<MTLTexture> tex = (__bridge id<MTLTexture>)buffers[i]->getRHIResource()->getNativeObject();
            const uint32_t index = startSlot + (uint32_t)i;
            if (vertex)
            {
                [encoder setVertexTexture:tex atIndex:index];
            }
            else
            {
                [encoder setFragmentTexture:tex atIndex:index];
            }
        }
    }

    static void bindSamplers(id<MTLRenderCommandEncoder> encoder, bool vertex,
        uint32_t startSlot, const Samplers &samplers)
    {
        if (encoder == nil)
        {
            return;
        }

        for (size_t i = 0; i < samplers.size(); ++i)
        {
            if (samplers[i] == nullptr || samplers[i]->getRHIResource() == nullptr)
            {
                continue;
            }
            id<MTLSamplerState> s = (__bridge id<MTLSamplerState>)samplers[i]->getRHIResource()->getNativeObject();
            const uint32_t index = startSlot + (uint32_t)i;
            if (vertex)
            {
                [encoder setVertexSamplerState:s atIndex:index];
            }
            else
            {
                [encoder setFragmentSamplerState:s atIndex:index];
            }
        }
    }

    TResult MetalContext::setVSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers)
    {
        bindConstantBuffers(mImpl->encoder, true, startSlot, buffers);
        return T3D_OK;
    }

    TResult MetalContext::setVSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers)
    {
        bindTextures(mImpl->encoder, true, startSlot, buffers);
        return T3D_OK;
    }

    TResult MetalContext::setVSSamplers(uint32_t startSlot, const Samplers &samplers)
    {
        bindSamplers(mImpl->encoder, true, startSlot, samplers);
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    RHIShaderPtr MetalContext::createPixelShader(ShaderVariant *shader)
    {
        MetalPixelShaderPtr metalShader = MetalPixelShader::create();
        void *fn = compileMSLFunction(shader);
        if (fn == nullptr)
        {
            return nullptr;
        }
        metalShader->setNativeObject(fn);
        return metalShader;
    }

    TResult MetalContext::setPixelShader(ShaderVariant *shader)
    {
        mImpl->ps = (shader != nullptr)
            ? static_cast<MetalPixelShader *>(shader->getRHIShader())
            : nullptr;
        return T3D_OK;
    }

    TResult MetalContext::setPSConstantBuffers(uint32_t startSlot, const ConstantBuffers &buffers)
    {
        bindConstantBuffers(mImpl->encoder, false, startSlot, buffers);
        return T3D_OK;
    }

    TResult MetalContext::setPSPixelBuffers(uint32_t startSlot, const PixelBuffers &buffers)
    {
        bindTextures(mImpl->encoder, false, startSlot, buffers);
        return T3D_OK;
    }

    TResult MetalContext::setPSSamplers(uint32_t startSlot, const Samplers &samplers)
    {
        bindSamplers(mImpl->encoder, false, startSlot, samplers);
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    RHIShaderPtr MetalContext::createHullShader(ShaderVariant *) { return MetalHullShader::create(); }
    TResult MetalContext::setHullShader(ShaderVariant *) { return T3D_OK; }
    TResult MetalContext::setHSConstantBuffers(uint32_t, const ConstantBuffers &) { return T3D_OK; }
    TResult MetalContext::setHSPixelBuffers(uint32_t, const PixelBuffers &) { return T3D_OK; }
    TResult MetalContext::setHSSamplers(uint32_t, const Samplers &) { return T3D_OK; }

    RHIShaderPtr MetalContext::createDomainShader(ShaderVariant *) { return MetalDomainShader::create(); }
    TResult MetalContext::setDomainShader(ShaderVariant *) { return T3D_OK; }
    TResult MetalContext::setDSConstantBuffers(uint32_t, const ConstantBuffers &) { return T3D_OK; }
    TResult MetalContext::setDSPixelBuffers(uint32_t, const PixelBuffers &) { return T3D_OK; }
    TResult MetalContext::setDSSamplers(uint32_t, const Samplers &) { return T3D_OK; }

    RHIShaderPtr MetalContext::createGeometryShader(ShaderVariant *) { return MetalGeometryShader::create(); }
    TResult MetalContext::setGeometryShader(ShaderVariant *) { return T3D_OK; }
    TResult MetalContext::setGSConstantBuffers(uint32_t, const ConstantBuffers &) { return T3D_OK; }
    TResult MetalContext::setGSPixelBuffers(uint32_t, const PixelBuffers &) { return T3D_OK; }
    TResult MetalContext::setGSSamplers(uint32_t, const Samplers &) { return T3D_OK; }

    RHIShaderPtr MetalContext::createComputeShader(ShaderVariant *shader)
    {
        MetalComputeShaderPtr metalShader = MetalComputeShader::create();
        void *fn = compileMSLFunction(shader);
        if (fn != nullptr)
        {
            metalShader->setNativeObject(fn);
        }
        return metalShader;
    }

    // 这四个接口曾经返回 T3D_OK 但没有任何行为，调用方拿到「成功」却什么都没发生。
    // Metal 的 compute encoder 链路尚未接入，改为显式声明不支持，避免继续假成功。
    TResult MetalContext::setComputeShader(ShaderVariant *) { T3D_RHI_UNSUPPORTED(supportsCompute); }
    TResult MetalContext::setCSConstantBuffers(uint32_t, const ConstantBuffers &) { T3D_RHI_UNSUPPORTED(supportsCompute); }
    TResult MetalContext::setCSPixelBuffers(uint32_t, const PixelBuffers &) { T3D_RHI_UNSUPPORTED(supportsCompute); }
    TResult MetalContext::setCSSamplers(uint32_t, const Samplers &) { T3D_RHI_UNSUPPORTED(supportsCompute); }

    RHIStructuredBufferPtr MetalContext::createStructuredBuffer(StructuredBuffer *) { T3D_RHI_UNSUPPORTED_PTR(supportsStructuredBuffer); }
    TResult MetalContext::setVSStructuredBuffers(uint32_t, const StructuredBuffers &) { T3D_RHI_UNSUPPORTED(supportsStructuredBuffer); }
    TResult MetalContext::setPSStructuredBuffers(uint32_t, const StructuredBuffers &) { T3D_RHI_UNSUPPORTED(supportsStructuredBuffer); }
    TResult MetalContext::setCSStructuredBuffers(uint32_t, const StructuredBuffers &) { T3D_RHI_UNSUPPORTED(supportsStructuredBuffer); }
    TResult MetalContext::setCSUnorderedAccessBuffers(uint32_t, const UnorderedAccessBuffers &, const UAVInitialCounts &) { T3D_RHI_UNSUPPORTED(supportsUnorderedAccess); }
    TResult MetalContext::dispatch(uint32_t, uint32_t, uint32_t) { T3D_RHI_UNSUPPORTED(supportsCompute); }
    TResult MetalContext::dispatchIndirect(RenderBuffer *, size_t) { T3D_RHI_UNSUPPORTED(supportsIndirectDispatch); }
    TResult MetalContext::uavBarrier(const UnorderedAccessBuffers &) { T3D_RHI_UNSUPPORTED(supportsUnorderedAccess); }
    TResult MetalContext::copyStructureCount(RenderBuffer *, size_t, RenderBuffer *) { T3D_RHI_UNSUPPORTED(supportsAppendConsumeBuffer); }
    TResult MetalContext::renderIndexedIndirect(RenderBuffer *, size_t) { T3D_RHI_UNSUPPORTED(supportsIndirectDraw); }
    TResult MetalContext::renderIndirect(RenderBuffer *, size_t) { T3D_RHI_UNSUPPORTED(supportsIndirectDraw); }

    //--------------------------------------------------------------------------

    TResult MetalContext::compileShader(ShaderVariant *shader)
    {
        void *fn = compileMSLFunction(shader);
        if (fn == nullptr)
        {
            return T3D_ERR_METAL_SHADER;
        }

        size_t length = 0;
        const char *code = shader->getBytesCode(length);
        if (code != nullptr && !shader->hasCompiled())
        {
            shader->setBytesCode(code, length);
        }
        return T3D_OK;
    }

    TResult MetalContext::reflectShaderAllBindings(ShaderVariant *, ShaderConstantParams &, ShaderSamplerParams &)
    {
        return T3D_OK;
    }

    TResult MetalContext::reflectSamplerBindings(ShaderVariant *, ShaderSamplerParams &)
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult MetalContext::setPrimitiveType(PrimitiveType primitive)
    {
        mImpl->primitive = primitive;
        return T3D_OK;
    }

    TResult MetalContext::render(uint32_t indexCount, uint32_t startIndex, uint32_t baseVertex)
    {
        return renderIndexedInstanced(indexCount, 1, startIndex, (int32_t)baseVertex, 0);
    }

    TResult MetalContext::render(uint32_t vertexCount, uint32_t startVertex)
    {
        return renderInstanced(vertexCount, 1, startVertex, 0);
    }

    TResult MetalContext::renderIndexedInstanced(uint32_t indexCount, uint32_t instanceCount,
        uint32_t startIndex, int32_t baseVertex, uint32_t startInstance)
    {
        TResult ret = ensureEncoder();
        if (T3D_FAILED(ret))
        {
            return ret;
        }

        ret = bindCurrentPipeline();
        if (T3D_FAILED(ret))
        {
            return ret;
        }

        if (mImpl->ib == nullptr || mImpl->ib->getNativeObject() == nullptr)
        {
            return T3D_ERR_INVALID_POINTER;
        }

        id<MTLBuffer> indexBuffer = (__bridge id<MTLBuffer>)mImpl->ib->getNativeObject();
        const MTLIndexType indexType = (mImpl->ib->getIndexType() == IndexType::E_IT_32BITS)
            ? MTLIndexTypeUInt32 : MTLIndexTypeUInt16;
        const NSUInteger indexSize = (indexType == MTLIndexTypeUInt32) ? 4 : 2;

        [mImpl->encoder drawIndexedPrimitives:MetalMapPrimitive(mImpl->primitive)
            indexCount:indexCount
            indexType:indexType
            indexBuffer:indexBuffer
            indexBufferOffset:(NSUInteger)startIndex * indexSize
            instanceCount:instanceCount
            baseVertex:baseVertex
            baseInstance:startInstance];
        return T3D_OK;
    }

    TResult MetalContext::renderInstanced(uint32_t vertexCount, uint32_t instanceCount,
        uint32_t startVertex, uint32_t startInstance)
    {
        TResult ret = ensureEncoder();
        if (T3D_FAILED(ret))
        {
            return ret;
        }

        ret = bindCurrentPipeline();
        if (T3D_FAILED(ret))
        {
            return ret;
        }

        [mImpl->encoder drawPrimitives:MetalMapPrimitive(mImpl->primitive)
            vertexStart:startVertex
            vertexCount:vertexCount
            instanceCount:instanceCount
            baseInstance:startInstance];
        return T3D_OK;
    }

    TResult MetalContext::reset()
    {
        mImpl->blendState = nullptr;
        mImpl->rasterState = nullptr;
        mImpl->depthState = nullptr;
        mImpl->vertexDecl = nullptr;
        mImpl->vs = nullptr;
        mImpl->ps = nullptr;
        mImpl->ib = nullptr;
        for (auto &slot : mImpl->vb)
        {
            slot = VBSlot();
        }
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult MetalContext::blit(RenderTarget *src, RenderTarget *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)
    {
        if (src == nullptr || dst == nullptr)
        {
            return T3D_ERR_INVALID_POINTER;
        }

        Texture *srcTex = nullptr;
        if (src->getNumOfRenderTextures() > 0)
        {
            srcTex = src->getRenderTexture();
        }
        if (srcTex == nullptr)
        {
            return T3D_ERR_INVALID_PARAM;
        }
        return blit(srcTex, dst, srcOffset, size, dstOffset);
    }

    TResult MetalContext::blit(Texture *src, RenderTarget *dst, const Vector3 &srcOffset, const Vector3 &size, const Vector3 dstOffset)
    {
        if (src == nullptr || dst == nullptr || mImpl->cmd == nil)
        {
            return T3D_ERR_INVALID_POINTER;
        }

        // blit 前必须结束当前 render encoder，否则源纹理仍被绑定为 color attachment
        if (mImpl->pendingColorClear || mImpl->pendingDepthClear || mImpl->pendingStencilClear)
        {
            TResult ret = ensureEncoder();
            if (T3D_FAILED(ret))
            {
                return ret;
            }
        }
        endEncoder();

        id<MTLTexture> srcTex = GetBlitSourceTexture(src);
        id<MTLTexture> dstTex = nil;

        if (dst->getType() == RenderTarget::Type::E_RT_WINDOW)
        {
            if (mImpl->window == nullptr)
            {
                return T3D_ERR_INVALID_POINTER;
            }
            id<CAMetalDrawable> drawable = (__bridge id<CAMetalDrawable>)mImpl->window->getDrawable();
            if (drawable == nil)
            {
                drawable = (__bridge id<CAMetalDrawable>)mImpl->window->acquireDrawable();
            }
            if (drawable == nil)
            {
                return T3D_ERR_METAL_DRAWABLE;
            }
            dstTex = drawable.texture;
        }
        else if (dst->getNumOfRenderTextures() > 0)
        {
            dstTex = GetBlitSourceTexture(dst->getRenderTexture());
        }

        if (srcTex == nil || dstTex == nil)
        {
            T3D_LOG_ERROR(LOG_TAG_METALRENDERER, "blit: source or destination texture is nil !");
            return T3D_ERR_METAL_TEXTURE;
        }

        NSUInteger srcX = 0;
        NSUInteger srcY = 0;
        NSUInteger dstX = 0;
        NSUInteger dstY = 0;
        NSUInteger w = 0;
        NSUInteger h = 0;
        if (size == Vector3::ZERO)
        {
            w = std::min(srcTex.width, dstTex.width);
            h = std::min(srcTex.height, dstTex.height);
        }
        else
        {
            srcX = (NSUInteger)std::max(0.0f, srcOffset.x());
            srcY = (NSUInteger)std::max(0.0f, srcOffset.y());
            dstX = (NSUInteger)std::max(0.0f, dstOffset.x());
            dstY = (NSUInteger)std::max(0.0f, dstOffset.y());
            w = (NSUInteger)std::max(0.0f, size.x());
            h = (NSUInteger)std::max(0.0f, size.y());
        }

        if (srcX >= srcTex.width || srcY >= srcTex.height
            || dstX >= dstTex.width || dstY >= dstTex.height)
        {
            return T3D_OK;
        }

        w = std::min(w, srcTex.width - srcX);
        h = std::min(h, srcTex.height - srcY);
        w = std::min(w, dstTex.width - dstX);
        h = std::min(h, dstTex.height - dstY);
        if (w == 0 || h == 0)
        {
            return T3D_OK;
        }

        id<MTLBlitCommandEncoder> blitEnc = [mImpl->cmd blitCommandEncoder];
        if (blitEnc == nil)
        {
            return T3D_ERR_METAL_DEVICE;
        }

        [blitEnc copyFromTexture:srcTex
            sourceSlice:0
            sourceLevel:0
            sourceOrigin:MTLOriginMake(srcX, srcY, 0)
            sourceSize:MTLSizeMake(w, h, 1)
            toTexture:dstTex
            destinationSlice:0
            destinationLevel:0
            destinationOrigin:MTLOriginMake(dstX, dstY, 0)];
        [blitEnc endEncoding];
        return T3D_OK;
    }

    TResult MetalContext::blit(RenderTarget *, Texture *, const Vector3 &, const Vector3 &, const Vector3)
    {
        return T3D_OK;
    }

    TResult MetalContext::blit(Texture *, Texture *, const Vector3 &, const Vector3 &, const Vector3)
    {
        return T3D_OK;
    }

    TResult MetalContext::copyBuffer(RenderBuffer *src, RenderBuffer *dst, size_t srcOffset, size_t size, size_t dstOffset)
    {
        if (src == nullptr || dst == nullptr
            || src->getRHIResource() == nullptr || dst->getRHIResource() == nullptr)
        {
            return T3D_ERR_INVALID_POINTER;
        }

        id<MTLBuffer> srcBuf = (__bridge id<MTLBuffer>)src->getRHIResource()->getNativeObject();
        id<MTLBuffer> dstBuf = (__bridge id<MTLBuffer>)dst->getRHIResource()->getNativeObject();
        if (srcBuf == nil || dstBuf == nil)
        {
            return T3D_ERR_METAL_BUFFER;
        }

        const size_t copySize = (size == 0) ? (srcBuf.length - srcOffset) : size;
        if (srcOffset + copySize > srcBuf.length || dstOffset + copySize > dstBuf.length)
        {
            return T3D_ERR_INVALID_PARAM;
        }

        memcpy((uint8_t *)[dstBuf contents] + dstOffset,
            (const uint8_t *)[srcBuf contents] + srcOffset, copySize);
        return T3D_OK;
    }

    ReadbackHandle MetalContext::map(RenderBuffer *src, size_t offset, size_t size)
    {
        T3D_RHI_UNSUPPORTED_VALUE(supportsReadback, ReadbackHandle::invalid());
    }

    ReadbackHandle MetalContext::map(RenderBuffer *src, const ReadbackRegion &region)
    {
        T3D_RHI_UNSUPPORTED_VALUE(supportsReadback, ReadbackHandle::invalid());
    }

    TResult MetalContext::unmap(ReadbackHandle handle, Buffer &dst)
    {
        T3D_RHI_UNSUPPORTED(supportsReadback);
    }

    TResult MetalContext::writeBuffer(RenderBuffer *renderBuffer, const Buffer &buffer, bool)
    {
        if (renderBuffer == nullptr || renderBuffer->getRHIResource() == nullptr || buffer.Data == nullptr)
        {
            return T3D_ERR_INVALID_POINTER;
        }

        void *native = renderBuffer->getRHIResource()->getNativeObject();
        if (native == nullptr)
        {
            return T3D_ERR_METAL_BUFFER;
        }

        const RenderResource::Type type = renderBuffer->getType();
        if (type == RenderResource::Type::kPixelBuffer2D)
        {
            id<MTLTexture> tex = (__bridge id<MTLTexture>)native;
            PixelBuffer2D *pb = static_cast<PixelBuffer2D *>(renderBuffer);
            uploadTexture2D(tex, buffer, pb->getDescriptor().format);
            return T3D_OK;
        }

        id<MTLBuffer> mtlBuffer = (__bridge id<MTLBuffer>)native;
        const size_t copySize = std::min((size_t)mtlBuffer.length, buffer.DataSize);
        memcpy([mtlBuffer contents], buffer.Data, copySize);
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult MetalContext::beginRender()
    {
        if (mImpl->window == nullptr || mImpl->queue == nil)
        {
            return T3D_ERR_INVALID_POINTER;
        }

        if (mImpl->window->acquireDrawable() == nullptr)
        {
            T3D_LOG_ERROR(LOG_TAG_METALRENDERER, "Failed to acquire CAMetalDrawable !");
            return T3D_ERR_METAL_DRAWABLE;
        }

        mImpl->cmd = [mImpl->queue commandBuffer];
        mImpl->encoder = nil;
        mImpl->pendingColorClear = false;
        mImpl->pendingDepthClear = false;
        mImpl->pendingStencilClear = false;
        return T3D_OK;
    }

    TResult MetalContext::endRender()
    {
        endEncoder();

        if (mImpl->cmd == nil)
        {
            return T3D_ERR_INVALID_POINTER;
        }

        id<CAMetalDrawable> drawable = (mImpl->window != nullptr)
            ? (__bridge id<CAMetalDrawable>)mImpl->window->getDrawable() : nil;
        if (drawable != nil)
        {
            [mImpl->cmd presentDrawable:drawable];
        }

        [mImpl->cmd commit];
        mImpl->cmd = nil;
        if (mImpl->window != nullptr)
        {
            mImpl->window->releaseDrawable();
        }
        return T3D_OK;
    }

    TResult MetalContext::beginPass()
    {
        return ensureEncoder();
    }

    TResult MetalContext::endPass()
    {
        endEncoder();
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
}
