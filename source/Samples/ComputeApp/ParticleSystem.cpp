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

#include "ParticleSystem.h"
#include "ComputeShaderSources.h"
#include "ComputeCases.h"
#include "../Common/SampleApp.h"

#include <cmath>
#include <cstring>
#include <cstddef>


using namespace Tiny3D;


namespace
{
    float frac(float x)
    {
        return x - floorf(x);
    }

    bool pickSource(const ComputeShaderSource &source, const char *&code, size_t &size)
    {
        RHIRendererPtr renderer = T3D_AGENT.getActiveRHIRenderer();
        if (renderer == nullptr)
        {
            return false;
        }

        const String &rendererName = renderer->getName();

#if defined(T3D_OS_WINDOWS)
        if (rendererName == RHIRenderer::OPENGL4)
        {
            code = source.gl;
            size = (source.gl != nullptr) ? strlen(source.gl) : 0;
        }
        else if (rendererName == RHIRenderer::VULKAN)
        {
            code = reinterpret_cast<const char *>(source.vk);
            size = source.vkSize;
        }
        else
        {
            code = source.hlsl;
            size = (source.hlsl != nullptr) ? strlen(source.hlsl) : 0;
        }
#elif defined(T3D_OS_ANDROID)
        code = source.gles;
        size = (source.gles != nullptr) ? strlen(source.gles) : 0;
#else
        (void)rendererName;
        code = nullptr;
        size = 0;
#endif
        return (code != nullptr && size > 0);
    }

    Buffer wrapBytes(void *data, size_t bytes)
    {
        Buffer buf;
        buf.Data = static_cast<uint8_t *>(data);
        buf.DataSize = bytes;
        return buf;
    }

    Buffer allocBytes(size_t bytes)
    {
        Buffer buf;
        buf.DataSize = bytes;
        buf.Data = T3D_POD_NEW_ARRAY(uint8_t, bytes);
        memset(buf.Data, 0, bytes);
        return buf;
    }
}


bool ParticleSystem::isGLFamily() const
{
    const uint32_t bit = currentBackendBit();
    return (bit & (kBackendGL4 | kBackendGLES3)) != 0;
}

uint32_t ParticleSystem::uavSlot(uint32_t srvCount, uint32_t uRegister) const
{
    return isGLFamily() ? (srvCount + uRegister) : uRegister;
}

void ParticleSystem::unbindComputeUAVs(RHIContext *ctx, uint32_t startSlot, uint32_t count)
{
    // uavBarrier 只是解绑再绑回来刷缓存，槽位仍然占着资源。D3D11 不允许同一资源
    // 同时做 UAV 和 SRV，不显式清空槽位的话，VSSetShaderResources 会把 SRV 强制
    // 置 NULL（DEVICE_VSSETSHADERRESOURCES_HAZARD），顶点拉流读到全 0，粒子全部退化。
    UnorderedAccessBuffers empty(count, nullptr);
    ctx->setCSUnorderedAccessBuffers(startSlot, empty);
}

void ParticleSystem::unbindVSStructuredBuffers(RHIContext *ctx, uint32_t startSlot, uint32_t count)
{
    // 画完不解绑，下一帧 dispatch 把同一块缓冲当 UAV 绑回去时，D3D11 会报
    // CSSETUNORDEREDACCESSVIEWS_HAZARD 再把 VS 的 SRV 强制置空
    StructuredBuffers empty(count, nullptr);
    ctx->setVSStructuredBuffers(startSlot, empty);
}

void ParticleSystem::unbindCSStructuredBuffers(RHIContext *ctx, uint32_t startSlot, uint32_t count)
{
    // 同理：剔除内核把粒子缓冲当 CS SRV 读过，下一帧更新内核要拿它当 UAV
    StructuredBuffers empty(count, nullptr);
    ctx->setCSStructuredBuffers(startSlot, empty);
}

ShaderVariantPtr ParticleSystem::compileGraphics(const char *name, SHADER_STAGE stage,
    const ComputeShaderSource &source)
{
    const char *code = nullptr;
    size_t codeSize = 0;
    if (!pickSource(source, code, codeSize))
    {
        APP_LOG_DEBUG("[particles] no embedded %s source on this backend !", name);
        return nullptr;
    }

    RHIRendererPtr renderer = T3D_AGENT.getActiveRHIRenderer();

    ShaderKeyword keyword;
    keyword.addKeyword("");
    keyword.generate();

    ShaderVariantPtr variant = ShaderVariant::create(std::move(keyword), code, codeSize);
    if (variant == nullptr)
    {
        return nullptr;
    }

    variant->setShaderStage(stage);
    variant->setLanguage(renderer->getShadingLanguage());

    Tiny3D::ShaderPtr dummy = ComputeKernel::attachDummyPass(variant.get(), name);
    if (dummy == nullptr)
    {
        APP_LOG_DEBUG("[particles] failed to attach dummy pass for [%s] !", name);
        return nullptr;
    }
    mDummyShaders.push_back(dummy);

    const TResult ret = variant->compile();
    if (T3D_FAILED(ret))
    {
        APP_LOG_DEBUG("[particles] failed to compile [%s] ! ERROR [%d]", name, ret);
        return nullptr;
    }

    return variant;
}

TResult ParticleSystem::setup(Camera *camera)
{
    mCamera = camera;
    if (mCamera == nullptr)
    {
        return T3D_ERR_INVALID_POINTER;
    }

    mCpuParticles.resize(kParticleCount);
    for (uint32_t i = 0; i < kParticleCount; ++i)
    {
        Particle &p = mCpuParticles[i];
        p.position[0] = frac(i * 0.0123f) * 8.0f - 4.0f;
        p.position[1] = frac(i * 0.0456f) * 8.0f - 4.0f;
        p.position[2] = frac(i * 0.0789f) * 8.0f - 4.0f;
        p.velocity[0] = 0.0f;
        p.velocity[1] = 0.0f;
        p.velocity[2] = 0.0f;
        p.life = 4.0f + frac(i * 0.017f) * 4.0f;
        p.size = 0.035f;
    }

    Buffer init = allocBytes(sizeof(Particle) * kParticleCount);
    memcpy(init.Data, mCpuParticles.data(), init.DataSize);

    StructuredBufferDesc particleDesc;
    particleDesc.kind = StructuredBufferKind::kStructured;
    particleDesc.elementSize = sizeof(Particle);
    particleDesc.elementCount = kParticleCount;
    particleDesc.buffer = init;

    mParticles = T3D_RENDER_BUFFER_MGR.loadStructuredBuffer(
        particleDesc, MemoryType::kBoth, Usage::kStatic, kCPUNone,
        kGPUUnorderedAccess | kGPUShaderResource);
    init.release();

    if (mParticles == nullptr)
    {
        APP_LOG_DEBUG("[particles] failed to create particle buffer !");
        return T3D_ERR_FAIL;
    }

    mUpdateKernel = ComputeKernel::create("ComputeParticleUpdate", particleUpdateSource(), 256);
    mCullKernel = ComputeKernel::create("ComputeParticleCull", particleCullSource(), 256);
    mDrawVS = compileGraphics("ParticleDrawVS", SHADER_STAGE::kVertex, particleDrawVSSource());
    mDrawCpuVS = compileGraphics("ParticleDrawCpuVS", SHADER_STAGE::kVertex, particleDrawCpuVSSource());
    mDrawPS = compileGraphics("ParticleDrawPS", SHADER_STAGE::kPixel, particleDrawPSSource());

    if (mDrawPS == nullptr || (mDrawVS == nullptr && mDrawCpuVS == nullptr))
    {
        APP_LOG_DEBUG("[particles] draw shaders unavailable, visual track disabled.");
        return T3D_ERR_FAIL;
    }

    // GPU 路径的 VS 只吃 SV_VertexID / SV_InstanceID，一个顶点属性都不读，本该绑空
    // input layout。但空 VertexDeclaration 建不出来（CreateInputLayout 收到 0 个元素
    // 直接 E_INVALIDARG），而 setVertexDeclaration 又不收 nullptr，于是绘制时留在 IA 上
    // 的是别人（管线 blit 或 CPU 路径）按**另一个 VS** 建的 layout。D3D11 把 input layout
    // 的签名当作 IA 的输出签名来校验，SV_VertexID 的寄存器号对不上就是
    // DEVICE_SHADER_LINKAGE_REGISTERINDEX，间接绘制会被整个丢弃。
    // 所以这里按 mDrawVS 建一个只有占位属性的 layout，签名天然匹配。
    if (mDrawVS != nullptr)
    {
        VertexAttributes gpuAttribs;
        gpuAttribs.push_back(VertexAttribute(0, 0, VertexAttribute::Type::E_VAT_FLOAT3,
            VertexAttribute::Semantic::E_VAS_POSITION, 0));
        mGpuDecl = T3D_RENDER_BUFFER_MGR.addVertexDeclaration(gpuAttribs, mDrawVS.get());
        if (mGpuDecl == nullptr)
        {
            APP_LOG_DEBUG("[particles] failed to create placeholder input layout for GPU draw !");
        }
    }

    if (mDrawCpuVS != nullptr)
    {
        VertexAttributes cpuAttribs;
        cpuAttribs.push_back(VertexAttribute(0, 0, VertexAttribute::Type::E_VAT_FLOAT3,
            VertexAttribute::Semantic::E_VAS_POSITION, 0,
            VertexAttribute::InputRate::kPerInstance, 1));
        cpuAttribs.push_back(VertexAttribute(0, 12, VertexAttribute::Type::E_VAT_FLOAT1,
            VertexAttribute::Semantic::E_VAS_TEXCOORD, 0,
            VertexAttribute::InputRate::kPerInstance, 1));
        cpuAttribs.push_back(VertexAttribute(0, 16, VertexAttribute::Type::E_VAT_FLOAT3,
            VertexAttribute::Semantic::E_VAS_TEXCOORD, 1,
            VertexAttribute::InputRate::kPerInstance, 1));
        cpuAttribs.push_back(VertexAttribute(0, 28, VertexAttribute::Type::E_VAT_FLOAT1,
            VertexAttribute::Semantic::E_VAS_TEXCOORD, 2,
            VertexAttribute::InputRate::kPerInstance, 1));
        mCpuDecl = T3D_RENDER_BUFFER_MGR.addVertexDeclaration(cpuAttribs, mDrawCpuVS.get());

        Buffer vbInit = allocBytes(sizeof(Particle) * kParticleCount);
        memcpy(vbInit.Data, mCpuParticles.data(), vbInit.DataSize);
        mCpuVB = T3D_RENDER_BUFFER_MGR.loadVertexBuffer(
            sizeof(Particle), kParticleCount, vbInit,
            MemoryType::kBoth, Usage::kDynamic, kCPUWrite);
        vbInit.release();
    }

    uint16_t indices[6] = { 0, 1, 2, 3, 4, 5 };
    Buffer ib = wrapBytes(indices, sizeof(indices));
    mIndexBuffer = T3D_RENDER_BUFFER_MGR.loadIndexBuffer(
        IndexType::E_IT_16BITS, 6, ib, MemoryType::kBoth, Usage::kImmutable, kCPUNone);
    ib.Data = nullptr;

    DrawIndexedIndirectArgs args;
    args.indexCountPerInstance = 6;
    args.instanceCount = kParticleCount;
    args.startIndexLocation = 0;
    args.baseVertexLocation = 0;
    args.startInstanceLocation = 0;

    Buffer argsInit = allocBytes(sizeof(args));
    memcpy(argsInit.Data, &args, sizeof(args));
    // 间接参数缓冲只能是 raw：D3D11 的 MISC_BUFFER_STRUCTURED 与 MISC_DRAWINDIRECT_ARGS
    // 互斥，两个标志一起给 CreateBuffer 会 E_INVALIDARG。
    StructuredBufferDesc argsDesc;
    argsDesc.kind = StructuredBufferKind::kByteAddress;
    argsDesc.elementSize = 4;
    argsDesc.elementCount = sizeof(DrawIndexedIndirectArgs) / 4;
    argsDesc.buffer = argsInit;
    mDrawArgs = T3D_RENDER_BUFFER_MGR.loadStructuredBuffer(
        argsDesc, MemoryType::kBoth, Usage::kStatic, kCPURead,
        kGPUIndirectArgs | kGPUShaderResource);
    argsInit.release();

    StructuredBufferDesc visDesc;
    visDesc.kind = StructuredBufferKind::kStructured;
    visDesc.elementSize = sizeof(uint32_t);
    visDesc.elementCount = kParticleCount;
    visDesc.hasCounter = true;
    mVisible = T3D_RENDER_BUFFER_MGR.loadStructuredBuffer(
        visDesc, MemoryType::kVRAM, Usage::kStatic, kCPUNone,
        kGPUUnorderedAccess | kGPUShaderResource);

    auto makeCB = [](const char *name, size_t bytes) -> ConstantBufferPtr
    {
        Buffer cb = allocBytes(bytes);
        ConstantBufferPtr ptr = T3D_RENDER_BUFFER_MGR.loadConstantBuffer(
            name, 0, cb, MemoryType::kBoth, Usage::kDynamic, kCPUWrite);
        cb.release();
        return ptr;
    };

    mUpdateCB = makeCB("ParticleParams", 32);
    mCullCB = makeCB("CullParams", 80);
    mDrawCB = makeCB("DrawParams", 208);

    RasterizerDesc raster;
    raster.CullMode = CullingMode::kNone;
    mRasterizer = T3D_RENDER_STATE_MGR.loadRasterizerState(raster);

    DepthStencilDesc depth;
    mDepthState = T3D_RENDER_STATE_MGR.loadDepthStencilState(depth);

    BlendDesc blend;
    mBlendState = T3D_RENDER_STATE_MGR.loadBlendState(blend);

    mCullReady = (mCullKernel != nullptr && mVisible != nullptr && mDrawArgs != nullptr
        && mIndexBuffer != nullptr);
    mReady = (mParticles != nullptr && mDrawPS != nullptr
        && mUpdateCB != nullptr && mDrawCB != nullptr
        && mRasterizer != nullptr && mDepthState != nullptr && mBlendState != nullptr
        && (mDrawVS != nullptr
            || (mDrawCpuVS != nullptr && mCpuDecl != nullptr && mCpuVB != nullptr)));

    if (mUpdateKernel == nullptr || mDrawVS == nullptr)
    {
        APP_LOG_DEBUG("[particles] GPU integrate/draw unavailable, visual track will stay on CPU.");
        mCpuMode = true;
    }

    if (!mCullReady)
    {
        APP_LOG_DEBUG("[particles] cull / indirect path not available on this backend.");
    }

    APP_LOG_DEBUG("[particles] ready=%d cpuFallback=%d cullReady=%d count=%u",
        mReady ? 1 : 0, mCpuMode ? 1 : 0, mCullReady ? 1 : 0, kParticleCount);

    return mReady ? T3D_OK : T3D_ERR_FAIL;
}

void ParticleSystem::teardown()
{
    // 还挂着的回读先收掉，unmap 阻塞等 GPU，总比把映射着的缓冲直接释放掉强
    consumeReadback();

    mReady = false;
    mCullReady = false;
    mCullEnabled = false;
    mPendingCountReadback = false;
    mCountHandle = ReadbackHandle::invalid();

    mRasterizer = nullptr;
    mDepthState = nullptr;
    mBlendState = nullptr;

    mUpdateCB = nullptr;
    mCullCB = nullptr;
    mDrawCB = nullptr;

    mParticles = nullptr;
    mVisible = nullptr;
    mDrawArgs = nullptr;
    mIndexBuffer = nullptr;
    mCpuVB = nullptr;
    mCpuDecl = nullptr;
    mGpuDecl = nullptr;

    mDrawVS = nullptr;
    mDrawCpuVS = nullptr;
    mDrawPS = nullptr;
    mUpdateKernel = nullptr;
    mCullKernel = nullptr;
    mDummyShaders.clear();

    mCpuParticles.clear();
    mCpuParticles.shrink_to_fit();

    mCamera = nullptr;
}

void ParticleSystem::packMatrix(float *dst, const Matrix4 &m) const
{
    const Matrix4 packed = isGLFamily() ? m.transpose() : m;
    for (int r = 0; r < 4; ++r)
    {
        for (int c = 0; c < 4; ++c)
        {
            dst[r * 4 + c] = packed[r][c];
        }
    }
}

TResult ParticleSystem::uploadCb(ConstantBuffer *cb, const void *data, size_t bytes)
{
    if (cb == nullptr)
    {
        return T3D_ERR_INVALID_POINTER;
    }
    Buffer buf = wrapBytes(const_cast<void *>(data), bytes);
    const size_t written = cb->writeData(0, buf, true);
    buf.Data = nullptr;
    return (written == bytes) ? T3D_OK : T3D_ERR_FAIL;
}

TResult ParticleSystem::uploadParticles()
{
    if (mCpuVB == nullptr)
    {
        return T3D_ERR_FAIL;
    }
    Buffer buf = wrapBytes(mCpuParticles.data(), sizeof(Particle) * kParticleCount);
    const size_t expected = sizeof(Particle) * kParticleCount;
    const size_t written = mCpuVB->writeData(0, buf, true);
    buf.Data = nullptr;
    return (written == expected) ? T3D_OK : T3D_ERR_FAIL;
}

void ParticleSystem::integrateCpu(float dt, float timeSeconds, const float attractor[4])
{
    for (uint32_t i = 0; i < kParticleCount; ++i)
    {
        Particle &p = mCpuParticles[i];
        float dir[3] =
        {
            attractor[0] - p.position[0],
            attractor[1] - p.position[1],
            attractor[2] - p.position[2]
        };
        const float len2 = dir[0] * dir[0] + dir[1] * dir[1] + dir[2] * dir[2] + 1e-4f;
        const float scale = (1.0f / sqrtf(len2)) * attractor[3] * dt;
        p.velocity[0] = (p.velocity[0] + dir[0] * scale) * 0.98f;
        p.velocity[1] = (p.velocity[1] + dir[1] * scale) * 0.98f;
        p.velocity[2] = (p.velocity[2] + dir[2] * scale) * 0.98f;
        p.position[0] += p.velocity[0] * dt;
        p.position[1] += p.velocity[1] * dt;
        p.position[2] += p.velocity[2] * dt;
        p.life -= dt;
        if (p.life < 0.0f)
        {
            p.life = 4.0f + frac(timeSeconds + i * 0.017f) * 4.0f;
            p.position[0] = frac(i * 0.0123f) * 8.0f - 4.0f;
            p.position[1] = frac(i * 0.0456f) * 8.0f - 4.0f;
            p.position[2] = frac(i * 0.0789f) * 8.0f - 4.0f;
            p.velocity[0] = p.velocity[1] = p.velocity[2] = 0.0f;
        }
    }
}

TResult ParticleSystem::record(float dt, float timeSeconds)
{
    if (!mReady)
    {
        return T3D_ERR_FAIL;
    }

    RHIContext *ctx = T3D_AGENT.getActiveRHIContext();
    if (ctx == nullptr || mCamera == nullptr)
    {
        return T3D_ERR_SYS_NOT_INIT;
    }

    const float attractor[4] =
    {
        cosf(timeSeconds) * 2.0f,
        0.0f,
        sinf(timeSeconds) * 2.0f,
        8.0f
    };

    if (!mPaused)
    {
        if (mCpuMode || mUpdateKernel == nullptr)
        {
            integrateCpu(dt, timeSeconds, attractor);
            TResult ret = uploadParticles();
            if (T3D_FAILED(ret))
            {
                return ret;
            }
        }
        else
        {
            float params[8] =
            {
                dt, timeSeconds, 0.0f, 0.0f,
                attractor[0], attractor[1], attractor[2], attractor[3]
            };
            TResult ret = uploadCb(mUpdateCB.get(), params, sizeof(params));
            if (T3D_FAILED(ret)) return ret;

            ret = ctx->setComputeShader(mUpdateKernel->getVariant());
            if (T3D_FAILED(ret)) return ret;

            ConstantBuffers cbs;
            cbs.push_back(mUpdateCB);
            ret = ctx->setCSConstantBuffers(0, cbs);
            if (T3D_FAILED(ret)) return ret;

            UnorderedAccessBuffers uavs;
            uavs.push_back(mParticles);
            ret = ctx->setCSUnorderedAccessBuffers(uavSlot(0, 0), uavs);
            if (T3D_FAILED(ret)) return ret;

            ret = mUpdateKernel->dispatchThreads(kParticleCount);
            if (T3D_FAILED(ret)) return ret;

            ctx->uavBarrier(uavs);
            unbindComputeUAVs(ctx, uavSlot(0, 0), 1);
            ctx->setComputeShader(nullptr);
        }
    }

    const bool doCull = mCullEnabled && mCullReady && !mCpuMode;
    mPendingCountReadback = false;

    if (doCull)
    {
        const Matrix4 view = mCamera->getViewMatrix();
        const Matrix4 proj = mCamera->getProjectionMatrix();
        const Matrix4 viewProj = proj * view;

        alignas(16) float cullData[20];
        packMatrix(cullData, viewProj);
        cullData[16] = 0.6f;
        cullData[17] = cullData[18] = cullData[19] = 0.0f;
        TResult ret = uploadCb(mCullCB.get(), cullData, sizeof(cullData));
        if (T3D_FAILED(ret)) return ret;

        ret = ctx->setComputeShader(mCullKernel->getVariant());
        if (T3D_FAILED(ret)) return ret;

        ConstantBuffers cbs;
        cbs.push_back(mCullCB);
        ret = ctx->setCSConstantBuffers(0, cbs);
        if (T3D_FAILED(ret)) return ret;

        StructuredBuffers srvs;
        srvs.push_back(mParticles);
        ret = ctx->setCSStructuredBuffers(0, srvs);
        if (T3D_FAILED(ret)) return ret;

        UnorderedAccessBuffers uavs;
        uavs.push_back(mVisible);
        UAVInitialCounts counts;
        counts.push_back(0);
        ret = ctx->setCSUnorderedAccessBuffers(uavSlot(1, 0), uavs, counts);
        if (T3D_FAILED(ret)) return ret;

        ret = mCullKernel->dispatchThreads(kParticleCount);
        if (T3D_FAILED(ret)) return ret;

        ctx->copyStructureCount(mDrawArgs.get(),
            offsetof(DrawIndexedIndirectArgs, instanceCount), mVisible.get());

        UnorderedAccessBuffers barrier;
        barrier.push_back(mParticles);
        barrier.push_back(mVisible);
        ctx->uavBarrier(barrier);
        unbindComputeUAVs(ctx, uavSlot(1, 0), 1);
        unbindCSStructuredBuffers(ctx, 0, 1);
        ctx->setComputeShader(nullptr);
    }

    alignas(16) float drawData[52];
    const Matrix4 view = mCamera->getViewMatrix();
    const Matrix4 proj = mCamera->getProjectionMatrix();
    const Matrix4 viewProj = proj * view;
    packMatrix(drawData + 0, view);
    packMatrix(drawData + 16, proj);
    packMatrix(drawData + 32, viewProj);
    drawData[48] = doCull ? 1.0f : 0.0f;
    drawData[49] = drawData[50] = drawData[51] = 0.0f;
    TResult ret = uploadCb(mDrawCB.get(), drawData, sizeof(drawData));
    if (T3D_FAILED(ret)) return ret;

    const bool cpuDraw = mCpuMode || mUpdateKernel == nullptr;
    if (cpuDraw && (mDrawCpuVS == nullptr || mCpuVB == nullptr || mCpuDecl == nullptr))
    {
        APP_LOG_DEBUG("[particles] CPU draw path is not available !");
        return T3D_ERR_FAIL;
    }

    // 自检用例留下的 UAV 绑定同样会和顶点拉流的 SRV 冲突，绘制前统一清一遍
    unbindComputeUAVs(ctx, 0, 4);

    ctx->setRenderTarget(mCamera->getRenderTarget());
    ctx->setViewport(mCamera->getViewport());

    // 管线渲染的是离屏 RT，最后只把颜色 blit 回后台缓冲，窗口自己的深度缓冲一帧都
    // 没被清过，内容是全 0。深度函数 Less 拿 0 当参考值，粒子会被整片剔掉，所以这里
    // 自己清一次再画。
    ctx->clearDepthStencil(1.0f, 0);
    ctx->beginPass();

    ctx->setRasterizerState(mRasterizer.get());
    ctx->setDepthStencilState(mDepthState.get());
    ctx->setBlendState(mBlendState.get());
    ctx->setPrimitiveType(PrimitiveType::kTriangleList);

    ConstantBuffers drawCbs;
    drawCbs.push_back(mDrawCB);
    ctx->setVSConstantBuffers(0, drawCbs);

    // 沿用引擎里 VS → PS 的绑定顺序（ForwardRenderPipeline、ImGui 都是这个顺序）。
    ctx->setVertexShader(cpuDraw ? mDrawCpuVS.get() : mDrawVS.get());
    ctx->setPixelShader(mDrawPS.get());

    if (cpuDraw)
    {
        ctx->setVertexDeclaration(mCpuDecl.get());
        VertexBuffers vbs;
        vbs.push_back(mCpuVB);
        VertexStrides strides;
        strides.push_back(sizeof(Particle));
        VertexOffsets offsets;
        offsets.push_back(0);
        ctx->setVertexBuffers(0, vbs, strides, offsets);
        ret = ctx->renderInstanced(6, kParticleCount, 0, 0);
        mLastVisibleCount = kParticleCount;
    }
    else
    {
        if (mGpuDecl != nullptr)
        {
            ctx->setVertexDeclaration(mGpuDecl.get());
            if (mCpuVB != nullptr)
            {
                // 占位属性没人读，但 IA 声明了 slot 0，不给缓冲会挨 debug layer 的警告
                VertexBuffers vbs;
                vbs.push_back(mCpuVB);
                VertexStrides strides;
                strides.push_back(sizeof(Particle));
                VertexOffsets offsets;
                offsets.push_back(0);
                ctx->setVertexBuffers(0, vbs, strides, offsets);
            }
        }

        StructuredBuffers vsSrvs;
        vsSrvs.push_back(mParticles);
        vsSrvs.push_back(mVisible);
        ctx->setVSStructuredBuffers(0, vsSrvs);

        if (doCull)
        {
            ctx->setIndexBuffer(mIndexBuffer.get());
            ret = ctx->renderIndexedIndirect(mDrawArgs.get(), 0);
            if (T3D_SUCCEEDED(ret) && (Time::frameCount() % 30 == 0))
            {
                mCountHandle = mDrawArgs->map(0, 0);
                mPendingCountReadback = mCountHandle.isValid();
            }
        }
        else
        {
            ret = ctx->renderInstanced(6, kParticleCount, 0, 0);
            mLastVisibleCount = kParticleCount;
        }
    }

    if (!cpuDraw)
    {
        unbindVSStructuredBuffers(ctx, 0, 2);
    }

    ctx->endPass();

    // setRenderTarget 里 backupRenderState() 把当时绑着的整套状态 AddRef 了一份，
    // 这份引用要靠 reset() 还回去（T3DApplication.h:121 的约定）。不还就是每帧漏一份，
    // 退出时 ReportLiveDeviceObjects 会看到一串 refcount 约等于帧数的残留对象。
    ctx->reset();
    return ret;
}

TResult ParticleSystem::consumeReadback()
{
    if (!mPendingCountReadback)
    {
        return T3D_OK;
    }

    mPendingCountReadback = false;
    Buffer data;
    TResult ret = mDrawArgs->unmap(mCountHandle, data);
    mCountHandle = ReadbackHandle::invalid();
    if (T3D_FAILED(ret) || data.Data == nullptr || data.DataSize < sizeof(DrawIndexedIndirectArgs))
    {
        data.release();
        return T3D_ERR_FAIL;
    }

    const auto *args = reinterpret_cast<const DrawIndexedIndirectArgs *>(data.Data);
    mLastVisibleCount = args->instanceCount;
    data.release();

    APP_LOG_DEBUG("[K10] visible=%u / %u (clipScale=0.6)", mLastVisibleCount, kParticleCount);
    return T3D_OK;
}
