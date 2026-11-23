/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to furnish persons to whom the Software is
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

#include "ComputeCases.h"
#include "ComputeKernel.h"
#include "ComputeShaderSources.h"
#include "../Common/SampleApp.h"

#include <Material/T3DShaderVariantInstance.h>

#include <algorithm>
#include <cstring>
#include <cmath>
#include <cstddef>


using namespace Tiny3D;


namespace
{
    bool isGLFamily()
    {
        const uint32_t bit = currentBackendBit();
        return (bit & (kBackendGL4 | kBackendGLES3)) != 0;
    }

    /// HLSL 的 u# 与 t# 是两套编号；GL SSBO 是统一空间，UAV 排在 SRV 后面。
    uint32_t uavSlot(uint32_t srvCount, uint32_t uRegister)
    {
        return isGLFamily() ? (srvCount + uRegister) : uRegister;
    }

    Buffer allocBytes(size_t bytes)
    {
        Buffer buf;
        buf.DataSize = bytes;
        buf.Data = T3D_POD_NEW_ARRAY(uint8_t, bytes);
        memset(buf.Data, 0, bytes);
        return buf;
    }

    StructuredBufferPtr loadSB(StructuredBufferKind kind, uint32_t elementSize, uint32_t count,
        uint32_t gpuAccess, uint32_t cpuAccess, const Buffer &init = Buffer(),
        PixelFormat format = PixelFormat::E_PF_UNKNOWN, bool hasCounter = false)
    {
        StructuredBufferDesc desc;
        desc.kind = kind;
        desc.elementSize = elementSize;
        desc.elementCount = count;
        desc.format = format;
        desc.hasCounter = hasCounter;
        desc.buffer = init;

        return T3D_RENDER_BUFFER_MGR.loadStructuredBuffer(
            desc, MemoryType::kVRAM, Usage::kStatic, cpuAccess, gpuAccess);
    }

    TResult mapWhole(RenderBuffer *buf, ReadbackHandle &handle)
    {
        handle = buf->map(0, 0);
        return handle.isValid() ? T3D_OK : T3D_ERR_FAIL;
    }

    TResult unmapCheckSize(RenderBuffer *buf, ReadbackHandle &handle, Buffer &data, size_t expected)
    {
        TResult ret = buf->unmap(handle, data);
        handle = ReadbackHandle::invalid();
        if (T3D_FAILED(ret))
        {
            return ret;
        }
        if (data.Data == nullptr || data.DataSize < expected)
        {
            data.release();
            return T3D_ERR_FAIL;
        }
        return T3D_OK;
    }


    class CaseLinearWrite : public ComputeCase
    {
    public:
        static const uint32_t kElemCount = 1024;

        const char *getName() const override { return "K1 linear write"; }
        uint32_t getRequiredCaps() const override
        {
            return kNeedCompute | kNeedUnorderedAccess | kNeedStructuredBuffer | kNeedReadback;
        }
        uint32_t getBackendMask() const override { return kBackendAll; }

        TResult setup() override
        {
            mKernel = ComputeKernel::create("ComputeLinearWrite", linearWriteSource(), 64);
            if (mKernel == nullptr)
            {
                return T3D_ERR_FAIL;
            }
            mOutput = loadSB(StructuredBufferKind::kStructured, sizeof(uint32_t), kElemCount,
                kGPUUnorderedAccess | kGPUShaderResource, kCPURead);
            return mOutput != nullptr ? T3D_OK : T3D_ERR_FAIL;
        }

        TResult record() override
        {
            RHIContext *ctx = T3D_AGENT.getActiveRHIContext();
            TResult ret = ctx->setComputeShader(mKernel->getVariant());
            if (T3D_FAILED(ret)) return ret;

            UnorderedAccessBuffers uavs;
            uavs.push_back(mOutput);
            ret = ctx->setCSUnorderedAccessBuffers(uavSlot(0, 0), uavs);
            if (T3D_FAILED(ret)) return ret;

            ret = mKernel->dispatchThreads(kElemCount);
            if (T3D_FAILED(ret)) return ret;

            ctx->uavBarrier(uavs);
            ctx->setComputeShader(nullptr);
            return mapWhole(mOutput.get(), mHandle);
        }

        TResult verify() override
        {
            Buffer data;
            TResult ret = unmapCheckSize(mOutput.get(), mHandle, data, sizeof(uint32_t) * kElemCount);
            if (T3D_FAILED(ret)) return ret;

            const uint32_t *out = reinterpret_cast<const uint32_t *>(data.Data);
            uint32_t mismatches = 0;
            uint32_t first = 0, got = 0;
            for (uint32_t i = 0; i < kElemCount; ++i)
            {
                if (out[i] != i * 2)
                {
                    if (mismatches == 0) { first = i; got = out[i]; }
                    ++mismatches;
                }
            }
            data.release();
            if (mismatches != 0)
            {
                APP_LOG_DEBUG("[%s] FAILED : %u mismatches, first [%u] got %u expected %u",
                    getName(), mismatches, first, got, first * 2);
                return T3D_ERR_FAIL;
            }
            return T3D_OK;
        }

    private:
        ComputeKernelPtr    mKernel;
        StructuredBufferPtr mOutput;
        ReadbackHandle      mHandle {};
    };


    class CaseSaxpy : public ComputeCase
    {
    public:
        static const uint32_t kElemCount = 1024;

        const char *getName() const override { return "K2 saxpy"; }
        uint32_t getRequiredCaps() const override
        {
            return kNeedCompute | kNeedUnorderedAccess | kNeedStructuredBuffer | kNeedReadback;
        }
        uint32_t getBackendMask() const override { return kBackendAll; }

        TResult setup() override
        {
            mKernel = ComputeKernel::create("ComputeSaxpy", saxpySource(), 64);
            if (mKernel == nullptr) return T3D_ERR_FAIL;

            Buffer init = allocBytes(sizeof(float) * 4 * kElemCount);
            float *f = reinterpret_cast<float *>(init.Data);
            for (uint32_t i = 0; i < kElemCount; ++i)
            {
                f[i * 4 + 0] = float(i);
                f[i * 4 + 1] = float(i) * 0.5f;
                f[i * 4 + 2] = 1.0f;
                f[i * 4 + 3] = 2.0f;
            }

            mInput = loadSB(StructuredBufferKind::kStructured, sizeof(float) * 4, kElemCount,
                kGPUShaderResource, kCPUNone, init);
            mOutput = loadSB(StructuredBufferKind::kStructured, sizeof(float) * 4, kElemCount,
                kGPUUnorderedAccess | kGPUShaderResource, kCPURead);
            // kVRAM 会接管 init.Data，这里只把指针摘掉以免 double-free
            init.Data = nullptr;
            init.DataSize = 0;

            struct Params { float scale[4]; float bias[4]; } params;
            params.scale[0] = 2.0f; params.scale[1] = 3.0f; params.scale[2] = 4.0f; params.scale[3] = 1.0f;
            params.bias[0] = 1.0f; params.bias[1] = -1.0f; params.bias[2] = 0.5f; params.bias[3] = 0.0f;

            Buffer cb = allocBytes(sizeof(params));
            memcpy(cb.Data, &params, sizeof(params));
            mCB = T3D_RENDER_BUFFER_MGR.loadConstantBuffer("SaxpyParams", 0, cb,
                MemoryType::kBoth, Usage::kDynamic, kCPUWrite);
            cb.release();

            return (mInput && mOutput && mCB) ? T3D_OK : T3D_ERR_FAIL;
        }

        TResult record() override
        {
            RHIContext *ctx = T3D_AGENT.getActiveRHIContext();
            TResult ret = ctx->setComputeShader(mKernel->getVariant());
            if (T3D_FAILED(ret)) return ret;

            ConstantBuffers cbs; cbs.push_back(mCB);
            ret = ctx->setCSConstantBuffers(0, cbs);
            if (T3D_FAILED(ret)) return ret;

            StructuredBuffers srvs; srvs.push_back(mInput);
            ret = ctx->setCSStructuredBuffers(0, srvs);
            if (T3D_FAILED(ret)) return ret;

            UnorderedAccessBuffers uavs; uavs.push_back(mOutput);
            ret = ctx->setCSUnorderedAccessBuffers(uavSlot(1, 0), uavs);
            if (T3D_FAILED(ret)) return ret;

            ret = mKernel->dispatchThreads(kElemCount);
            if (T3D_FAILED(ret)) return ret;

            ctx->uavBarrier(uavs);
            ctx->setComputeShader(nullptr);
            return mapWhole(mOutput.get(), mHandle);
        }

        TResult verify() override
        {
            Buffer data;
            TResult ret = unmapCheckSize(mOutput.get(), mHandle, data, sizeof(float) * 4 * kElemCount);
            if (T3D_FAILED(ret)) return ret;

            const float *out = reinterpret_cast<const float *>(data.Data);
            const float scale[4] = { 2, 3, 4, 1 };
            const float bias[4] = { 1, -1, 0.5f, 0 };
            uint32_t mismatches = 0;
            for (uint32_t i = 0; i < kElemCount; ++i)
            {
                const float in[4] = { float(i), float(i) * 0.5f, 1.0f, 2.0f };
                for (int c = 0; c < 4; ++c)
                {
                    const float expect = in[c] * scale[c] + bias[c];
                    if (fabsf(out[i * 4 + c] - expect) > 1e-4f)
                    {
                        ++mismatches;
                    }
                }
            }
            data.release();
            if (mismatches != 0)
            {
                APP_LOG_DEBUG("[%s] FAILED : %u component mismatches", getName(), mismatches);
                return T3D_ERR_FAIL;
            }
            return T3D_OK;
        }

    private:
        ComputeKernelPtr    mKernel;
        StructuredBufferPtr mInput;
        StructuredBufferPtr mOutput;
        ConstantBufferPtr   mCB;
        ReadbackHandle      mHandle {};
    };


    class CaseRawTyped : public ComputeCase
    {
    public:
        static const uint32_t kElemCount = 1024;

        const char *getName() const override { return "K3 raw typed"; }
        uint32_t getRequiredCaps() const override
        {
            return kNeedCompute | kNeedUnorderedAccess | kNeedStructuredBuffer | kNeedReadback;
        }
        uint32_t getBackendMask() const override { return kBackendD3D11 | kBackendGL4; }

        TResult setup() override
        {
            mKernel = ComputeKernel::create("ComputeRawTyped", rawTypedSource(), 64);
            if (mKernel == nullptr) return T3D_ERR_FAIL;

            mRaw = loadSB(StructuredBufferKind::kByteAddress, 4, kElemCount,
                kGPUUnorderedAccess, kCPURead);
            mTyped = loadSB(StructuredBufferKind::kTyped, 4, kElemCount,
                kGPUUnorderedAccess, kCPURead, Buffer(), PixelFormat::E_PF_R8G8B8A8);
            return (mRaw && mTyped) ? T3D_OK : T3D_ERR_FAIL;
        }

        TResult record() override
        {
            RHIContext *ctx = T3D_AGENT.getActiveRHIContext();
            TResult ret = ctx->setComputeShader(mKernel->getVariant());
            if (T3D_FAILED(ret)) return ret;

            UnorderedAccessBuffers uavs;
            uavs.push_back(mRaw);
            uavs.push_back(mTyped);
            ret = ctx->setCSUnorderedAccessBuffers(0, uavs);
            if (T3D_FAILED(ret)) return ret;

            ret = mKernel->dispatchThreads(kElemCount);
            if (T3D_FAILED(ret)) return ret;

            ctx->uavBarrier(uavs);
            ctx->setComputeShader(nullptr);

            TResult r0 = mapWhole(mRaw.get(), mRawHandle);
            TResult r1 = mapWhole(mTyped.get(), mTypedHandle);
            return (T3D_SUCCEEDED(r0) && T3D_SUCCEEDED(r1)) ? T3D_OK : T3D_ERR_FAIL;
        }

        TResult verify() override
        {
            Buffer rawData, typedData;
            TResult ret = unmapCheckSize(mRaw.get(), mRawHandle, rawData, sizeof(uint32_t) * kElemCount);
            if (T3D_FAILED(ret)) return ret;
            ret = unmapCheckSize(mTyped.get(), mTypedHandle, typedData, 4 * kElemCount);
            if (T3D_FAILED(ret))
            {
                rawData.release();
                return ret;
            }

            const uint32_t *raw = reinterpret_cast<const uint32_t *>(rawData.Data);
            uint32_t rawBad = 0;
            for (uint32_t i = 0; i < kElemCount; ++i)
            {
                if (raw[i] != (i ^ 0xA5A5A5A5u)) ++rawBad;
            }

            const uint8_t *px = typedData.Data;
            uint32_t typedBad = 0;
            for (uint32_t i = 0; i < kElemCount; ++i)
            {
                const uint8_t *p = px + i * 4;
                if (p[0] != uint8_t(i & 255) || p[1] != 0 || p[2] != 128 || p[3] != 255)
                {
                    ++typedBad;
                }
            }
            rawData.release();
            typedData.release();

            if (rawBad || typedBad)
            {
                APP_LOG_DEBUG("[%s] FAILED : rawBad=%u typedBad=%u", getName(), rawBad, typedBad);
                return T3D_ERR_FAIL;
            }
            return T3D_OK;
        }

    private:
        ComputeKernelPtr    mKernel;
        StructuredBufferPtr mRaw;
        StructuredBufferPtr mTyped;
        ReadbackHandle      mRawHandle {};
        ReadbackHandle      mTypedHandle {};
    };


    class CaseReduce : public ComputeCase
    {
    public:
        static const uint32_t kElemCount = 65536;

        const char *getName() const override { return "K4 reduce barrier"; }
        uint32_t getRequiredCaps() const override
        {
            return kNeedCompute | kNeedUnorderedAccess | kNeedStructuredBuffer | kNeedReadback;
        }
        uint32_t getBackendMask() const override { return kBackendAll; }

        TResult setup() override
        {
            mPass0 = ComputeKernel::create("ComputeReducePass0", reducePass0Source(), 256);
            mPass1 = ComputeKernel::create("ComputeReducePass1", reducePass1Source(), 256);
            if (mPass0 == nullptr || mPass1 == nullptr) return T3D_ERR_FAIL;

            Buffer init = allocBytes(sizeof(uint32_t) * kElemCount);
            uint32_t *v = reinterpret_cast<uint32_t *>(init.Data);
            for (uint32_t i = 0; i < kElemCount; ++i) v[i] = i;

            mInput = loadSB(StructuredBufferKind::kStructured, 4, kElemCount,
                kGPUShaderResource, kCPUNone, init);
            mPartial = loadSB(StructuredBufferKind::kStructured, 4, 256,
                kGPUUnorderedAccess | kGPUShaderResource, kCPUNone);
            mResult = loadSB(StructuredBufferKind::kStructured, 4, 1,
                kGPUUnorderedAccess, kCPURead);
            init.Data = nullptr;
            init.DataSize = 0;
            return (mInput && mPartial && mResult) ? T3D_OK : T3D_ERR_FAIL;
        }

        TResult record() override
        {
            RHIContext *ctx = T3D_AGENT.getActiveRHIContext();

            TResult ret = ctx->setComputeShader(mPass0->getVariant());
            if (T3D_FAILED(ret)) return ret;
            StructuredBuffers srvs; srvs.push_back(mInput);
            ret = ctx->setCSStructuredBuffers(0, srvs);
            if (T3D_FAILED(ret)) return ret;
            UnorderedAccessBuffers uavs0; uavs0.push_back(mPartial);
            ret = ctx->setCSUnorderedAccessBuffers(uavSlot(1, 0), uavs0);
            if (T3D_FAILED(ret)) return ret;
            ret = mPass0->dispatchThreads(kElemCount);
            if (T3D_FAILED(ret)) return ret;
            ctx->uavBarrier(uavs0);

            ret = ctx->setComputeShader(mPass1->getVariant());
            if (T3D_FAILED(ret)) return ret;
            UnorderedAccessBuffers uavs1; uavs1.push_back(mPartial); uavs1.push_back(mResult);
            ret = ctx->setCSUnorderedAccessBuffers(0, uavs1);
            if (T3D_FAILED(ret)) return ret;
            ret = mPass1->dispatchThreads(256);
            if (T3D_FAILED(ret)) return ret;
            ctx->uavBarrier(uavs1);
            ctx->setComputeShader(nullptr);
            return mapWhole(mResult.get(), mHandle);
        }

        TResult verify() override
        {
            Buffer data;
            TResult ret = unmapCheckSize(mResult.get(), mHandle, data, sizeof(uint32_t));
            if (T3D_FAILED(ret)) return ret;
            const uint32_t got = *reinterpret_cast<const uint32_t *>(data.Data);
            data.release();
            const uint32_t expect = 2147450880u;
            if (got != expect)
            {
                APP_LOG_DEBUG("[%s] FAILED : got %u expected %u", getName(), got, expect);
                return T3D_ERR_FAIL;
            }
            return T3D_OK;
        }

    private:
        ComputeKernelPtr    mPass0;
        ComputeKernelPtr    mPass1;
        StructuredBufferPtr mInput;
        StructuredBufferPtr mPartial;
        StructuredBufferPtr mResult;
        ReadbackHandle      mHandle {};
    };


    /**
     * \brief compute 把 (x, y, 128, 255) 写进 256x256 的 RGBA8 纹理 UAV 后读回
     * \remarks renderTexture 为 true 时（K21）纹理改走 createRenderTexture 创建，
     *          校验 RT 带 kGPUUnorderedAccess 时能作为 UAV（E.1.6）。
     */
    class CaseTextureWrite : public ComputeCase
    {
    public:
        static const uint32_t kSize = 256;

        explicit CaseTextureWrite(bool renderTexture = false) : mRenderTexture(renderTexture) {}

        const char *getName() const override { return mRenderTexture ? "K21 render texture UAV" : "K5 texture UAV"; }
        uint32_t getRequiredCaps() const override
        {
            return kNeedCompute | kNeedUnorderedAccess | kNeedReadback;
        }
        uint32_t getBackendMask() const override
        {
            return mRenderTexture ? (kBackendD3D11 | kBackendGL4) : (kBackendD3D11 | kBackendGL4 | kBackendGLES3);
        }

        TResult setup() override
        {
            mKernel = ComputeKernel::create("ComputeTextureWrite", textureWriteSource(), 8, 8, 1);
            if (mKernel == nullptr) return T3D_ERR_FAIL;

            if (mRenderTexture)
            {
                // PixelBuffer 只保存描述的指针，缓存里的对象可能活得比本用例久，描述用静态存储
                static PixelBuffer2DDesc sDesc = makeRenderTextureDesc();
                mTex = T3D_RENDER_BUFFER_MGR.loadRenderTexture(&sDesc, MemoryType::kVRAM, Usage::kStatic, kCPURead);
                return (mTex != nullptr && mTex->getRHIResource() != nullptr) ? T3D_OK : T3D_ERR_FAIL;
            }

            PixelBuffer2DDesc *desc = T3D_NEW PixelBuffer2DDesc();
            desc->width = kSize;
            desc->height = kSize;
            desc->mipmaps = 1;
            desc->arraySize = 1;
            desc->format = PixelFormat::E_PF_R8G8B8A8;
            desc->gpuAccess = kGPUUnorderedAccess;
            desc->shaderReadable = true;
            mTex = T3D_RENDER_BUFFER_MGR.loadPixelBuffer2D(desc, MemoryType::kVRAM, Usage::kStatic, kCPURead);
            return mTex != nullptr ? T3D_OK : T3D_ERR_FAIL;
        }

        TResult record() override
        {
            RHIContext *ctx = T3D_AGENT.getActiveRHIContext();
            TResult ret = ctx->setComputeShader(mKernel->getVariant());
            if (T3D_FAILED(ret)) return ret;

            UnorderedAccessBuffers uavs; uavs.push_back(mTex);
            ret = ctx->setCSUnorderedAccessBuffers(0, uavs);
            if (T3D_FAILED(ret)) return ret;
            ret = mKernel->dispatchThreads(kSize, kSize);
            if (T3D_FAILED(ret)) return ret;
            ctx->uavBarrier(uavs);
            ctx->setComputeShader(nullptr);

            ReadbackRegion region;
            region.size = Vector3(Real(kSize), Real(kSize), Real(1));
            mHandle = ctx->map(mTex.get(), region);
            return mHandle.isValid() ? T3D_OK : T3D_ERR_FAIL;
        }

        TResult verify() override
        {
            Buffer data;
            TResult ret = mTex->unmap(mHandle, data);
            mHandle = ReadbackHandle::invalid();
            if (T3D_FAILED(ret)) return ret;

            const size_t expected = size_t(kSize) * kSize * 4;
            uint32_t bad = 0;
            if (data.Data == nullptr || data.DataSize < expected)
            {
                data.release();
                return T3D_ERR_FAIL;
            }
            for (uint32_t y = 0; y < kSize; ++y)
            {
                for (uint32_t x = 0; x < kSize; ++x)
                {
                    const uint8_t *p = data.Data + (y * kSize + x) * 4;
                    if (p[0] != uint8_t(x) || p[1] != uint8_t(y) || p[2] != 128 || p[3] != 255)
                    {
                        ++bad;
                    }
                }
            }
            data.release();
            if (bad != 0)
            {
                APP_LOG_DEBUG("[%s] FAILED : %u pixels mismatch", getName(), bad);
                return T3D_ERR_FAIL;
            }
            return T3D_OK;
        }

    private:
        static PixelBuffer2DDesc makeRenderTextureDesc()
        {
            PixelBuffer2DDesc desc;
            desc.width = kSize;
            desc.height = kSize;
            desc.mipmaps = 1;
            desc.arraySize = 1;
            desc.format = PixelFormat::E_PF_R8G8B8A8;
            desc.gpuAccess = kGPUUnorderedAccess;
            desc.shaderReadable = true;
            return desc;
        }

        bool                mRenderTexture {false};
        ComputeKernelPtr    mKernel;
        PixelBuffer2DPtr    mTex;
        ReadbackHandle      mHandle {};
    };


    class CaseReflect : public ComputeCase
    {
    public:
        const char *getName() const override { return "K6 reflect"; }
        uint32_t getRequiredCaps() const override { return kNeedCompute; }
        uint32_t getBackendMask() const override { return kBackendD3D11 | kBackendGL4; }

        TResult setup() override
        {
            mKernel = ComputeKernel::create("ComputeReflectProbe", reflectProbeSource(), 8, 8, 1);
            return mKernel != nullptr ? T3D_OK : T3D_ERR_FAIL;
        }

        TResult record() override { return T3D_OK; }

        TResult verify() override
        {
            uint32_t gx = 0, gy = 0, gz = 0;
            mKernel->getGroupSize(gx, gy, gz);
            if (gx != 8 || gy != 8 || gz != 1)
            {
                APP_LOG_DEBUG("[%s] FAILED : numthreads=(%u,%u,%u) expected (8,8,1)",
                    getName(), gx, gy, gz);
                return T3D_ERR_FAIL;
            }

            const ShaderResourceParams &params = mKernel->getVariant()->getShaderResourceParams();

            if (isGLFamily())
            {
                // GLSL 没有区分 ByteAddressBuffer 与 StructuredBuffer 的语法，两者均映射为 SSBO，
                // 且 SSBO 在同一 binding 空间内依次编号（0, 1, 2, 3），image 独立编号（2）。
                auto findName = [&](const char *name) -> const ShaderResourceParam *
                {
                    auto it = params.find(name);
                    return (it != params.end()) ? it->second.get() : nullptr;
                };

                const ShaderResourceParam *srvS = findName("gStructuredSRV");
                const ShaderResourceParam *srvB = findName("gByteSRV");
                const ShaderResourceParam *uavS = findName("gStructuredUAV");
                const ShaderResourceParam *uavB = findName("gByteUAV");
                const ShaderResourceParam *uavT = findName("gTextureUAV");

                if (srvS == nullptr || srvB == nullptr || uavS == nullptr || uavB == nullptr || uavT == nullptr)
                {
                    APP_LOG_DEBUG("[%s] FAILED : missing GL reflected resources (count=%zu)",
                        getName(), params.size());
                    return T3D_ERR_FAIL;
                }

                if (srvS->getKind() != ShaderResourceParam::Kind::kStructuredSRV
                    || srvB->getKind() != ShaderResourceParam::Kind::kStructuredSRV
                    || uavS->getKind() != ShaderResourceParam::Kind::kStructuredUAV
                    || uavB->getKind() != ShaderResourceParam::Kind::kStructuredUAV
                    || uavT->getKind() != ShaderResourceParam::Kind::kTextureUAV)
                {
                    APP_LOG_DEBUG("[%s] FAILED : unexpected GL resource kinds", getName());
                    return T3D_ERR_FAIL;
                }

                if (srvS->getBinding() != 0 || srvB->getBinding() != 1
                    || uavS->getBinding() != 2 || uavB->getBinding() != 3 || uavT->getBinding() != 2)
                {
                    APP_LOG_DEBUG("[%s] FAILED : unexpected GL bindings srv(%u,%u) uav(%u,%u,%u)",
                        getName(), srvS->getBinding(), srvB->getBinding(),
                        uavS->getBinding(), uavB->getBinding(), uavT->getBinding());
                    return T3D_ERR_FAIL;
                }

                return T3D_OK;
            }

            auto findKind = [&](ShaderResourceParam::Kind kind) -> const ShaderResourceParam *
            {
                for (const auto &kv : params)
                {
                    if (kv.second != nullptr && kv.second->getKind() == kind)
                    {
                        return kv.second.get();
                    }
                }
                return nullptr;
            };

            const ShaderResourceParam *srvS = findKind(ShaderResourceParam::Kind::kStructuredSRV);
            const ShaderResourceParam *srvB = findKind(ShaderResourceParam::Kind::kByteAddressSRV);
            const ShaderResourceParam *uavS = findKind(ShaderResourceParam::Kind::kStructuredUAV);
            const ShaderResourceParam *uavB = findKind(ShaderResourceParam::Kind::kByteAddressUAV);
            const ShaderResourceParam *uavT = findKind(ShaderResourceParam::Kind::kTextureUAV);

            if (srvS == nullptr || srvB == nullptr || uavS == nullptr || uavB == nullptr || uavT == nullptr)
            {
                APP_LOG_DEBUG("[%s] FAILED : missing reflected resource kinds (count=%zu)",
                    getName(), params.size());
                return T3D_ERR_FAIL;
            }

            if (srvS->getBinding() != 0 || srvB->getBinding() != 1
                || uavS->getBinding() != 0 || uavB->getBinding() != 1 || uavT->getBinding() != 2)
            {
                APP_LOG_DEBUG("[%s] FAILED : unexpected bindings srv(%u,%u) uav(%u,%u,%u)",
                    getName(), srvS->getBinding(), srvB->getBinding(),
                    uavS->getBinding(), uavB->getBinding(), uavT->getBinding());
                return T3D_ERR_FAIL;
            }

            return T3D_OK;
        }

    private:
        ComputeKernelPtr mKernel;
    };


    class CaseDispatchIndirect : public ComputeCase
    {
    public:
        static const uint32_t kElemCount = 1024;

        const char *getName() const override { return "K7 dispatchIndirect"; }
        uint32_t getRequiredCaps() const override
        {
            return kNeedCompute | kNeedUnorderedAccess | kNeedStructuredBuffer
                | kNeedIndirectDispatch | kNeedReadback;
        }
        uint32_t getBackendMask() const override { return kBackendD3D11 | kBackendGL4; }

        TResult setup() override
        {
            mMakeArgs = ComputeKernel::create("ComputeMakeArgs", makeArgsSource(), 1, 1, 1);
            mWrite = ComputeKernel::create("ComputeLinearWrite", linearWriteSource(), 64);
            if (mMakeArgs == nullptr || mWrite == nullptr) return T3D_ERR_FAIL;

            // D3D11 的 MISC_BUFFER_STRUCTURED 与 MISC_DRAWINDIRECT_ARGS 互斥，
            // 间接参数缓冲只能走 raw，对应 shader 侧的 RWByteAddressBuffer。
            mArgs = loadSB(StructuredBufferKind::kByteAddress, 4, 3,
                kGPUUnorderedAccess | kGPUIndirectArgs, kCPUNone);

            Buffer init = allocBytes(sizeof(uint32_t) * kElemCount);
            uint32_t *v = reinterpret_cast<uint32_t *>(init.Data);
            for (uint32_t i = 0; i < kElemCount; ++i) v[i] = 0xDEADBEEFu;
            mOutput = loadSB(StructuredBufferKind::kStructured, 4, kElemCount,
                kGPUUnorderedAccess, kCPURead, init);
            init.Data = nullptr;
            init.DataSize = 0;
            return (mArgs && mOutput) ? T3D_OK : T3D_ERR_FAIL;
        }

        TResult record() override
        {
            RHIContext *ctx = T3D_AGENT.getActiveRHIContext();

            TResult ret = ctx->setComputeShader(mMakeArgs->getVariant());
            if (T3D_FAILED(ret)) return ret;
            UnorderedAccessBuffers argsUAV; argsUAV.push_back(mArgs);
            ret = ctx->setCSUnorderedAccessBuffers(0, argsUAV);
            if (T3D_FAILED(ret)) return ret;
            ret = mMakeArgs->dispatchThreads(1);
            if (T3D_FAILED(ret)) return ret;
            ctx->uavBarrier(argsUAV);

            ret = ctx->setComputeShader(mWrite->getVariant());
            if (T3D_FAILED(ret)) return ret;
            UnorderedAccessBuffers outUAV; outUAV.push_back(mOutput);
            ret = ctx->setCSUnorderedAccessBuffers(0, outUAV);
            if (T3D_FAILED(ret)) return ret;
            ret = ctx->dispatchIndirect(mArgs.get(), 0);
            if (T3D_FAILED(ret)) return ret;
            ctx->uavBarrier(outUAV);
            ctx->setComputeShader(nullptr);
            return mapWhole(mOutput.get(), mHandle);
        }

        TResult verify() override
        {
            Buffer data;
            TResult ret = unmapCheckSize(mOutput.get(), mHandle, data, sizeof(uint32_t) * kElemCount);
            if (T3D_FAILED(ret)) return ret;
            const uint32_t *out = reinterpret_cast<const uint32_t *>(data.Data);
            uint32_t bad = 0;
            for (uint32_t i = 0; i < 256; ++i)
            {
                if (out[i] != i * 2) ++bad;
            }
            if (out[256] != 0xDEADBEEFu) ++bad;
            data.release();
            if (bad != 0)
            {
                APP_LOG_DEBUG("[%s] FAILED : %u mismatches (expect 256 written, sentinel at 256)",
                    getName(), bad);
                return T3D_ERR_FAIL;
            }
            return T3D_OK;
        }

    private:
        ComputeKernelPtr    mMakeArgs;
        ComputeKernelPtr    mWrite;
        StructuredBufferPtr mArgs;
        StructuredBufferPtr mOutput;
        ReadbackHandle      mHandle {};
    };


    class CaseCopyCount : public ComputeCase
    {
    public:
        static const uint32_t kElemCount = 1024;

        const char *getName() const override { return "K8 copyStructureCount"; }
        uint32_t getRequiredCaps() const override
        {
            return kNeedCompute | kNeedUnorderedAccess | kNeedStructuredBuffer
                | kNeedAppendConsume | kNeedReadback;
        }
        uint32_t getBackendMask() const override { return kBackendD3D11 | kBackendGL4; }

        TResult setup() override
        {
            mKernel = ComputeKernel::create("ComputeFilterCount", filterCountSource(), 64);
            if (mKernel == nullptr) return T3D_ERR_FAIL;

            mVisible = loadSB(StructuredBufferKind::kStructured, 4, kElemCount,
                kGPUUnorderedAccess, kCPUNone, Buffer(), PixelFormat::E_PF_UNKNOWN, true);
            mArgs = loadSB(StructuredBufferKind::kByteAddress, 4, 5,
                kGPUIndirectArgs | kGPUShaderResource, kCPURead);
            return (mVisible && mArgs) ? T3D_OK : T3D_ERR_FAIL;
        }

        TResult record() override
        {
            RHIContext *ctx = T3D_AGENT.getActiveRHIContext();
            TResult ret = ctx->setComputeShader(mKernel->getVariant());
            if (T3D_FAILED(ret)) return ret;

            UnorderedAccessBuffers uavs; uavs.push_back(mVisible);
            UAVInitialCounts counts; counts.push_back(0);
            ret = ctx->setCSUnorderedAccessBuffers(0, uavs, counts);
            if (T3D_FAILED(ret)) return ret;
            ret = mKernel->dispatchThreads(kElemCount);
            if (T3D_FAILED(ret)) return ret;

            ret = ctx->copyStructureCount(mArgs.get(),
                offsetof(DrawIndexedIndirectArgs, instanceCount), mVisible.get());
            if (T3D_FAILED(ret)) return ret;

            ctx->uavBarrier(uavs);
            ctx->setComputeShader(nullptr);
            return mapWhole(mArgs.get(), mHandle);
        }

        TResult verify() override
        {
            Buffer data;
            TResult ret = unmapCheckSize(mArgs.get(), mHandle, data, sizeof(DrawIndexedIndirectArgs));
            if (T3D_FAILED(ret)) return ret;
            const auto *args = reinterpret_cast<const DrawIndexedIndirectArgs *>(data.Data);
            const uint32_t got = args->instanceCount;
            data.release();
            if (got != 512)
            {
                APP_LOG_DEBUG("[%s] FAILED : count=%u expected 512", getName(), got);
                return T3D_ERR_FAIL;
            }
            return T3D_OK;
        }

    private:
        ComputeKernelPtr    mKernel;
        StructuredBufferPtr mVisible;
        StructuredBufferPtr mArgs;
        ReadbackHandle      mHandle {};
    };


    class CaseVertexBufferSRV : public ComputeCase
    {
    public:
        static const uint32_t kElemCount = 256;

        const char *getName() const override { return "K9 vertex buffer SRV"; }
        uint32_t getRequiredCaps() const override
        {
            return kNeedCompute | kNeedUnorderedAccess | kNeedStructuredBuffer | kNeedReadback;
        }
        uint32_t getBackendMask() const override { return kBackendAll; }

        TResult setup() override
        {
            mKernel = ComputeKernel::create("ComputeVertexBufferRead", vertexBufferReadSource(), 64);
            if (mKernel == nullptr) return T3D_ERR_FAIL;

            Buffer init = allocBytes(sizeof(uint32_t) * kElemCount);
            uint32_t *v = reinterpret_cast<uint32_t *>(init.Data);
            for (uint32_t i = 0; i < kElemCount; ++i)
            {
                v[i] = i * 3u + 7u;
            }

            mInputVB = T3D_RENDER_BUFFER_MGR.loadVertexBuffer(
                sizeof(uint32_t) * 4, kElemCount / 4, init,
                MemoryType::kVRAM, Usage::kStatic, kCPUNone, kGPUShaderResource);
            init.Data = nullptr;
            init.DataSize = 0;

            mOutput = loadSB(StructuredBufferKind::kStructured, sizeof(uint32_t), kElemCount,
                kGPUUnorderedAccess | kGPUShaderResource, kCPURead);

            return (mInputVB && mOutput) ? T3D_OK : T3D_ERR_FAIL;
        }

        TResult record() override
        {
            RHIContext *ctx = T3D_AGENT.getActiveRHIContext();
            TResult ret = ctx->setComputeShader(mKernel->getVariant());
            if (T3D_FAILED(ret)) return ret;

            ShaderResourceBuffers srvs;
            srvs.push_back(mInputVB);
            ret = ctx->setCSStructuredBuffers(0, srvs);
            if (T3D_FAILED(ret)) return ret;

            UnorderedAccessBuffers uavs;
            uavs.push_back(mOutput);
            ret = ctx->setCSUnorderedAccessBuffers(uavSlot(1, 0), uavs);
            if (T3D_FAILED(ret)) return ret;

            ret = mKernel->dispatchThreads(kElemCount);
            if (T3D_FAILED(ret)) return ret;

            ctx->uavBarrier(uavs);
            ShaderResourceBuffers emptySrvs(1, nullptr);
            ctx->setCSStructuredBuffers(0, emptySrvs);
            ctx->setComputeShader(nullptr);
            return mapWhole(mOutput.get(), mHandle);
        }

        TResult verify() override
        {
            Buffer data;
            TResult ret = unmapCheckSize(mOutput.get(), mHandle, data, sizeof(uint32_t) * kElemCount);
            if (T3D_FAILED(ret)) return ret;

            const uint32_t *out = reinterpret_cast<const uint32_t *>(data.Data);
            uint32_t mismatches = 0;
            uint32_t first = 0, got = 0, expectFirst = 0;
            for (uint32_t i = 0; i < kElemCount; ++i)
            {
                const uint32_t expect = (i * 3u + 7u) ^ 0x5A5A5A5Au;
                if (out[i] != expect)
                {
                    if (mismatches == 0) { first = i; got = out[i]; expectFirst = expect; }
                    ++mismatches;
                }
            }
            data.release();
            if (mismatches != 0)
            {
                APP_LOG_DEBUG("[%s] FAILED : %u mismatches, first [%u] got 0x%08X expected 0x%08X",
                    getName(), mismatches, first, got, expectFirst);
                return T3D_ERR_FAIL;
            }
            return T3D_OK;
        }

    private:
        ComputeKernelPtr    mKernel;
        VertexBufferPtr     mInputVB;
        StructuredBufferPtr mOutput;
        ReadbackHandle      mHandle {};
    };


    /**
     * \brief 大组数派发：200 万个元素按 64 线程一组，一次派发 31250 个组
     * \remarks 线程组数量上限与单组线程数上限是两回事。D3D11 曾把前者填成 1024，
     *          超过 1024 组的合法派发会被 dispatch 拒绝。另外检查组数刚好超出
     *          maxDispatchGroupCount 时 dispatch 必须拒绝，证明上限确实生效。
     */
    class CaseLargeDispatch : public ComputeCase
    {
    public:
        static const uint32_t kElemCount = 2000000;

        const char *getName() const override { return "K15 large dispatch"; }
        uint32_t getRequiredCaps() const override
        {
            return kNeedCompute | kNeedUnorderedAccess | kNeedStructuredBuffer | kNeedReadback;
        }
        uint32_t getBackendMask() const override { return kBackendAll; }

        TResult setup() override
        {
            mKernel = ComputeKernel::create("ComputeLinearWrite", linearWriteSource(), 64);
            if (mKernel == nullptr) return T3D_ERR_FAIL;

            uint32_t gx = 0, gy = 0, gz = 0;
            mKernel->getGroupSize(gx, gy, gz);
            if (gx == 0 || (kElemCount % gx) != 0 || kElemCount / gx <= 1024)
            {
                APP_LOG_DEBUG("[%s] FAILED : numthreads.x=%u does not yield more than 1024 whole groups",
                    getName(), gx);
                return T3D_ERR_FAIL;
            }

            mOutput = loadSB(StructuredBufferKind::kStructured, sizeof(uint32_t), kElemCount,
                kGPUUnorderedAccess | kGPUShaderResource, kCPURead);
            return mOutput != nullptr ? T3D_OK : T3D_ERR_FAIL;
        }

        TResult record() override
        {
            RHIContext *ctx = T3D_AGENT.getActiveRHIContext();

            // 超限检查放在绑定 shader 之前：万一后端漏了校验，也不会真的派发出去
            const uint32_t maxX = ctx->getCapabilities().maxDispatchGroupCount[0];
            if (maxX != UINT32_MAX && T3D_SUCCEEDED(ctx->dispatch(maxX + 1, 1, 1)))
            {
                APP_LOG_DEBUG("[%s] FAILED : dispatch(%u, 1, 1) exceeds maxDispatchGroupCount but was accepted",
                    getName(), maxX + 1);
                return T3D_ERR_FAIL;
            }

            TResult ret = ctx->setComputeShader(mKernel->getVariant());
            if (T3D_FAILED(ret)) return ret;

            UnorderedAccessBuffers uavs;
            uavs.push_back(mOutput);
            ret = ctx->setCSUnorderedAccessBuffers(uavSlot(0, 0), uavs);
            if (T3D_FAILED(ret)) return ret;

            ret = mKernel->dispatchThreads(kElemCount);
            if (T3D_FAILED(ret))
            {
                APP_LOG_DEBUG("[%s] FAILED : dispatch of %u threads rejected, maxDispatchGroupCount.x=%u",
                    getName(), kElemCount, maxX);
                ctx->setComputeShader(nullptr);
                return ret;
            }

            ctx->uavBarrier(uavs);
            ctx->setComputeShader(nullptr);
            return mapWhole(mOutput.get(), mHandle);
        }

        TResult verify() override
        {
            Buffer data;
            TResult ret = unmapCheckSize(mOutput.get(), mHandle, data, sizeof(uint32_t) * kElemCount);
            if (T3D_FAILED(ret)) return ret;

            const uint32_t *out = reinterpret_cast<const uint32_t *>(data.Data);
            uint32_t mismatches = 0;
            uint32_t first = 0, got = 0;
            for (uint32_t i = 0; i < kElemCount; ++i)
            {
                if (out[i] != i * 2)
                {
                    if (mismatches == 0) { first = i; got = out[i]; }
                    ++mismatches;
                }
            }
            data.release();
            if (mismatches != 0)
            {
                APP_LOG_DEBUG("[%s] FAILED : %u mismatches, first [%u] got %u expected %u",
                    getName(), mismatches, first, got, first * 2);
                return T3D_ERR_FAIL;
            }
            return T3D_OK;
        }

    private:
        ComputeKernelPtr    mKernel;
        StructuredBufferPtr mOutput;
        ReadbackHandle      mHandle {};
    };


    /**
     * \brief CPU 写入的动态结构化缓冲：writeBuffer 填数据，compute 读 SRV 后写到 UAV
     * \remarks 先写一遍垃圾数据，再用 discard 方式写正确数据，结果必须是第二次的内容。
     *          D3D11 的 writeBuffer 曾漏掉 StructuredBuffer，走到断言后拿空指针 Map。
     */
    class CaseDynamicStructuredWrite : public ComputeCase
    {
    public:
        static const uint32_t kElemCount = 256;

        const char *getName() const override { return "K16 dynamic structured writeBuffer"; }
        uint32_t getRequiredCaps() const override
        {
            return kNeedCompute | kNeedUnorderedAccess | kNeedStructuredBuffer | kNeedReadback;
        }
        uint32_t getBackendMask() const override { return kBackendAll; }

        TResult setup() override
        {
            mKernel = ComputeKernel::create("ComputeStructuredRead", structuredReadSource(), 64);
            if (mKernel == nullptr) return T3D_ERR_FAIL;

            StructuredBufferDesc desc;
            desc.kind = StructuredBufferKind::kStructured;
            desc.elementSize = sizeof(uint32_t);
            desc.elementCount = kElemCount;
            mInput = T3D_RENDER_BUFFER_MGR.loadStructuredBuffer(
                desc, MemoryType::kVRAM, Usage::kDynamic, kCPUWrite, kGPUShaderResource);

            mOutput = loadSB(StructuredBufferKind::kStructured, sizeof(uint32_t), kElemCount,
                kGPUUnorderedAccess | kGPUShaderResource, kCPURead);
            return (mInput && mOutput) ? T3D_OK : T3D_ERR_FAIL;
        }

        TResult record() override
        {
            RHIContext *ctx = T3D_AGENT.getActiveRHIContext();

            Buffer data = allocBytes(sizeof(uint32_t) * kElemCount);
            uint32_t *v = reinterpret_cast<uint32_t *>(data.Data);
            for (uint32_t i = 0; i < kElemCount; ++i) v[i] = 0xDEADBEEFu;
            TResult ret = ctx->writeBuffer(mInput.get(), data, true);
            if (T3D_SUCCEEDED(ret))
            {
                for (uint32_t i = 0; i < kElemCount; ++i) v[i] = i * 7u + 3u;
                ret = ctx->writeBuffer(mInput.get(), data, true);
            }
            data.release();
            if (T3D_FAILED(ret))
            {
                APP_LOG_DEBUG("[%s] FAILED : writeBuffer on dynamic structured buffer returned %d", getName(), ret);
                return ret;
            }

            ret = ctx->setComputeShader(mKernel->getVariant());
            if (T3D_FAILED(ret)) return ret;

            ShaderResourceBuffers srvs;
            srvs.push_back(mInput);
            ret = ctx->setCSStructuredBuffers(0, srvs);
            if (T3D_FAILED(ret)) return ret;

            UnorderedAccessBuffers uavs;
            uavs.push_back(mOutput);
            ret = ctx->setCSUnorderedAccessBuffers(uavSlot(1, 0), uavs);
            if (T3D_FAILED(ret)) return ret;

            ret = mKernel->dispatchThreads(kElemCount);
            if (T3D_FAILED(ret)) return ret;

            ctx->uavBarrier(uavs);
            ShaderResourceBuffers emptySrvs(1, nullptr);
            ctx->setCSStructuredBuffers(0, emptySrvs);
            ctx->setComputeShader(nullptr);
            return mapWhole(mOutput.get(), mHandle);
        }

        TResult verify() override
        {
            Buffer data;
            TResult ret = unmapCheckSize(mOutput.get(), mHandle, data, sizeof(uint32_t) * kElemCount);
            if (T3D_FAILED(ret)) return ret;

            const uint32_t *out = reinterpret_cast<const uint32_t *>(data.Data);
            uint32_t mismatches = 0;
            uint32_t first = 0, got = 0, expectFirst = 0;
            for (uint32_t i = 0; i < kElemCount; ++i)
            {
                const uint32_t expect = (i * 7u + 3u) ^ 0x5A5A5A5Au;
                if (out[i] != expect)
                {
                    if (mismatches == 0) { first = i; got = out[i]; expectFirst = expect; }
                    ++mismatches;
                }
            }
            data.release();
            if (mismatches != 0)
            {
                APP_LOG_DEBUG("[%s] FAILED : %u mismatches, first [%u] got 0x%08X expected 0x%08X "
                    "(0xDEADBEEF ^ 0x5A5A5A5A = 0x84F7E4B5 means the second write was lost)",
                    getName(), mismatches, first, got, expectFirst);
                return T3D_ERR_FAIL;
            }
            return T3D_OK;
        }

    private:
        ComputeKernelPtr    mKernel;
        StructuredBufferPtr mInput;
        StructuredBufferPtr mOutput;
        ReadbackHandle      mHandle {};
    };


    // ---- 图形回归用例（K11～K14）：验证 GL4 后端行为级修复，判定同样靠 readback ----

    /// 读回像素的粗分类。只用黑 / 白 / 绿三种颜色，RGBA 与 BGRA 两种字节序下结论相同。
    enum class PixelClass : char { kBlack = 'K', kWhite = 'W', kGreen = 'G', kOther = '?' };

    PixelClass classifyPixel(const uint8_t *p)
    {
        auto hi = [](uint8_t v) { return v >= 250; };
        auto lo = [](uint8_t v) { return v <= 5; };
        if (hi(p[0]) && hi(p[1]) && hi(p[2])) return PixelClass::kWhite;
        if (lo(p[0]) && lo(p[1]) && lo(p[2])) return PixelClass::kBlack;
        if (lo(p[0]) && hi(p[1]) && lo(p[2])) return PixelClass::kGreen;
        return PixelClass::kOther;
    }

    /// 纹理读回的行距按 DataSize / height 推算，不假设各后端都紧密排列
    const uint8_t *pixelAt(const Buffer &data, uint32_t width, uint32_t height, uint32_t x, uint32_t y)
    {
        const size_t pitch = data.DataSize / height;
        if (pitch < size_t(width) * 4)
        {
            return nullptr;
        }
        return data.Data + y * pitch + x * 4;
    }

    RenderTexturePtr createColorRT(const char *name, uint32_t width, uint32_t height,
        PixelFormat format, uint32_t msaaCount, uint32_t cpuAccess, bool shaderReadable = false)
    {
        return T3D_TEXTURE_MGR.createRenderTexture(name, width, height, format,
            1, msaaCount, 0, shaderReadable, cpuAccess);
    }

    // createRenderTexture 建出的 RT 要调用方自己 unload，否则引擎退出时 unloadAllResources 会报 invalid state
    void releaseColorRT(RenderTexturePtr &tex, RenderTargetPtr &rt)
    {
        rt = nullptr;
        if (tex != nullptr)
        {
            T3D_TEXTURE_MGR.unload(tex);
            tex = nullptr;
        }
    }

    ReadbackHandle mapColorRT(RenderTexture *tex)
    {
        ReadbackRegion region;
        region.size = Vector3(Real(tex->getWidth()), Real(tex->getHeight()), Real(1));
        return T3D_AGENT.getActiveRHIContext()->map(tex->getPixelBuffer(), region);
    }

    TResult unmapColorRT(RenderTexture *tex, ReadbackHandle &handle, Buffer &data)
    {
        TResult ret = tex->getPixelBuffer()->unmap(handle, data);
        handle = ReadbackHandle::invalid();
        if (T3D_FAILED(ret))
        {
            return ret;
        }
        const size_t expected = size_t(tex->getWidth()) * tex->getHeight() * 4;
        if (data.Data == nullptr || data.DataSize < expected)
        {
            data.release();
            return T3D_ERR_FAIL;
        }
        return T3D_OK;
    }

    /**
     * \brief 画全屏三角形的最小管线：只读位置的 VS、纯绿与纯白两个 PS，关闭深度测试与剔除
     * \remarks VS 不用任何矩阵，NDC 直出，所以与 GL 画到 FBO 时的投影 Y 翻转无关。
     */
    class FullscreenDraw
    {
    public:
        TResult setup()
        {
            mVS = ComputeKernel::compileGraphics("FullscreenVS", SHADER_STAGE::kVertex,
                fullscreenVSSource(), mDummyVS);
            mGreenPS = ComputeKernel::compileGraphics("SolidGreenPS", SHADER_STAGE::kPixel,
                solidGreenPSSource(), mDummyGreen);
            mWhitePS = ComputeKernel::compileGraphics("SolidWhitePS", SHADER_STAGE::kPixel,
                solidWhitePSSource(), mDummyWhite);
            if (mVS == nullptr || mGreenPS == nullptr || mWhitePS == nullptr)
            {
                return T3D_ERR_FAIL;
            }

            VertexAttributes attribs;
            attribs.push_back(VertexAttribute(0, 0, VertexAttribute::Type::E_VAT_FLOAT3,
                VertexAttribute::Semantic::E_VAS_POSITION, 0));
            mDecl = T3D_RENDER_BUFFER_MGR.addVertexDeclaration(attribs, mVS.get());

            // 一个盖住整个 NDC 的大三角形
            const float verts[9] = { -1.0f, -1.0f, 0.0f,  3.0f, -1.0f, 0.0f,  -1.0f, 3.0f, 0.0f };
            Buffer vb = allocBytes(sizeof(verts));
            memcpy(vb.Data, verts, sizeof(verts));
            mVB = T3D_RENDER_BUFFER_MGR.loadVertexBuffer(sizeof(float) * 3, 3, vb,
                MemoryType::kBoth, Usage::kImmutable, kCPUNone);
            vb.release();

            RasterizerDesc raster;
            raster.CullMode = CullingMode::kNone;
            mRasterizer = T3D_RENDER_STATE_MGR.loadRasterizerState(raster);

            DepthStencilDesc depth;
            depth.DepthTestEnable = false;
            depth.DepthWriteEnable = false;
            mDepthState = T3D_RENDER_STATE_MGR.loadDepthStencilState(depth);

            BlendDesc blend;
            mBlendState = T3D_RENDER_STATE_MGR.loadBlendState(blend);

            return (mDecl && mVB && mRasterizer && mDepthState && mBlendState) ? T3D_OK : T3D_ERR_FAIL;
        }

        /// 绑定除 VS / PS 之外的全部管线状态。光栅化状态里 scissor 关闭，之后的清屏不受裁剪影响
        void bindStates(RHIContext *ctx)
        {
            ctx->setRasterizerState(mRasterizer.get());
            ctx->setDepthStencilState(mDepthState.get());
            ctx->setBlendState(mBlendState.get());
            ctx->setPrimitiveType(PrimitiveType::kTriangleList);
            ctx->setVertexDeclaration(mDecl.get());

            VertexBuffers vbs;
            vbs.push_back(mVB);
            VertexStrides strides;
            strides.push_back(sizeof(float) * 3);
            VertexOffsets offsets;
            offsets.push_back(0);
            ctx->setVertexBuffers(0, vbs, strides, offsets);
        }

        TResult draw(RHIContext *ctx) { return ctx->render(3, 0); }

        ShaderVariant *vs() const { return mVS.get(); }
        ShaderVariant *greenPS() const { return mGreenPS.get(); }
        ShaderVariant *whitePS() const { return mWhitePS.get(); }

    private:
        ShaderVariantPtr        mVS;
        ShaderVariantPtr        mGreenPS;
        ShaderVariantPtr        mWhitePS;
        ShaderPtr               mDummyVS;
        ShaderPtr               mDummyGreen;
        ShaderPtr               mDummyWhite;
        VertexDeclarationPtr    mDecl;
        VertexBufferPtr         mVB;
        RasterizerStatePtr      mRasterizer;
        DepthStencilStatePtr    mDepthState;
        BlendStatePtr           mBlendState;
    };

    const Viewport kFullViewport {0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};

    /// 深度值 [0, 1] 换成 FullscreenVS 直出的 NDC z。GL 的 NDC 深度是 [-1, 1]，经 glDepthRange(0, 1) 映射到窗口深度；D3D 直接取 z
    float ndcZ(float depth)
    {
        return isGLFamily() ? depth * 2.0f - 1.0f : depth;
    }

    /// 与 FullscreenDraw 相同的全屏三角形，z 取指定值
    VertexBufferPtr createFullscreenVB(float z)
    {
        const float verts[9] = { -1.0f, -1.0f, z,  3.0f, -1.0f, z,  -1.0f, 3.0f, z };
        Buffer vb = allocBytes(sizeof(verts));
        memcpy(vb.Data, verts, sizeof(verts));
        VertexBufferPtr buffer = T3D_RENDER_BUFFER_MGR.loadVertexBuffer(sizeof(float) * 3, 3, vb,
            MemoryType::kBoth, Usage::kImmutable, kCPUNone);
        vb.release();
        return buffer;
    }

    void bindVertexBuffer(RHIContext *ctx, VertexBuffer *vb)
    {
        VertexBuffers vbs;
        vbs.push_back(VertexBufferPtr(vb));
        VertexStrides strides;
        strides.push_back(sizeof(float) * 3);
        VertexOffsets offsets;
        offsets.push_back(0);
        ctx->setVertexBuffers(0, vbs, strides, offsets);
    }


    /**
     * \brief MSAA 源缩放 blit：4x MSAA 的 64x64 白色 RT 整张 blit 到 32x32 的单采样 RT
     * \remarks GL 规定多重采样读 FBO 的 blit 源 / 目标矩形必须一致，缩放会 GL_INVALID_OPERATION，
     *          目标保持清屏黑色。修复后后端先 resolve 到同尺寸的 resolve FBO 再缩放（D.3.1）。
     */
    class CaseMsaaScaledBlit : public ComputeCase
    {
    public:
        ~CaseMsaaScaledBlit() override
        {
            releaseColorRT(mSrc, mSrcRT);
            releaseColorRT(mDst, mDstRT);
        }

        static const uint32_t kSrcSize = 64;
        static const uint32_t kDstSize = 32;

        const char *getName() const override { return "K11 MSAA scaled blit"; }
        uint32_t getRequiredCaps() const override { return kNeedReadback; }
        uint32_t getBackendMask() const override { return kBackendD3D11 | kBackendGL4; }

        TResult setup() override
        {
            mSrc = createColorRT("K11-MSAA-Src", kSrcSize, kSrcSize, PixelFormat::E_PF_R8G8B8A8, 4, kCPUNone);
            mDst = createColorRT("K11-Dst", kDstSize, kDstSize, PixelFormat::E_PF_R8G8B8A8, 1, kCPURead);
            if (mSrc == nullptr || mDst == nullptr) return T3D_ERR_FAIL;
            mSrcRT = RenderTarget::create(mSrc);
            mDstRT = RenderTarget::create(mDst);
            return (mSrcRT && mDstRT) ? T3D_OK : T3D_ERR_FAIL;
        }

        TResult record() override
        {
            RHIContext *ctx = T3D_AGENT.getActiveRHIContext();

            ctx->setRenderTarget(mDstRT.get());
            ctx->setViewport(kFullViewport);
            ctx->clearColor(ColorRGB::BLACK);

            ctx->setRenderTarget(mSrcRT.get());
            ctx->setViewport(kFullViewport);
            ctx->clearColor(ColorRGB::WHITE);

            TResult ret = ctx->blit(mSrcRT.get(), mDstRT.get());
            if (T3D_SUCCEEDED(ret))
            {
                mHandle = mapColorRT(mDst.get());
                ret = mHandle.isValid() ? T3D_OK : T3D_ERR_FAIL;
            }
            ctx->reset();
            return ret;
        }

        TResult verify() override
        {
            Buffer data;
            TResult ret = unmapColorRT(mDst.get(), mHandle, data);
            if (T3D_FAILED(ret)) return ret;

            uint32_t bad = 0;
            PixelClass firstBad = PixelClass::kWhite;
            for (uint32_t y = 0; y < kDstSize; ++y)
            {
                for (uint32_t x = 0; x < kDstSize; ++x)
                {
                    const uint8_t *p = pixelAt(data, kDstSize, kDstSize, x, y);
                    const PixelClass c = (p != nullptr) ? classifyPixel(p) : PixelClass::kOther;
                    if (c != PixelClass::kWhite)
                    {
                        if (bad == 0) firstBad = c;
                        ++bad;
                    }
                }
            }
            data.release();

            if (bad != 0)
            {
                APP_LOG_DEBUG("[%s] FAILED : %u / %u pixels not white, first got '%c' "
                    "('K' means the blit did not happen)", getName(), bad, kDstSize * kDstSize, char(firstBad));
                return T3D_ERR_FAIL;
            }
            return T3D_OK;
        }

    private:
        RenderTexturePtr    mSrc;
        RenderTexturePtr    mDst;
        RenderTargetPtr     mSrcRT;
        RenderTargetPtr     mDstRT;
        ReadbackHandle      mHandle {};
    };


    /**
     * \brief 着色器设置顺序与 Program 缓存：64x8 的 RT 按 4 条竖带分别绘制
     * \remarks 竖带 0：先设 PS 再设 VS（绿）。竖带 1：只换 PS（白），走新组合。
     *          竖带 2：绑回绿色 PS 后 setVertexShader(nullptr) 再重设 VS，PS 必须保留，
     *          命中竖带 0 的缓存组合（绿）；旧实现会删掉整个 Program，只剩 VS，输出未定义。
     *          竖带 3：不绘制，保持清屏黑色，确认视口限制了绘制范围。
     */
    class CaseShaderBindOrder : public ComputeCase
    {
    public:
        ~CaseShaderBindOrder() override
        {
            releaseColorRT(mTex, mRT);
        }

        static const uint32_t kWidth = 64;
        static const uint32_t kHeight = 8;
        static const uint32_t kStripes = 4;

        const char *getName() const override { return "K12 shader bind order"; }
        uint32_t getRequiredCaps() const override { return kNeedReadback; }
        uint32_t getBackendMask() const override { return kBackendD3D11 | kBackendGL4; }

        TResult setup() override
        {
            if (T3D_FAILED(mDraw.setup())) return T3D_ERR_FAIL;
            mTex = createColorRT("K12-Target", kWidth, kHeight, PixelFormat::E_PF_R8G8B8A8, 1, kCPURead);
            if (mTex == nullptr) return T3D_ERR_FAIL;
            mRT = RenderTarget::create(mTex);
            return mRT != nullptr ? T3D_OK : T3D_ERR_FAIL;
        }

        TResult record() override
        {
            RHIContext *ctx = T3D_AGENT.getActiveRHIContext();

            ctx->setRenderTarget(mRT.get());
            mDraw.bindStates(ctx);
            ctx->setViewport(kFullViewport);
            ctx->clearColor(ColorRGB::BLACK);
            ctx->beginPass();

            TResult ret = T3D_OK;
            do
            {
                ctx->setPixelShader(mDraw.greenPS());
                ctx->setVertexShader(mDraw.vs());
                ctx->setViewport(stripe(0));
                ret = mDraw.draw(ctx);
                if (T3D_FAILED(ret)) break;

                ctx->setPixelShader(mDraw.whitePS());
                ctx->setViewport(stripe(1));
                ret = mDraw.draw(ctx);
                if (T3D_FAILED(ret)) break;

                ctx->setPixelShader(mDraw.greenPS());
                ctx->setVertexShader(nullptr);
                ctx->setVertexShader(mDraw.vs());
                ctx->setViewport(stripe(2));
                ret = mDraw.draw(ctx);
            } while (false);

            ctx->endPass();

            if (T3D_SUCCEEDED(ret))
            {
                mHandle = mapColorRT(mTex.get());
                ret = mHandle.isValid() ? T3D_OK : T3D_ERR_FAIL;
            }
            ctx->reset();
            return ret;
        }

        TResult verify() override
        {
            Buffer data;
            TResult ret = unmapColorRT(mTex.get(), mHandle, data);
            if (T3D_FAILED(ret)) return ret;

            const PixelClass expected[kStripes] =
            {
                PixelClass::kGreen, PixelClass::kWhite, PixelClass::kGreen, PixelClass::kBlack
            };
            const uint32_t stripeW = kWidth / kStripes;

            uint32_t bad = 0;
            char got[kStripes + 1] = {};
            for (uint32_t s = 0; s < kStripes; ++s)
            {
                // 只看竖带内部，避开相邻竖带的边界像素
                const uint32_t x0 = s * stripeW + 2;
                const uint32_t x1 = (s + 1) * stripeW - 2;
                got[s] = '?';
                for (uint32_t y = 0; y < kHeight; ++y)
                {
                    for (uint32_t x = x0; x < x1; ++x)
                    {
                        const uint8_t *p = pixelAt(data, kWidth, kHeight, x, y);
                        const PixelClass c = (p != nullptr) ? classifyPixel(p) : PixelClass::kOther;
                        if (x == x0 && y == 0) got[s] = char(c);
                        if (c != expected[s]) ++bad;
                    }
                }
            }
            data.release();

            if (bad != 0)
            {
                APP_LOG_DEBUG("[%s] FAILED : stripes got \"%s\" expected \"GWGK\" (%u pixels mismatch)",
                    getName(), got, bad);
                return T3D_ERR_FAIL;
            }
            return T3D_OK;
        }

    private:
        static Viewport stripe(uint32_t index)
        {
            const Real w = Real(1) / Real(kStripes);
            return Viewport {w * Real(index), 0.0f, w, 1.0f, 0.0f, 1.0f};
        }

        FullscreenDraw      mDraw;
        RenderTexturePtr    mTex;
        RenderTargetPtr     mRT;
        ReadbackHandle      mHandle {};
    };


    /**
     * \brief 非全屏视口直接画到窗口：视口取左上 1/4，画白色，再把窗口 blit 到 RT 读回
     * \remarks Viewport::Top 是左上原点的比例，窗口（默认 FBO）是左下原点，GL 需要换算。
     *          旧实现直接用 Top * height，白块会落到左下角（D.1.3）。
     *          用例会在窗口上清屏并绘制，自检那一帧画面会闪一下，属正常现象。
     */
    class CaseWindowViewport : public ComputeCase
    {
    public:
        ~CaseWindowViewport() override
        {
            releaseColorRT(mCapture, mCaptureRT);
        }

        const char *getName() const override { return "K13 window viewport"; }
        uint32_t getRequiredCaps() const override { return kNeedReadback; }
        uint32_t getBackendMask() const override { return kBackendD3D11 | kBackendGL4; }

        TResult setup() override
        {
            if (T3D_FAILED(mDraw.setup())) return T3D_ERR_FAIL;

            RenderWindowPtr rw = T3D_AGENT.getDefaultRenderWindow();
            if (rw == nullptr) return T3D_ERR_FAIL;
            mWindowRT = RenderTarget::create(rw);

            // 与相机离屏 RT 同格式（T3DCamera.cpp 按 32 位色深选 B8G8R8A8），窗口 blit 过来不需要格式转换
            mWidth = rw->getDescriptor().Width;
            mHeight = rw->getDescriptor().Height;
            mCapture = createColorRT("K13-Capture", mWidth, mHeight, PixelFormat::E_PF_B8G8R8A8, 1, kCPURead);
            if (mWindowRT == nullptr || mCapture == nullptr) return T3D_ERR_FAIL;
            mCaptureRT = RenderTarget::create(mCapture);
            return mCaptureRT != nullptr ? T3D_OK : T3D_ERR_FAIL;
        }

        TResult record() override
        {
            RHIContext *ctx = T3D_AGENT.getActiveRHIContext();

            ctx->setRenderTarget(mWindowRT.get());
            mDraw.bindStates(ctx);
            ctx->setViewport(kFullViewport);
            ctx->clearColor(ColorRGB::BLACK);
            ctx->beginPass();

            ctx->setVertexShader(mDraw.vs());
            ctx->setPixelShader(mDraw.whitePS());
            ctx->setViewport(Viewport {0.0f, 0.0f, 0.5f, 0.5f, 0.0f, 1.0f});
            TResult ret = mDraw.draw(ctx);

            ctx->endPass();

            if (T3D_SUCCEEDED(ret))
            {
                ret = ctx->blit(mWindowRT.get(), mCaptureRT.get());
            }
            if (T3D_SUCCEEDED(ret))
            {
                mHandle = mapColorRT(mCapture.get());
                ret = mHandle.isValid() ? T3D_OK : T3D_ERR_FAIL;
            }
            ctx->reset();
            return ret;
        }

        TResult verify() override
        {
            Buffer data;
            TResult ret = unmapColorRT(mCapture.get(), mHandle, data);
            if (T3D_FAILED(ret)) return ret;

            // 四个象限中心各取一个 3x3 小块；读回纹理 row 0 是画面顶部
            struct Probe { uint32_t x; uint32_t y; PixelClass expect; const char *name; };
            const Probe probes[4] =
            {
                { mWidth / 4,     mHeight / 4,     PixelClass::kWhite, "top-left"     },
                { mWidth * 3 / 4, mHeight / 4,     PixelClass::kBlack, "top-right"    },
                { mWidth / 4,     mHeight * 3 / 4, PixelClass::kBlack, "bottom-left"  },
                { mWidth * 3 / 4, mHeight * 3 / 4, PixelClass::kBlack, "bottom-right" },
            };

            uint32_t bad = 0;
            for (const Probe &probe : probes)
            {
                PixelClass got = PixelClass::kOther;
                bool ok = true;
                for (uint32_t dy = 0; dy < 3; ++dy)
                {
                    for (uint32_t dx = 0; dx < 3; ++dx)
                    {
                        const uint8_t *p = pixelAt(data, mWidth, mHeight, probe.x + dx - 1, probe.y + dy - 1);
                        got = (p != nullptr) ? classifyPixel(p) : PixelClass::kOther;
                        ok = ok && (got == probe.expect);
                    }
                }
                if (!ok)
                {
                    APP_LOG_DEBUG("[%s] FAILED : %s quadrant got '%c' expected '%c'",
                        getName(), probe.name, char(got), char(probe.expect));
                    ++bad;
                }
            }
            data.release();
            return (bad == 0) ? T3D_OK : T3D_ERR_FAIL;
        }

    private:
        FullscreenDraw      mDraw;
        RenderTargetPtr     mWindowRT;
        RenderTexturePtr    mCapture;
        RenderTargetPtr     mCaptureRT;
        uint32_t            mWidth {0};
        uint32_t            mHeight {0};
        ReadbackHandle      mHandle {};
    };


    /**
     * \brief 窗口目标外挂深度：同一张深度先在纹理目标里写好上半屏，再挂到窗口上做深度测试
     * \remarks 依次：① 窗口（不挂深度）清黑，左半屏画白；② 纹理 RT 挂这张深度，深度清 1.0，
     *          上半屏以深度 0.25 写入；③ 窗口挂这张深度，全屏以深度 0.5、LESS 画绿，不写深度。
     *          期望左上白、右上黑、下半屏绿。① 检查切到外挂深度后窗口原有内容仍在，
     *          ② 检查外部深度在窗口与纹理目标之间上下方向一致。外部深度与窗口同为窗口的 MSAA 采样数。
     *          GL 的默认帧缓冲不能挂外部附件，后端曾直接忽略外部深度、改用窗口自带深度（E.1.4）。
     *          用例会在窗口上清屏并绘制，自检那一帧画面会闪一下，属正常现象。
     */
    class CaseWindowExternalDepth : public ComputeCase
    {
    public:
        ~CaseWindowExternalDepth() override
        {
            releaseColorRT(mCapture, mCaptureRT);
            releaseColorRT(mPrepassColor, mPrepassRT);
            mWindowDepthRT = nullptr;
            RenderTargetPtr none;
            releaseColorRT(mDepth, none);
        }

        const char *getName() const override { return "K19 window external depth"; }
        uint32_t getRequiredCaps() const override { return kNeedReadback; }
        uint32_t getBackendMask() const override { return kBackendD3D11 | kBackendGL4; }

        TResult setup() override
        {
            if (T3D_FAILED(mDraw.setup())) return T3D_ERR_FAIL;

            RenderWindowPtr rw = T3D_AGENT.getDefaultRenderWindow();
            if (rw == nullptr) return T3D_ERR_FAIL;

            mWidth = rw->getDescriptor().Width;
            mHeight = rw->getDescriptor().Height;
            const uint32_t samples = std::max<uint32_t>(rw->getDescriptor().MSAA.Count, 1);

            mDepth = T3D_TEXTURE_MGR.createRenderTexture("K19-Depth", mWidth, mHeight,
                PixelFormat::E_PF_D24_UNORM_S8_UINT, 1, samples, 0, false, kCPUNone);
            mPrepassColor = createColorRT("K19-Prepass", mWidth, mHeight, PixelFormat::E_PF_R8G8B8A8, samples, kCPUNone);
            mCapture = createColorRT("K19-Capture", mWidth, mHeight, PixelFormat::E_PF_B8G8R8A8, 1, kCPURead);
            if (mDepth == nullptr || mPrepassColor == nullptr || mCapture == nullptr) return T3D_ERR_FAIL;

            mWindowRT = RenderTarget::create(rw);
            mWindowDepthRT = RenderTarget::create(rw, mDepth);
            mPrepassRT = RenderTarget::create(mPrepassColor, mDepth);
            mCaptureRT = RenderTarget::create(mCapture);

            mNearVB = createFullscreenVB(ndcZ(0.25f));
            mMidVB = createFullscreenVB(ndcZ(0.5f));

            DepthStencilDesc write;
            write.DepthWriteEnable = true;
            write.DepthFunc = CompareFunction::kAlwaysPass;
            mDepthWrite = T3D_RENDER_STATE_MGR.loadDepthStencilState(write);

            DepthStencilDesc test;
            test.DepthWriteEnable = false;
            test.DepthFunc = CompareFunction::kLess;
            mDepthTest = T3D_RENDER_STATE_MGR.loadDepthStencilState(test);

            return (mWindowRT && mWindowDepthRT && mPrepassRT && mCaptureRT && mNearVB && mMidVB
                && mDepthWrite && mDepthTest) ? T3D_OK : T3D_ERR_FAIL;
        }

        TResult record() override
        {
            RHIContext *ctx = T3D_AGENT.getActiveRHIContext();
            TResult ret = T3D_OK;

            do
            {
                ctx->setRenderTarget(mWindowRT.get());
                mDraw.bindStates(ctx);
                ctx->setViewport(kFullViewport);
                ctx->clearColor(ColorRGB::BLACK);
                ctx->beginPass();
                ctx->setVertexShader(mDraw.vs());
                ctx->setPixelShader(mDraw.whitePS());
                ctx->setViewport(Viewport {0.0f, 0.0f, 0.5f, 1.0f, 0.0f, 1.0f});
                ret = mDraw.draw(ctx);
                ctx->endPass();
                if (T3D_FAILED(ret)) break;

                ctx->setRenderTarget(mPrepassRT.get());
                mDraw.bindStates(ctx);
                ctx->setViewport(kFullViewport);
                ctx->clearColor(ColorRGB::BLACK);
                ctx->clearDepthStencil(1.0f, 0);
                ctx->beginPass();
                ctx->setDepthStencilState(mDepthWrite.get());
                bindVertexBuffer(ctx, mNearVB.get());
                ctx->setVertexShader(mDraw.vs());
                ctx->setPixelShader(mDraw.whitePS());
                ctx->setViewport(Viewport {0.0f, 0.0f, 1.0f, 0.5f, 0.0f, 1.0f});
                ret = mDraw.draw(ctx);
                ctx->endPass();
                if (T3D_FAILED(ret)) break;

                ret = ctx->setRenderTarget(mWindowDepthRT.get());
                if (T3D_FAILED(ret))
                {
                    APP_LOG_DEBUG("[%s] FAILED : setRenderTarget(window + depth) returned %d", getName(), ret);
                    break;
                }
                mDraw.bindStates(ctx);
                ctx->setDepthStencilState(mDepthTest.get());
                bindVertexBuffer(ctx, mMidVB.get());
                ctx->setViewport(kFullViewport);
                ctx->beginPass();
                ctx->setVertexShader(mDraw.vs());
                ctx->setPixelShader(mDraw.greenPS());
                ret = mDraw.draw(ctx);
                ctx->endPass();
                if (T3D_FAILED(ret)) break;

                ret = ctx->blit(mWindowRT.get(), mCaptureRT.get());
                if (T3D_FAILED(ret)) break;

                mHandle = mapColorRT(mCapture.get());
                ret = mHandle.isValid() ? T3D_OK : T3D_ERR_FAIL;
            } while (false);

            ctx->reset();
            return ret;
        }

        TResult verify() override
        {
            Buffer data;
            TResult ret = unmapColorRT(mCapture.get(), mHandle, data);
            if (T3D_FAILED(ret)) return ret;

            struct Probe { uint32_t x; uint32_t y; PixelClass expect; const char *name; };
            const Probe probes[4] =
            {
                { mWidth / 4,     mHeight / 4,     PixelClass::kWhite, "top-left"     },
                { mWidth * 3 / 4, mHeight / 4,     PixelClass::kBlack, "top-right"    },
                { mWidth / 4,     mHeight * 3 / 4, PixelClass::kGreen, "bottom-left"  },
                { mWidth * 3 / 4, mHeight * 3 / 4, PixelClass::kGreen, "bottom-right" },
            };

            uint32_t bad = 0;
            for (const Probe &probe : probes)
            {
                const uint8_t *p = pixelAt(data, mWidth, mHeight, probe.x, probe.y);
                const PixelClass got = (p != nullptr) ? classifyPixel(p) : PixelClass::kOther;
                if (got != probe.expect)
                {
                    APP_LOG_DEBUG("[%s] FAILED : %s quadrant got '%c' expected '%c'",
                        getName(), probe.name, char(got), char(probe.expect));
                    ++bad;
                }
            }
            data.release();
            return (bad == 0) ? T3D_OK : T3D_ERR_FAIL;
        }

    private:
        FullscreenDraw          mDraw;
        RenderTargetPtr         mWindowRT;
        RenderTargetPtr         mWindowDepthRT;
        RenderTexturePtr        mDepth;
        RenderTexturePtr        mPrepassColor;
        RenderTargetPtr         mPrepassRT;
        RenderTexturePtr        mCapture;
        RenderTargetPtr         mCaptureRT;
        VertexBufferPtr         mNearVB;
        VertexBufferPtr         mMidVB;
        DepthStencilStatePtr    mDepthWrite;
        DepthStencilStatePtr    mDepthTest;
        uint32_t                mWidth {0};
        uint32_t                mHeight {0};
        ReadbackHandle          mHandle {};
    };


    /**
     * \brief 采样器 BorderColor 的通道顺序：BorderColor 设为纯红，在纹理范围外采样
     * \remarks 64x8 的 RT 分 4 条竖带：Border 采样、纯红对照、纯蓝对照、纹理中心采样（源纹理清成绿色）。
     *          Border 竖带的读回字节必须与纯红对照一致；R/B 被交换时会与纯蓝对照一致（D.1.4）。
     *          与对照竖带比字节，不依赖 RT 的字节序。中心采样为绿，确认纹理与采样器确实绑上了。
     */
    class CaseSamplerBorderColor : public ComputeCase
    {
    public:
        ~CaseSamplerBorderColor() override
        {
            releaseColorRT(mSrcTex, mSrcRT);
            releaseColorRT(mDstTex, mDstRT);
        }

        static const uint32_t kWidth = 64;
        static const uint32_t kHeight = 8;
        static const uint32_t kStripes = 4;

        const char *getName() const override { return "K14 sampler border color"; }
        uint32_t getRequiredCaps() const override { return kNeedReadback; }
        uint32_t getBackendMask() const override { return kBackendD3D11 | kBackendGL4; }

        TResult setup() override
        {
            if (T3D_FAILED(mDraw.setup())) return T3D_ERR_FAIL;

            mOutsidePS = ComputeKernel::compileGraphics("SampleOutsidePS", SHADER_STAGE::kPixel,
                sampleOutsidePSSource(), mDummyOutside);
            mCenterPS = ComputeKernel::compileGraphics("SampleCenterPS", SHADER_STAGE::kPixel,
                sampleCenterPSSource(), mDummyCenter);
            mRedPS = ComputeKernel::compileGraphics("SolidRedPS", SHADER_STAGE::kPixel,
                solidRedPSSource(), mDummyRed);
            mBluePS = ComputeKernel::compileGraphics("SolidBluePS", SHADER_STAGE::kPixel,
                solidBluePSSource(), mDummyBlue);
            if (mOutsidePS == nullptr || mCenterPS == nullptr || mRedPS == nullptr || mBluePS == nullptr)
            {
                return T3D_ERR_FAIL;
            }

            SamplerDesc desc;
            desc.MinFilter = FilterOptions::kPoint;
            desc.MagFilter = FilterOptions::kPoint;
            desc.MipFilter = FilterOptions::kPoint;
            desc.AddressU = TextureAddressMode::kBorder;
            desc.AddressV = TextureAddressMode::kBorder;
            desc.AddressW = TextureAddressMode::kBorder;
            desc.BorderColor = ColorRGBA(1.0f, 0.0f, 0.0f, 1.0f);
            mSampler = T3D_RENDER_STATE_MGR.loadSamplerState(desc);

            mSrcTex = createColorRT("K14-Src", 4, 4, PixelFormat::E_PF_R8G8B8A8, 1, kCPUNone, true);
            mDstTex = createColorRT("K14-Dst", kWidth, kHeight, PixelFormat::E_PF_R8G8B8A8, 1, kCPURead);
            if (mSampler == nullptr || mSrcTex == nullptr || mDstTex == nullptr) return T3D_ERR_FAIL;
            mSrcRT = RenderTarget::create(mSrcTex);
            mDstRT = RenderTarget::create(mDstTex);
            return (mSrcRT && mDstRT) ? T3D_OK : T3D_ERR_FAIL;
        }

        TResult record() override
        {
            RHIContext *ctx = T3D_AGENT.getActiveRHIContext();

            ctx->setRenderTarget(mSrcRT.get());
            ctx->setViewport(kFullViewport);
            ctx->clearColor(ColorRGB::GREEN);

            ctx->setRenderTarget(mDstRT.get());
            mDraw.bindStates(ctx);
            ctx->setViewport(kFullViewport);
            ctx->clearColor(ColorRGB::BLACK);
            ctx->beginPass();

            PixelBuffers textures;
            textures.push_back(mSrcTex->getPixelBuffer());
            Samplers samplers;
            samplers.push_back(mSampler);

            ShaderVariant *const stripePS[kStripes] = { mOutsidePS.get(), mRedPS.get(), mBluePS.get(), mCenterPS.get() };
            TResult ret = T3D_OK;
            ctx->setVertexShader(mDraw.vs());
            for (uint32_t s = 0; s < kStripes && T3D_SUCCEEDED(ret); ++s)
            {
                ctx->setPixelShader(stripePS[s]);
                ctx->setPSPixelBuffers(0, textures);
                ctx->setPSSamplers(0, samplers);
                ctx->setViewport(stripe(s));
                ret = mDraw.draw(ctx);
            }

            ctx->endPass();

            if (T3D_SUCCEEDED(ret))
            {
                mHandle = mapColorRT(mDstTex.get());
                ret = mHandle.isValid() ? T3D_OK : T3D_ERR_FAIL;
            }
            ctx->reset();
            return ret;
        }

        TResult verify() override
        {
            Buffer data;
            TResult ret = unmapColorRT(mDstTex.get(), mHandle, data);
            if (T3D_FAILED(ret)) return ret;

            enum { kOutside = 0, kRed, kBlue, kCenter };
            const char *names[kStripes] = { "border", "red control", "blue control", "center" };
            const uint32_t stripeW = kWidth / kStripes;

            // 每条竖带取内部第一个像素作代表，再要求竖带内部所有像素与之一致
            uint8_t rep[kStripes][4] = {};
            bool uniform = true;
            for (uint32_t s = 0; s < kStripes; ++s)
            {
                const uint32_t x0 = s * stripeW + 2;
                const uint32_t x1 = (s + 1) * stripeW - 2;
                const uint8_t *first = pixelAt(data, kWidth, kHeight, x0, 0);
                if (first == nullptr)
                {
                    data.release();
                    return T3D_ERR_FAIL;
                }
                memcpy(rep[s], first, 4);
                for (uint32_t y = 0; y < kHeight; ++y)
                {
                    for (uint32_t x = x0; x < x1; ++x)
                    {
                        if (!samePixel(pixelAt(data, kWidth, kHeight, x, y), rep[s]))
                        {
                            if (uniform)
                            {
                                APP_LOG_DEBUG("[%s] FAILED : %s stripe is not uniform at (%u, %u)", getName(), names[s], x, y);
                            }
                            uniform = false;
                        }
                    }
                }
            }
            data.release();
            if (!uniform) return T3D_ERR_FAIL;

            if (samePixel(rep[kRed], rep[kBlue]))
            {
                APP_LOG_DEBUG("[%s] FAILED : red and blue controls read back identical %s", getName(), hex(rep[kRed]).c_str());
                return T3D_ERR_FAIL;
            }
            if (classifyPixel(rep[kCenter]) != PixelClass::kGreen)
            {
                APP_LOG_DEBUG("[%s] FAILED : center sample got %s expected green, texture or sampler not bound",
                    getName(), hex(rep[kCenter]).c_str());
                return T3D_ERR_FAIL;
            }
            if (samePixel(rep[kOutside], rep[kRed]))
            {
                return T3D_OK;
            }
            if (samePixel(rep[kOutside], rep[kBlue]))
            {
                APP_LOG_DEBUG("[%s] FAILED : border sample matches the blue control, BorderColor R/B are swapped", getName());
            }
            else
            {
                APP_LOG_DEBUG("[%s] FAILED : border sample got %s, red control %s, blue control %s", getName(),
                    hex(rep[kOutside]).c_str(), hex(rep[kRed]).c_str(), hex(rep[kBlue]).c_str());
            }
            return T3D_ERR_FAIL;
        }

    private:
        static Viewport stripe(uint32_t index)
        {
            const Real w = Real(1) / Real(kStripes);
            return Viewport {w * Real(index), 0.0f, w, 1.0f, 0.0f, 1.0f};
        }

        static bool samePixel(const uint8_t *a, const uint8_t *b)
        {
            if (a == nullptr || b == nullptr) return false;
            for (int i = 0; i < 4; ++i)
            {
                if (std::abs(int(a[i]) - int(b[i])) > 2) return false;
            }
            return true;
        }

        static String hex(const uint8_t *p)
        {
            char buf[16] = {};
            snprintf(buf, sizeof(buf), "%02X%02X%02X%02X", p[0], p[1], p[2], p[3]);
            return String(buf);
        }

        FullscreenDraw      mDraw;
        ShaderVariantPtr    mOutsidePS;
        ShaderVariantPtr    mCenterPS;
        ShaderVariantPtr    mRedPS;
        ShaderVariantPtr    mBluePS;
        ShaderPtr           mDummyOutside;
        ShaderPtr           mDummyCenter;
        ShaderPtr           mDummyRed;
        ShaderPtr           mDummyBlue;
        SamplerStatePtr     mSampler;
        RenderTexturePtr    mSrcTex;
        RenderTargetPtr     mSrcRT;
        RenderTexturePtr    mDstTex;
        RenderTargetPtr     mDstRT;
        ReadbackHandle      mHandle {};
    };


    /**
     * \brief 深度/模板 RT：四种深度格式各测 shaderReadable = false / true，共 8 组
     * \remarks 每组 64x8 的颜色 RT 外挂该格式的深度 RT，深度清成 0.5，全程不写深度，分 4 条竖带：
     *          竖带 0：深度 0.75 画白，LESS 不通过（黑）。竖带 1：深度 0.25 画绿，通过（绿）。
     *          竖带 2：关深度测试，模板 EQUAL 清屏值画绿（绿）。竖带 3：模板 EQUAL 另一个值画白（黑）。
     *          无模板的格式不画竖带 2、3。shaderReadable 的组再把深度纹理采样到 4x4 的颜色 RT，R 应约为 128。
     *          深度 RT 没建出存储时 FBO / DSV 无效，竖带 1 读回不是绿：GL 的 D32FS8 曾因像素类型映射错误
     *          创建失败（E.1.2），D3D11 的 D32FS8 + shaderReadable 曾用视图格式建资源而失败。
     */
    class CaseDepthStencilFormats : public ComputeCase
    {
    public:
        ~CaseDepthStencilFormats() override
        {
            for (Group &g : mGroups)
            {
                RenderTargetPtr none;
                releaseColorRT(g.color, g.rt);
                releaseColorRT(g.depth, none);
                releaseColorRT(g.sample, g.sampleRT);
            }
        }

        static const uint32_t kWidth = 64;
        static const uint32_t kHeight = 8;
        static const uint32_t kStripes = 4;
        static const uint32_t kSampleSize = 4;
        static const uint32_t kStencilClear = 3;

        const char *getName() const override { return "K17 depth stencil formats"; }
        uint32_t getRequiredCaps() const override { return kNeedReadback; }
        uint32_t getBackendMask() const override { return kBackendD3D11 | kBackendGL4; }

        TResult setup() override
        {
            if (T3D_FAILED(mDraw.setup())) return T3D_ERR_FAIL;

            mSamplePS = ComputeKernel::compileGraphics("SampleDepthPS", SHADER_STAGE::kPixel,
                sampleCenterPSSource(), mDummySample);
            if (mSamplePS == nullptr) return T3D_ERR_FAIL;

            // GL 的 NDC 深度是 [-1, 1]，经 glDepthRange(0, 1) 映射到窗口深度；D3D 直接取 z
            mNearVB = createFullscreenVB(ndcZ(0.25f));
            mFarVB = createFullscreenVB(ndcZ(0.75f));

            DepthStencilDesc less;
            less.DepthWriteEnable = false;
            less.DepthFunc = CompareFunction::kLess;
            mDepthLess = T3D_RENDER_STATE_MGR.loadDepthStencilState(less);
            mStencilPass = loadStencilEqual(kStencilClear);
            mStencilFail = loadStencilEqual(kStencilClear + 2);

            SamplerDesc sampler;
            sampler.MinFilter = FilterOptions::kPoint;
            sampler.MagFilter = FilterOptions::kPoint;
            sampler.MipFilter = FilterOptions::kPoint;
            mSampler = T3D_RENDER_STATE_MGR.loadSamplerState(sampler);

            if (!mNearVB || !mFarVB || !mDepthLess || !mStencilPass || !mStencilFail || !mSampler)
            {
                return T3D_ERR_FAIL;
            }

            struct FormatInfo { PixelFormat format; bool hasStencil; const char *name; };
            const FormatInfo formats[] =
            {
                { PixelFormat::E_PF_D24_UNORM_S8_UINT,    true,  "D24S8"  },
                { PixelFormat::E_PF_D32_FLOAT,            false, "D32F"   },
                { PixelFormat::E_PF_D32_FLOAT_S8X24_UINT, true,  "D32FS8" },
                { PixelFormat::E_PF_D16_UNORM,            false, "D16"    },
            };

            for (const FormatInfo &f : formats)
            {
                for (bool readable : { false, true })
                {
                    Group g;
                    g.format = f.format;
                    g.hasStencil = f.hasStencil;
                    g.readable = readable;
                    g.label = String(f.name) + (readable ? " readable" : "");

                    const String prefix = "K17-" + String(f.name) + (readable ? "-R" : "");
                    g.color = createColorRT((prefix + "-Color").c_str(), kWidth, kHeight,
                        PixelFormat::E_PF_R8G8B8A8, 1, kCPURead);
                    g.depth = T3D_TEXTURE_MGR.createRenderTexture(prefix + "-Depth", kWidth, kHeight,
                        f.format, 1, 1, 0, readable, kCPUNone);
                    if (g.color == nullptr || g.depth == nullptr)
                    {
                        APP_LOG_DEBUG("[%s] FAILED : %s create render texture failed", getName(), g.label.c_str());
                        mGroups.push_back(std::move(g));
                        return T3D_ERR_FAIL;
                    }
                    g.rt = RenderTarget::create(g.color, g.depth);

                    if (readable)
                    {
                        g.sample = createColorRT((prefix + "-Sample").c_str(), kSampleSize, kSampleSize,
                            PixelFormat::E_PF_R8G8B8A8, 1, kCPURead);
                        if (g.sample == nullptr)
                        {
                            mGroups.push_back(std::move(g));
                            return T3D_ERR_FAIL;
                        }
                        g.sampleRT = RenderTarget::create(g.sample);
                    }

                    const bool ok = g.rt != nullptr && (!readable || g.sampleRT != nullptr);
                    mGroups.push_back(std::move(g));
                    if (!ok) return T3D_ERR_FAIL;
                }
            }

            return T3D_OK;
        }

        TResult record() override
        {
            RHIContext *ctx = T3D_AGENT.getActiveRHIContext();

            TResult ret = T3D_OK;
            for (Group &g : mGroups)
            {
                TResult r = recordGroup(ctx, g);
                if (T3D_FAILED(r))
                {
                    APP_LOG_DEBUG("[%s] FAILED : %s record failed", getName(), g.label.c_str());
                    if (T3D_SUCCEEDED(ret)) ret = r;
                }
            }
            ctx->reset();
            return ret;
        }

        TResult verify() override
        {
            // 每组的 readback 都要 unmap，不能在第一处失败就返回
            uint32_t bad = 0;
            for (Group &g : mGroups)
            {
                if (!verifyStripes(g)) ++bad;
                if (g.readable && !verifySample(g)) ++bad;
            }
            return (bad == 0) ? T3D_OK : T3D_ERR_FAIL;
        }

    private:
        struct Group
        {
            PixelFormat         format {PixelFormat::E_PF_UNKNOWN};
            bool                hasStencil {false};
            bool                readable {false};
            String              label;
            RenderTexturePtr    color;
            RenderTexturePtr    depth;
            RenderTargetPtr     rt;
            RenderTexturePtr    sample;
            RenderTargetPtr     sampleRT;
            ReadbackHandle      colorHandle {};
            ReadbackHandle      sampleHandle {};
        };

        static DepthStencilStatePtr loadStencilEqual(uint32_t ref)
        {
            DepthStencilDesc desc;
            desc.DepthTestEnable = false;
            desc.DepthWriteEnable = false;
            desc.StencilEnable = true;
            desc.StencilRef = ref;
            desc.FrontFace.StencilFunc = CompareFunction::kEqual;
            desc.BackFace.StencilFunc = CompareFunction::kEqual;
            return T3D_RENDER_STATE_MGR.loadDepthStencilState(desc);
        }

        static Viewport stripe(uint32_t index)
        {
            const Real w = Real(1) / Real(kStripes);
            return Viewport {w * Real(index), 0.0f, w, 1.0f, 0.0f, 1.0f};
        }

        TResult drawStripe(RHIContext *ctx, uint32_t index, DepthStencilState *state,
            VertexBuffer *vb, ShaderVariant *ps)
        {
            ctx->setDepthStencilState(state);
            bindVertexBuffer(ctx, vb);
            ctx->setPixelShader(ps);
            ctx->setViewport(stripe(index));
            return mDraw.draw(ctx);
        }

        TResult recordGroup(RHIContext *ctx, Group &g)
        {
            ctx->setRenderTarget(g.rt.get());
            mDraw.bindStates(ctx);
            ctx->setViewport(kFullViewport);
            ctx->clearColor(ColorRGB::BLACK);
            if (g.hasStencil)
            {
                ctx->clearDepthStencil(0.5f, kStencilClear);
            }
            else
            {
                ctx->clearDepth(0.5f);
            }
            ctx->beginPass();
            ctx->setVertexShader(mDraw.vs());

            TResult ret = T3D_OK;
            do
            {
                ret = drawStripe(ctx, 0, mDepthLess.get(), mFarVB.get(), mDraw.whitePS());
                if (T3D_FAILED(ret)) break;
                ret = drawStripe(ctx, 1, mDepthLess.get(), mNearVB.get(), mDraw.greenPS());
                if (T3D_FAILED(ret)) break;
                if (g.hasStencil)
                {
                    ret = drawStripe(ctx, 2, mStencilPass.get(), mNearVB.get(), mDraw.greenPS());
                    if (T3D_FAILED(ret)) break;
                    ret = drawStripe(ctx, 3, mStencilFail.get(), mNearVB.get(), mDraw.whitePS());
                }
            } while (false);
            ctx->endPass();
            if (T3D_FAILED(ret)) return ret;

            g.colorHandle = mapColorRT(g.color.get());
            if (!g.colorHandle.isValid()) return T3D_ERR_FAIL;

            if (!g.readable) return T3D_OK;

            // 换到不带该深度的 RT 再采样，避免同一张深度纹理既是附件又是 SRV
            ctx->setRenderTarget(g.sampleRT.get());
            mDraw.bindStates(ctx);
            ctx->setViewport(kFullViewport);
            ctx->clearColor(ColorRGB::BLACK);
            ctx->beginPass();

            PixelBuffers textures;
            textures.push_back(g.depth->getPixelBuffer());
            Samplers samplers;
            samplers.push_back(mSampler);
            ctx->setVertexShader(mDraw.vs());
            ctx->setPixelShader(mSamplePS.get());
            ctx->setPSPixelBuffers(0, textures);
            ctx->setPSSamplers(0, samplers);
            ret = mDraw.draw(ctx);
            ctx->endPass();
            if (T3D_FAILED(ret)) return ret;

            g.sampleHandle = mapColorRT(g.sample.get());
            return g.sampleHandle.isValid() ? T3D_OK : T3D_ERR_FAIL;
        }

        bool verifyStripes(Group &g)
        {
            if (!g.colorHandle.isValid()) return false;

            Buffer data;
            if (T3D_FAILED(unmapColorRT(g.color.get(), g.colorHandle, data))) return false;

            const char *expected = g.hasStencil ? "KGGK" : "KGKK";
            const uint32_t stripeW = kWidth / kStripes;
            char got[kStripes + 1] = {};
            uint32_t bad = 0;
            for (uint32_t s = 0; s < kStripes; ++s)
            {
                // 只看竖带内部，避开相邻竖带的边界像素
                const uint32_t x0 = s * stripeW + 2;
                const uint32_t x1 = (s + 1) * stripeW - 2;
                got[s] = '?';
                for (uint32_t y = 0; y < kHeight; ++y)
                {
                    for (uint32_t x = x0; x < x1; ++x)
                    {
                        const uint8_t *p = pixelAt(data, kWidth, kHeight, x, y);
                        const PixelClass c = (p != nullptr) ? classifyPixel(p) : PixelClass::kOther;
                        if (x == x0 && y == 0) got[s] = char(c);
                        if (char(c) != expected[s]) ++bad;
                    }
                }
            }
            data.release();

            if (bad != 0)
            {
                APP_LOG_DEBUG("[%s] FAILED : %s stripes got \"%s\" expected \"%s\" (%u pixels mismatch)",
                    getName(), g.label.c_str(), got, expected, bad);
                return false;
            }
            return true;
        }

        bool verifySample(Group &g)
        {
            if (!g.sampleHandle.isValid()) return false;

            Buffer data;
            if (T3D_FAILED(unmapColorRT(g.sample.get(), g.sampleHandle, data))) return false;

            const uint8_t *p = pixelAt(data, kSampleSize, kSampleSize, kSampleSize / 2, kSampleSize / 2);
            const int red = (p != nullptr) ? int(p[0]) : -1;
            data.release();

            if (std::abs(red - 128) > 4)
            {
                APP_LOG_DEBUG("[%s] FAILED : %s sampled depth R = %d expected about 128 (depth 0.5)",
                    getName(), g.label.c_str(), red);
                return false;
            }
            return true;
        }

        FullscreenDraw          mDraw;
        ShaderVariantPtr        mSamplePS;
        ShaderPtr               mDummySample;
        VertexBufferPtr         mNearVB;
        VertexBufferPtr         mFarVB;
        DepthStencilStatePtr    mDepthLess;
        DepthStencilStatePtr    mStencilPass;
        DepthStencilStatePtr    mStencilFail;
        SamplerStatePtr         mSampler;
        std::vector<Group>      mGroups;
    };


    /**
     * \brief VS 与 PS 同时采样：两个阶段各在自己的 t0 / s0 上绑一张不同的纹理
     * \remarks VS 的 t0 清成绿，PS 的 t0 清成红，PS 输出 (PS 纹理的 R, VS 纹理的 G, 0)，两张都读对才是黄色。
     *          竖带 0 先绑 VS 再绑 PS，竖带 1 反过来。GL 的纹理单元由各阶段共享，后端曾把各阶段的
     *          t# 直接当作纹理单元号，后绑的覆盖先绑的，竖带 0 读回红、竖带 1 读回绿（E.1.3）。
     */
    class CaseCrossStageTextures : public ComputeCase
    {
    public:
        ~CaseCrossStageTextures() override
        {
            releaseColorRT(mVSTex, mVSRT);
            releaseColorRT(mPSTex, mPSRT);
            releaseColorRT(mDstTex, mDstRT);
        }

        static const uint32_t kWidth = 32;
        static const uint32_t kHeight = 8;
        static const uint32_t kStripes = 2;

        const char *getName() const override { return "K18 cross stage textures"; }
        uint32_t getRequiredCaps() const override { return kNeedReadback; }
        uint32_t getBackendMask() const override { return kBackendD3D11 | kBackendGL4; }

        TResult setup() override
        {
            if (T3D_FAILED(mDraw.setup())) return T3D_ERR_FAIL;

            mSampleVS = ComputeKernel::compileGraphics("SampleTextureVS", SHADER_STAGE::kVertex,
                sampleTextureVSSource(), mDummyVS);
            mCombinePS = ComputeKernel::compileGraphics("CombineStageTexturesPS", SHADER_STAGE::kPixel,
                combineStageTexturesPSSource(), mDummyPS);
            if (mSampleVS == nullptr || mCombinePS == nullptr) return T3D_ERR_FAIL;

            SamplerDesc desc;
            desc.MinFilter = FilterOptions::kPoint;
            desc.MagFilter = FilterOptions::kPoint;
            desc.MipFilter = FilterOptions::kPoint;
            mSampler = T3D_RENDER_STATE_MGR.loadSamplerState(desc);

            mVSTex = createColorRT("K18-VS", 4, 4, PixelFormat::E_PF_R8G8B8A8, 1, kCPUNone, true);
            mPSTex = createColorRT("K18-PS", 4, 4, PixelFormat::E_PF_R8G8B8A8, 1, kCPUNone, true);
            mDstTex = createColorRT("K18-Dst", kWidth, kHeight, PixelFormat::E_PF_R8G8B8A8, 1, kCPURead);
            if (mSampler == nullptr || mVSTex == nullptr || mPSTex == nullptr || mDstTex == nullptr)
            {
                return T3D_ERR_FAIL;
            }
            mVSRT = RenderTarget::create(mVSTex);
            mPSRT = RenderTarget::create(mPSTex);
            mDstRT = RenderTarget::create(mDstTex);
            return (mVSRT && mPSRT && mDstRT) ? T3D_OK : T3D_ERR_FAIL;
        }

        TResult record() override
        {
            RHIContext *ctx = T3D_AGENT.getActiveRHIContext();

            ctx->setRenderTarget(mVSRT.get());
            ctx->setViewport(kFullViewport);
            ctx->clearColor(ColorRGB::GREEN);

            ctx->setRenderTarget(mPSRT.get());
            ctx->setViewport(kFullViewport);
            ctx->clearColor(ColorRGB::RED);

            ctx->setRenderTarget(mDstRT.get());
            mDraw.bindStates(ctx);
            ctx->setViewport(kFullViewport);
            ctx->clearColor(ColorRGB::BLACK);
            ctx->beginPass();

            PixelBuffers vsTextures;
            vsTextures.push_back(mVSTex->getPixelBuffer());
            PixelBuffers psTextures;
            psTextures.push_back(mPSTex->getPixelBuffer());
            Samplers samplers;
            samplers.push_back(mSampler);

            ctx->setVertexShader(mSampleVS.get());
            ctx->setPixelShader(mCombinePS.get());

            TResult ret = T3D_OK;
            for (uint32_t s = 0; s < kStripes && T3D_SUCCEEDED(ret); ++s)
            {
                if (s == 0)
                {
                    ctx->setVSPixelBuffers(0, vsTextures);
                    ctx->setVSSamplers(0, samplers);
                    ctx->setPSPixelBuffers(0, psTextures);
                    ctx->setPSSamplers(0, samplers);
                }
                else
                {
                    ctx->setPSPixelBuffers(0, psTextures);
                    ctx->setPSSamplers(0, samplers);
                    ctx->setVSPixelBuffers(0, vsTextures);
                    ctx->setVSSamplers(0, samplers);
                }
                ctx->setViewport(stripe(s));
                ret = mDraw.draw(ctx);
            }

            ctx->endPass();

            if (T3D_SUCCEEDED(ret))
            {
                mHandle = mapColorRT(mDstTex.get());
                ret = mHandle.isValid() ? T3D_OK : T3D_ERR_FAIL;
            }
            ctx->reset();
            return ret;
        }

        TResult verify() override
        {
            Buffer data;
            TResult ret = unmapColorRT(mDstTex.get(), mHandle, data);
            if (T3D_FAILED(ret)) return ret;

            const char *names[kStripes] = { "VS bound first", "PS bound first" };
            const uint32_t stripeW = kWidth / kStripes;
            uint32_t bad = 0;
            for (uint32_t s = 0; s < kStripes; ++s)
            {
                const uint32_t x0 = s * stripeW + 2;
                const uint32_t x1 = (s + 1) * stripeW - 2;
                bool reported = false;
                for (uint32_t y = 0; y < kHeight; ++y)
                {
                    for (uint32_t x = x0; x < x1; ++x)
                    {
                        const uint8_t *p = pixelAt(data, kWidth, kHeight, x, y);
                        if (p != nullptr && isYellow(p)) continue;

                        ++bad;
                        if (!reported)
                        {
                            reported = true;
                            if (p == nullptr)
                            {
                                APP_LOG_DEBUG("[%s] FAILED : %s stripe, readback row pitch too small", getName(), names[s]);
                            }
                            else
                            {
                                APP_LOG_DEBUG("[%s] FAILED : %s stripe got %02X%02X%02X at (%u, %u) expected FFFF00 "
                                    "(R from PS texture, G from VS texture)", getName(), names[s], p[0], p[1], p[2], x, y);
                            }
                        }
                    }
                }
            }
            data.release();
            return (bad == 0) ? T3D_OK : T3D_ERR_FAIL;
        }

    private:
        static Viewport stripe(uint32_t index)
        {
            const Real w = Real(1) / Real(kStripes);
            return Viewport {w * Real(index), 0.0f, w, 1.0f, 0.0f, 1.0f};
        }

        static bool isYellow(const uint8_t *p)
        {
            return p[0] >= 250 && p[1] >= 250 && p[2] <= 5;
        }

        FullscreenDraw      mDraw;
        ShaderVariantPtr    mSampleVS;
        ShaderVariantPtr    mCombinePS;
        ShaderPtr           mDummyVS;
        ShaderPtr           mDummyPS;
        SamplerStatePtr     mSampler;
        RenderTexturePtr    mVSTex;
        RenderTargetPtr     mVSRT;
        RenderTexturePtr    mPSTex;
        RenderTargetPtr     mPSRT;
        RenderTexturePtr    mDstTex;
        RenderTargetPtr     mDstRT;
        ReadbackHandle      mHandle {};
    };


    /**
     * \brief 非 RT 的 MSAA 纹理：4x 的 Texture2DMS 与 3 层 Texture2DMSArray，在 PS 里查询维度
     * \remarks 纹理不带初始数据，PS 只用 GetDimensions / textureSize + textureSamples 把
     *          (采样数, 宽, 高, 层数) 编码进颜色，两条竖带分别对应单张与数组。
     *          GL4 曾忽略非 RT 纹理的 sampleDesc，建成单采样纹理，sampler2DMS 读到的采样数为 0；
     *          D3D11 曾给 MSAA 纹理建 TEXTURE2D 维度的 SRV，创建失败（E.1.5）。
     */
    class CaseMsaaTexture : public ComputeCase
    {
    public:
        ~CaseMsaaTexture() override
        {
            releaseColorRT(mDstTex, mDstRT);
        }

        static const uint32_t kWidth = 32;
        static const uint32_t kHeight = 8;
        static const uint32_t kStripes = 2;
        static const uint32_t kTexWidth = 16;
        static const uint32_t kTexHeight = 8;
        static const uint32_t kSamples = 4;
        static const uint32_t kLayers = 3;

        const char *getName() const override { return "K20 MSAA texture"; }
        uint32_t getRequiredCaps() const override { return kNeedReadback; }
        uint32_t getBackendMask() const override { return kBackendD3D11 | kBackendGL4; }

        TResult setup() override
        {
            if (T3D_FAILED(mDraw.setup())) return T3D_ERR_FAIL;

            mInfoPS = ComputeKernel::compileGraphics("MsaaTextureInfoPS", SHADER_STAGE::kPixel,
                msaaTextureInfoPSSource(), mDummyInfo);
            mArrayInfoPS = ComputeKernel::compileGraphics("MsaaArrayTextureInfoPS", SHADER_STAGE::kPixel,
                msaaArrayTextureInfoPSSource(), mDummyArrayInfo);
            if (mInfoPS == nullptr || mArrayInfoPS == nullptr) return T3D_ERR_FAIL;

            // PixelBuffer 只保存描述的指针，缓存里的对象可能活得比本用例久，描述用静态存储
            static PixelBuffer2DDesc sDesc = makeDesc(1);
            static PixelBuffer2DDesc sArrayDesc = makeDesc(kLayers);
            mTex = T3D_RENDER_BUFFER_MGR.loadPixelBuffer2D(&sDesc, MemoryType::kVRAM, Usage::kStatic, kCPUNone);
            mArrayTex = T3D_RENDER_BUFFER_MGR.loadPixelBuffer2D(&sArrayDesc, MemoryType::kVRAM, Usage::kStatic, kCPUNone);
            if (mTex == nullptr || mTex->getRHIResource() == nullptr
                || mArrayTex == nullptr || mArrayTex->getRHIResource() == nullptr)
            {
                APP_LOG_DEBUG("[%s] FAILED : cannot create %ux MSAA texture", getName(), kSamples);
                return T3D_ERR_FAIL;
            }

            mDstTex = createColorRT("K20-Dst", kWidth, kHeight, PixelFormat::E_PF_R8G8B8A8, 1, kCPURead);
            if (mDstTex == nullptr) return T3D_ERR_FAIL;
            mDstRT = RenderTarget::create(mDstTex);
            return mDstRT ? T3D_OK : T3D_ERR_FAIL;
        }

        TResult record() override
        {
            RHIContext *ctx = T3D_AGENT.getActiveRHIContext();

            ctx->setRenderTarget(mDstRT.get());
            mDraw.bindStates(ctx);
            ctx->setViewport(kFullViewport);
            ctx->clearColor(ColorRGB::BLACK);
            ctx->beginPass();
            ctx->setVertexShader(mDraw.vs());

            ShaderVariant *const stripePS[kStripes] = { mInfoPS.get(), mArrayInfoPS.get() };
            PixelBuffer2D *const stripeTex[kStripes] = { mTex.get(), mArrayTex.get() };
            TResult ret = T3D_OK;
            for (uint32_t s = 0; s < kStripes && T3D_SUCCEEDED(ret); ++s)
            {
                PixelBuffers textures;
                textures.push_back(PixelBufferPtr(stripeTex[s]));
                ctx->setPixelShader(stripePS[s]);
                ctx->setPSPixelBuffers(0, textures);
                ctx->setViewport(stripe(s));
                ret = mDraw.draw(ctx);
            }

            ctx->endPass();

            if (T3D_SUCCEEDED(ret))
            {
                mHandle = mapColorRT(mDstTex.get());
                ret = mHandle.isValid() ? T3D_OK : T3D_ERR_FAIL;
            }
            ctx->reset();
            return ret;
        }

        TResult verify() override
        {
            Buffer data;
            TResult ret = unmapColorRT(mDstTex.get(), mHandle, data);
            if (T3D_FAILED(ret)) return ret;

            const char *names[kStripes] = { "Texture2DMS", "Texture2DMSArray" };
            const uint32_t expectedLayers[kStripes] = { 0, kLayers };
            const uint32_t stripeW = kWidth / kStripes;
            uint32_t bad = 0;
            for (uint32_t s = 0; s < kStripes; ++s)
            {
                // 每条竖带取中心一个像素即可，整条竖带输出的是同一个常量
                const uint8_t *p = pixelAt(data, kWidth, kHeight, s * stripeW + stripeW / 2, kHeight / 2);
                if (p == nullptr)
                {
                    APP_LOG_DEBUG("[%s] FAILED : readback row pitch too small", getName());
                    ++bad;
                    break;
                }

                const uint32_t samples = decode(p[0], 8);
                const uint32_t w = decode(p[1], 64);
                const uint32_t h = decode(p[2], 64);
                const uint32_t layers = (expectedLayers[s] != 0) ? decode(p[3], 8) : 0;
                if (samples != kSamples || w != kTexWidth || h != kTexHeight || layers != expectedLayers[s])
                {
                    ++bad;
                    APP_LOG_DEBUG("[%s] FAILED : %s got samples=%u size=%ux%u layers=%u, expected samples=%u size=%ux%u layers=%u",
                        getName(), names[s], samples, w, h, layers, kSamples, kTexWidth, kTexHeight, expectedLayers[s]);
                }
            }
            data.release();
            return (bad == 0) ? T3D_OK : T3D_ERR_FAIL;
        }

    private:
        static PixelBuffer2DDesc makeDesc(uint32_t arraySize)
        {
            PixelBuffer2DDesc desc;
            desc.width = kTexWidth;
            desc.height = kTexHeight;
            desc.mipmaps = 1;
            desc.arraySize = arraySize;
            desc.format = PixelFormat::E_PF_R8G8B8A8;
            desc.sampleDesc.Count = kSamples;
            desc.sampleDesc.Quality = 0;
            desc.gpuAccess = kGPUShaderResource;
            return desc;
        }

        static Viewport stripe(uint32_t index)
        {
            const Real w = Real(1) / Real(kStripes);
            return Viewport {w * Real(index), 0.0f, w, 1.0f, 0.0f, 1.0f};
        }

        /// 着色器把整数除以 scale 写进 UNORM8，这里还原回整数
        static uint32_t decode(uint8_t v, uint32_t scale)
        {
            return static_cast<uint32_t>(std::lround(double(v) * double(scale) / 255.0));
        }

        FullscreenDraw      mDraw;
        ShaderVariantPtr    mInfoPS;
        ShaderVariantPtr    mArrayInfoPS;
        ShaderPtr           mDummyInfo;
        ShaderPtr           mDummyArrayInfo;
        PixelBuffer2DPtr    mTex;
        PixelBuffer2DPtr    mArrayTex;
        RenderTexturePtr    mDstTex;
        RenderTargetPtr     mDstRT;
        ReadbackHandle      mHandle {};
    };


    /**
     * \brief 引擎层 RenderTexture 带 kGPUUnorderedAccess，交替作为渲染目标与 compute UAV
     * \remarks 用 TextureManager::createRenderTexture 的 gpuAccess 参数建 32x32 的 RT，resize 到 64x64
     *          （后端重建资源时要保留 UAV），再依次：作为渲染目标清白；作为 UAV 由 compute 把
     *          (x, y, 128, 255) 写进上半部分的行；再作为渲染目标在左半屏画绿。
     *          期望左半绿、右上为 compute 图案、右下保持白色，覆盖 RT → UAV → RT 两个方向的切换（E.1.6）。
     */
    class CaseRenderTextureUAVTarget : public ComputeCase
    {
    public:
        ~CaseRenderTextureUAVTarget() override
        {
            releaseColorRT(mTex, mRT);
        }

        static const uint32_t kInitSize = 32;
        static const uint32_t kSize = 64;
        static const uint32_t kHalf = kSize / 2;

        const char *getName() const override { return "K22 render texture UAV as render target"; }
        uint32_t getRequiredCaps() const override
        {
            return kNeedCompute | kNeedUnorderedAccess | kNeedReadback;
        }
        uint32_t getBackendMask() const override { return kBackendD3D11 | kBackendGL4; }

        TResult setup() override
        {
            if (T3D_FAILED(mDraw.setup())) return T3D_ERR_FAIL;

            mKernel = ComputeKernel::create("ComputeTextureWrite", textureWriteSource(), 8, 8, 1);
            if (mKernel == nullptr) return T3D_ERR_FAIL;

            mTex = T3D_TEXTURE_MGR.createRenderTexture("K22-RT", kInitSize, kInitSize, PixelFormat::E_PF_R8G8B8A8,
                1, 1, 0, false, kCPURead, kGPUUnorderedAccess);
            if (mTex == nullptr || mTex->getPixelBuffer() == nullptr) return T3D_ERR_FAIL;
            if ((mTex->getPixelBuffer()->getGPUAccess() & kGPUUnorderedAccess) == 0)
            {
                APP_LOG_DEBUG("[%s] FAILED : render texture lost kGPUUnorderedAccess", getName());
                return T3D_ERR_FAIL;
            }

            if (T3D_FAILED(mTex->resize(kSize, kSize))) return T3D_ERR_FAIL;

            mRT = RenderTarget::create(mTex);
            return mRT ? T3D_OK : T3D_ERR_FAIL;
        }

        TResult record() override
        {
            RHIContext *ctx = T3D_AGENT.getActiveRHIContext();

            ctx->setRenderTarget(mRT.get());
            mDraw.bindStates(ctx);
            ctx->setViewport(kFullViewport);
            ctx->clearColor(ColorRGB::WHITE);
            // D3D11 不允许同一资源同时作为 RTV 与 CS UAV 绑定
            ctx->resetRenderTarget();

            TResult ret = ctx->setComputeShader(mKernel->getVariant());
            if (T3D_FAILED(ret)) return ret;
            UnorderedAccessBuffers uavs;
            uavs.push_back(RenderBufferPtr(mTex->getPixelBuffer()));
            ret = ctx->setCSUnorderedAccessBuffers(0, uavs);
            if (T3D_FAILED(ret)) return ret;
            ret = mKernel->dispatchThreads(kSize, kHalf);
            if (T3D_FAILED(ret)) return ret;
            ctx->uavBarrier(uavs);
            UnorderedAccessBuffers unbind;
            unbind.push_back(nullptr);
            ctx->setCSUnorderedAccessBuffers(0, unbind);
            ctx->setComputeShader(nullptr);

            ctx->setRenderTarget(mRT.get());
            mDraw.bindStates(ctx);
            ctx->setViewport(Viewport {0.0f, 0.0f, 0.5f, 1.0f, 0.0f, 1.0f});
            ctx->beginPass();
            ctx->setVertexShader(mDraw.vs());
            ctx->setPixelShader(mDraw.greenPS());
            ret = mDraw.draw(ctx);
            ctx->endPass();

            if (T3D_SUCCEEDED(ret))
            {
                mHandle = mapColorRT(mTex.get());
                ret = mHandle.isValid() ? T3D_OK : T3D_ERR_FAIL;
            }
            ctx->reset();
            return ret;
        }

        TResult verify() override
        {
            Buffer data;
            TResult ret = unmapColorRT(mTex.get(), mHandle, data);
            if (T3D_FAILED(ret)) return ret;

            uint32_t badGreen = 0, badPattern = 0, badWhite = 0;
            for (uint32_t y = 0; y < kSize; ++y)
            {
                for (uint32_t x = 0; x < kSize; ++x)
                {
                    const uint8_t *p = pixelAt(data, kSize, kSize, x, y);
                    if (p == nullptr)
                    {
                        APP_LOG_DEBUG("[%s] FAILED : readback row pitch too small", getName());
                        data.release();
                        return T3D_ERR_FAIL;
                    }

                    if (x < kHalf)
                    {
                        badGreen += (classifyPixel(p) != PixelClass::kGreen) ? 1 : 0;
                    }
                    else if (y < kHalf)
                    {
                        const bool match = p[0] == uint8_t(x) && p[1] == uint8_t(y) && p[2] == 128 && p[3] == 255;
                        badPattern += match ? 0 : 1;
                    }
                    else
                    {
                        badWhite += (classifyPixel(p) != PixelClass::kWhite) ? 1 : 0;
                    }
                }
            }

            const uint8_t *tl = pixelAt(data, kSize, kSize, kHalf / 2, kHalf / 2);
            const uint8_t *tr = pixelAt(data, kSize, kSize, kHalf + kHalf / 2, kHalf / 2);
            const uint8_t *br = pixelAt(data, kSize, kSize, kHalf + kHalf / 2, kHalf + kHalf / 2);
            if (badGreen + badPattern + badWhite != 0)
            {
                APP_LOG_DEBUG("[%s] FAILED : mismatched green=%u pattern=%u white=%u, "
                    "samples left=(%u,%u,%u,%u) top-right=(%u,%u,%u,%u) bottom-right=(%u,%u,%u,%u)",
                    getName(), badGreen, badPattern, badWhite,
                    tl[0], tl[1], tl[2], tl[3], tr[0], tr[1], tr[2], tr[3], br[0], br[1], br[2], br[3]);
            }
            data.release();
            return (badGreen + badPattern + badWhite == 0) ? T3D_OK : T3D_ERR_FAIL;
        }

    private:
        FullscreenDraw      mDraw;
        ComputeKernelPtr    mKernel;
        RenderTexturePtr    mTex;
        RenderTargetPtr     mRT;
        ReadbackHandle      mHandle {};
    };


    /**
     * \brief K23：UAV 槽位越界检查（E.1.7）
     * \remarks 槽位上限取 caps.maxUnorderedAccessSlots（记为 N）。起点为 N、[N-1, N+1) 两个槽、
     *          起点为 UINT32_MAX（加法溢出）三种越界调用都应返回 T3D_ERR_INVALID_PARAM；
     *          在最后一个合法槽 N-1 上绑定、解绑结构化缓冲应返回 T3D_OK，确认检查没有收得过紧。
     *          纹理与带计数器的结构化缓冲分别按 maxImageUnorderedAccessSlots（I）与
     *          maxCounterUnorderedAccessSlots（C）再测一遍：槽 I-1 / C-1 应成功；I / C 小于 N 时，
     *          槽 I / C 虽在 N 以内也应被拒绝（GL4 上 image unit 与 atomic counter 绑定点比 SSBO 少）。
     *          修复前 GL4 对越界调用一律返回 T3D_OK，由驱动报 GL_INVALID_VALUE。
     */
    class CaseUAVSlotRange : public ComputeCase
    {
    public:
        const char *getName() const override { return "K23 UAV slot range"; }
        uint32_t getRequiredCaps() const override
        {
            return kNeedCompute | kNeedUnorderedAccess | kNeedStructuredBuffer | kNeedAppendConsume;
        }
        uint32_t getBackendMask() const override { return kBackendD3D11 | kBackendGL4; }

        TResult setup() override
        {
            mBuffer = loadSB(StructuredBufferKind::kStructured, sizeof(uint32_t), 64,
                kGPUUnorderedAccess, kCPUNone);
            mCounterBuffer = loadSB(StructuredBufferKind::kStructured, sizeof(uint32_t), 64,
                kGPUUnorderedAccess, kCPUNone, Buffer(), PixelFormat::E_PF_UNKNOWN, true);

            // PixelBuffer 只保存描述的指针，缓存里的对象可能活得比本用例久，描述用静态存储
            static PixelBuffer2DDesc sDesc = []()
            {
                PixelBuffer2DDesc desc;
                desc.width = 4;
                desc.height = 4;
                desc.mipmaps = 1;
                desc.arraySize = 1;
                desc.format = PixelFormat::E_PF_R8G8B8A8;
                desc.gpuAccess = kGPUUnorderedAccess;
                return desc;
            }();
            mTexture = T3D_RENDER_BUFFER_MGR.loadPixelBuffer2D(&sDesc, MemoryType::kVRAM, Usage::kStatic, kCPUNone);

            return (mBuffer != nullptr && mCounterBuffer != nullptr && mTexture != nullptr) ? T3D_OK : T3D_ERR_FAIL;
        }

        TResult record() override
        {
            RHIContext *ctx = T3D_AGENT.getActiveRHIContext();
            const RHICapabilities &caps = ctx->getCapabilities();
            const uint32_t maxSlots = caps.maxUnorderedAccessSlots;
            const uint32_t maxImage = caps.maxImageUnorderedAccessSlots;
            const uint32_t maxCounter = caps.maxCounterUnorderedAccessSlots;
            if (maxSlots == 0 || maxImage == 0 || maxCounter == 0 || maxImage > maxSlots || maxCounter > maxSlots)
            {
                APP_LOG_DEBUG("[%s] FAILED : invalid caps, slots=%u image=%u counter=%u",
                    getName(), maxSlots, maxImage, maxCounter);
                return T3D_ERR_FAIL;
            }

            UnorderedAccessBuffers one;
            one.push_back(nullptr);
            UnorderedAccessBuffers two;
            two.push_back(nullptr);
            two.push_back(nullptr);

            mFailures = 0;
            expect(ctx->setCSUnorderedAccessBuffers(maxSlots, one), T3D_ERR_INVALID_PARAM, "start = N");
            expect(ctx->setCSUnorderedAccessBuffers(maxSlots - 1, two), T3D_ERR_INVALID_PARAM, "[N-1, N+1)");
            expect(ctx->setCSUnorderedAccessBuffers(UINT32_MAX, one), T3D_ERR_INVALID_PARAM, "start = UINT32_MAX");

            checkLimit(ctx, mBuffer, maxSlots, maxSlots, "buffer");
            checkLimit(ctx, mTexture, maxImage, maxSlots, "texture");
            checkLimit(ctx, mCounterBuffer, maxCounter, maxSlots, "counter buffer");
            return T3D_OK;
        }

        TResult verify() override
        {
            return mFailures == 0 ? T3D_OK : T3D_ERR_FAIL;
        }

    private:
        void expect(TResult got, TResult expected, const char *what)
        {
            if (got != expected)
            {
                APP_LOG_DEBUG("[%s] FAILED : %s returned %d, expected %d", getName(), what, (int)got, (int)expected);
                ++mFailures;
            }
        }

        /// 槽 limit-1 上绑定、解绑应成功；limit 小于 N 时，槽 limit 应被拒绝
        void checkLimit(RHIContext *ctx, const RenderBufferPtr &resource, uint32_t limit, uint32_t maxSlots, const char *what)
        {
            UnorderedAccessBuffers bound;
            bound.push_back(resource);
            UnorderedAccessBuffers unbind;
            unbind.push_back(nullptr);

            char label[96];
            snprintf(label, sizeof(label), "%s bind at slot %u", what, limit - 1);
            expect(ctx->setCSUnorderedAccessBuffers(limit - 1, bound), T3D_OK, label);
            snprintf(label, sizeof(label), "%s unbind at slot %u", what, limit - 1);
            expect(ctx->setCSUnorderedAccessBuffers(limit - 1, unbind), T3D_OK, label);

            if (limit < maxSlots)
            {
                snprintf(label, sizeof(label), "%s bind at slot %u", what, limit);
                expect(ctx->setCSUnorderedAccessBuffers(limit, bound), T3D_ERR_INVALID_PARAM, label);
            }
        }

        StructuredBufferPtr mBuffer;
        StructuredBufferPtr mCounterBuffer;
        PixelBuffer2DPtr    mTexture;
        uint32_t            mFailures {0};
    };


    /**
     * \brief K24：常量缓冲按阶段与槽位绑定，nullptr 解绑（E.2.1）
     * \remarks CS 的 b0 / b1 与 PS 的 b0 用同名 cbuffer（CBSlotA / CBSlotB），GL 只能按名字把 UBO 对到 block 上。
     *          1. 返回码：起点 UINT32_MAX 应返回 T3D_ERR_INVALID_PARAM；槽 13 上绑定、用 nullptr 解绑应返回 T3D_OK。
     *          2. graphics：PS b0 设绿色后，CS 再以 [0, 2) 设同名的 CBSlotA，左竖带期望仍为绿；
     *             PS b0 先用 nullptr 解绑再设白色，右竖带期望为白。
     *          3. compute：PS 的同名 CB 都在 CS 之后设置，再以起点 1 单独替换 CS 的 b1，
     *             期望读回 b0 = (1, 2, 3, 4)、b1 = (9, 10, 11, 12)。
     *          修复前 GL4 把所有阶段的 CB 按名字放进同一张表，后设置的覆盖先设置的：左竖带读到 CS 的值
     *          (1, 2, 3, 4) 截断成白色，compute 的 b0 读到 PS 的白色；D3D11 遇到 nullptr 解引用空指针崩溃。
     */
    class CaseConstantBufferSlots : public ComputeCase
    {
    public:
        ~CaseConstantBufferSlots() override
        {
            releaseColorRT(mTex, mRT);
        }

        static const uint32_t kWidth = 32;
        static const uint32_t kHeight = 8;
        static const uint32_t kStripes = 2;

        const char *getName() const override { return "K24 constant buffer slots"; }
        uint32_t getRequiredCaps() const override
        {
            return kNeedCompute | kNeedUnorderedAccess | kNeedStructuredBuffer | kNeedReadback;
        }
        uint32_t getBackendMask() const override { return kBackendD3D11 | kBackendGL4; }

        TResult setup() override
        {
            if (T3D_FAILED(mDraw.setup())) return T3D_ERR_FAIL;

            mKernel = ComputeKernel::create("ComputeCBSlots", cbSlotsCSSource(), 64);
            mColorPS = ComputeKernel::compileGraphics("CBSlotColorPS", SHADER_STAGE::kPixel,
                cbSlotColorPSSource(), mDummyPS);
            if (mKernel == nullptr || mColorPS == nullptr) return T3D_ERR_FAIL;

            mPSGreen = loadCB("CBSlotA", 0.0f, 1.0f, 0.0f, 1.0f);
            mPSWhite = loadCB("CBSlotA", 1.0f, 1.0f, 1.0f, 1.0f);
            mCSA = loadCB("CBSlotA", 1.0f, 2.0f, 3.0f, 4.0f);
            mCSB = loadCB("CBSlotB", 5.0f, 6.0f, 7.0f, 8.0f);
            mCSB2 = loadCB("CBSlotB", 9.0f, 10.0f, 11.0f, 12.0f);

            mOutput = loadSB(StructuredBufferKind::kStructured, sizeof(float) * 4, 2,
                kGPUUnorderedAccess | kGPUShaderResource, kCPURead);
            mTex = createColorRT("K24-Dst", kWidth, kHeight, PixelFormat::E_PF_R8G8B8A8, 1, kCPURead);
            if (!mPSGreen || !mPSWhite || !mCSA || !mCSB || !mCSB2 || !mOutput || !mTex)
            {
                return T3D_ERR_FAIL;
            }
            mRT = RenderTarget::create(mTex);
            return mRT ? T3D_OK : T3D_ERR_FAIL;
        }

        TResult record() override
        {
            RHIContext *ctx = T3D_AGENT.getActiveRHIContext();
            mFailures = 0;

            ConstantBuffers unbind;
            unbind.push_back(nullptr);

            expect(ctx->setCSConstantBuffers(UINT32_MAX, list(mCSB)), T3D_ERR_INVALID_PARAM, "CS start = UINT32_MAX");
            expect(ctx->setCSConstantBuffers(13, list(mCSB)), T3D_OK, "CS bind at slot 13");
            expect(ctx->setCSConstantBuffers(13, unbind), T3D_OK, "CS unbind at slot 13");

            ctx->setRenderTarget(mRT.get());
            mDraw.bindStates(ctx);
            ctx->setViewport(kFullViewport);
            ctx->clearColor(ColorRGB::BLACK);
            ctx->beginPass();

            ctx->setVertexShader(mDraw.vs());
            ctx->setPixelShader(mColorPS.get());

            TResult ret = T3D_OK;

            expect(ctx->setPSConstantBuffers(0, list(mPSGreen)), T3D_OK, "PS bind green");
            ConstantBuffers csBuffers;
            csBuffers.push_back(mCSA);
            csBuffers.push_back(mCSB);
            expect(ctx->setCSConstantBuffers(0, csBuffers), T3D_OK, "CS bind [0, 2)");
            ctx->setViewport(stripe(0));
            ret = mDraw.draw(ctx);

            if (T3D_SUCCEEDED(ret))
            {
                expect(ctx->setPSConstantBuffers(0, unbind), T3D_OK, "PS unbind slot 0");
                expect(ctx->setPSConstantBuffers(0, list(mPSWhite)), T3D_OK, "PS bind white");
                ctx->setViewport(stripe(1));
                ret = mDraw.draw(ctx);
            }

            ctx->endPass();

            if (T3D_SUCCEEDED(ret))
            {
                expect(ctx->setCSConstantBuffers(1, list(mCSB2)), T3D_OK, "CS replace slot 1");
                ret = ctx->setComputeShader(mKernel->getVariant());
            }

            UnorderedAccessBuffers uavs;
            uavs.push_back(mOutput);
            if (T3D_SUCCEEDED(ret))
            {
                ret = ctx->setCSUnorderedAccessBuffers(uavSlot(0, 0), uavs);
            }
            if (T3D_SUCCEEDED(ret))
            {
                ret = mKernel->dispatchThreads(2);
            }
            if (T3D_SUCCEEDED(ret))
            {
                ctx->uavBarrier(uavs);
                ctx->setComputeShader(nullptr);

                mRTHandle = mapColorRT(mTex.get());
                ret = mapWhole(mOutput.get(), mSBHandle);
                if (T3D_SUCCEEDED(ret) && !mRTHandle.isValid())
                {
                    ret = T3D_ERR_FAIL;
                }
            }
            ctx->reset();
            return ret;
        }

        TResult verify() override
        {
            Buffer pixels;
            TResult ret = unmapColorRT(mTex.get(), mRTHandle, pixels);
            if (T3D_FAILED(ret)) return ret;

            const char *names[kStripes] = { "PS green after CS same name", "PS white after nullptr unbind" };
            const PixelClass expected[kStripes] = { PixelClass::kGreen, PixelClass::kWhite };
            const uint32_t stripeW = kWidth / kStripes;
            for (uint32_t s = 0; s < kStripes; ++s)
            {
                bool reported = false;
                for (uint32_t y = 0; y < kHeight && !reported; ++y)
                {
                    for (uint32_t x = s * stripeW + 2; x < (s + 1) * stripeW - 2; ++x)
                    {
                        const uint8_t *p = pixelAt(pixels, kWidth, kHeight, x, y);
                        if (p != nullptr && classifyPixel(p) == expected[s]) continue;

                        ++mFailures;
                        reported = true;
                        if (p == nullptr)
                        {
                            APP_LOG_DEBUG("[%s] FAILED : %s stripe, readback row pitch too small", getName(), names[s]);
                        }
                        else
                        {
                            APP_LOG_DEBUG("[%s] FAILED : %s stripe got %02X%02X%02X at (%u, %u) expected %c",
                                getName(), names[s], p[0], p[1], p[2], x, y, (char)expected[s]);
                        }
                        break;
                    }
                }
            }
            pixels.release();

            Buffer data;
            ret = unmapCheckSize(mOutput.get(), mSBHandle, data, sizeof(float) * 4 * 2);
            if (T3D_FAILED(ret)) return ret;

            const float *out = reinterpret_cast<const float *>(data.Data);
            const float expectCS[2][4] = { { 1, 2, 3, 4 }, { 9, 10, 11, 12 } };
            const char *slots[2] = { "b0", "b1" };
            for (uint32_t i = 0; i < 2; ++i)
            {
                const float *v = out + i * 4;
                if (v[0] != expectCS[i][0] || v[1] != expectCS[i][1] || v[2] != expectCS[i][2] || v[3] != expectCS[i][3])
                {
                    APP_LOG_DEBUG("[%s] FAILED : CS %s got (%g, %g, %g, %g) expected (%g, %g, %g, %g)",
                        getName(), slots[i], v[0], v[1], v[2], v[3],
                        expectCS[i][0], expectCS[i][1], expectCS[i][2], expectCS[i][3]);
                    ++mFailures;
                }
            }
            data.release();

            return mFailures == 0 ? T3D_OK : T3D_ERR_FAIL;
        }

    private:
        static Viewport stripe(uint32_t index)
        {
            const Real w = Real(1) / Real(kStripes);
            return Viewport {w * Real(index), 0.0f, w, 1.0f, 0.0f, 1.0f};
        }

        static ConstantBufferPtr loadCB(const char *name, float x, float y, float z, float w)
        {
            const float value[4] = { x, y, z, w };
            Buffer cb = allocBytes(sizeof(value));
            memcpy(cb.Data, value, sizeof(value));
            ConstantBufferPtr buffer = T3D_RENDER_BUFFER_MGR.loadConstantBuffer(name, 0, cb,
                MemoryType::kBoth, Usage::kDynamic, kCPUWrite);
            cb.release();
            return buffer;
        }

        static ConstantBuffers list(const ConstantBufferPtr &buffer)
        {
            ConstantBuffers buffers;
            buffers.push_back(buffer);
            return buffers;
        }

        void expect(TResult got, TResult expected, const char *what)
        {
            if (got != expected)
            {
                APP_LOG_DEBUG("[%s] FAILED : %s returned %d, expected %d", getName(), what, (int)got, (int)expected);
                ++mFailures;
            }
        }

        FullscreenDraw      mDraw;
        ComputeKernelPtr    mKernel;
        ShaderVariantPtr    mColorPS;
        ShaderPtr           mDummyPS;
        ConstantBufferPtr   mPSGreen;
        ConstantBufferPtr   mPSWhite;
        ConstantBufferPtr   mCSA;
        ConstantBufferPtr   mCSB;
        ConstantBufferPtr   mCSB2;
        StructuredBufferPtr mOutput;
        RenderTexturePtr    mTex;
        RenderTargetPtr     mRT;
        ReadbackHandle      mRTHandle {};
        ReadbackHandle      mSBHandle {};
        uint32_t            mFailures {0};
    };


    /**
     * \brief K25：同一张纹理配两个采样器（E.2.2）
     * \remarks 2x1 的纹理左黑右白，PS 用 s0、s1 两个采样器在 u = 0.375 处采样 t0，R 取 s0、G 取 s1。
     *          点采样落在左边的黑色纹素上得 0，线性采样在两个纹素中心之间按 0.25 插值得约 64。
     *          竖带 0 绑 [点, 线性]，期望 (0, 64)；竖带 1 绑 [线性, 点]，期望 (64, 0)，确认采样器按槽位对应而不是按顺序。
     *          GL 只有合并采样器，一个纹理单元只能挂一个 sampler object。修复前 GL4 的反射按纹理名建参数，
     *          两个合并采样器写同一个参数，纹理槽位被后者改成 1，两个 uniform 都指到没有绑纹理的单元，两条竖带均读回 (0, 0)。
     */
    class CaseTwoSamplersOneTexture : public ComputeCase
    {
    public:
        ~CaseTwoSamplersOneTexture() override
        {
            releaseColorRT(mSrcTex, mSrcRT);
            releaseColorRT(mDstTex, mDstRT);
        }

        static const uint32_t kWidth = 32;
        static const uint32_t kHeight = 8;
        static const uint32_t kStripes = 2;

        const char *getName() const override { return "K25 two samplers one texture"; }
        uint32_t getRequiredCaps() const override { return kNeedReadback; }
        uint32_t getBackendMask() const override { return kBackendD3D11 | kBackendGL4; }

        TResult setup() override
        {
            if (T3D_FAILED(mDraw.setup())) return T3D_ERR_FAIL;

            mSamplePS = ComputeKernel::compileGraphics("TwoSamplersPS", SHADER_STAGE::kPixel,
                twoSamplersPSSource(), mDummyPS);
            if (mSamplePS == nullptr) return T3D_ERR_FAIL;

            SamplerDesc desc;
            desc.AddressU = TextureAddressMode::kClamp;
            desc.AddressV = TextureAddressMode::kClamp;
            desc.AddressW = TextureAddressMode::kClamp;
            desc.MipFilter = FilterOptions::kPoint;
            desc.MinFilter = FilterOptions::kPoint;
            desc.MagFilter = FilterOptions::kPoint;
            mPoint = T3D_RENDER_STATE_MGR.loadSamplerState(desc);
            desc.MinFilter = FilterOptions::kLinear;
            desc.MagFilter = FilterOptions::kLinear;
            mLinear = T3D_RENDER_STATE_MGR.loadSamplerState(desc);

            mSrcTex = createColorRT("K25-Src", 2, 1, PixelFormat::E_PF_R8G8B8A8, 1, kCPUNone, true);
            mDstTex = createColorRT("K25-Dst", kWidth, kHeight, PixelFormat::E_PF_R8G8B8A8, 1, kCPURead);
            if (mPoint == nullptr || mLinear == nullptr || mSrcTex == nullptr || mDstTex == nullptr)
            {
                return T3D_ERR_FAIL;
            }
            mSrcRT = RenderTarget::create(mSrcTex);
            mDstRT = RenderTarget::create(mDstTex);
            return (mSrcRT && mDstRT) ? T3D_OK : T3D_ERR_FAIL;
        }

        TResult record() override
        {
            RHIContext *ctx = T3D_AGENT.getActiveRHIContext();

            // 源纹理：清黑后在右半边画白，得到左黑右白两个纹素
            ctx->setRenderTarget(mSrcRT.get());
            mDraw.bindStates(ctx);
            ctx->setViewport(kFullViewport);
            ctx->clearColor(ColorRGB::BLACK);
            ctx->setVertexShader(mDraw.vs());
            ctx->setPixelShader(mDraw.whitePS());
            ctx->setViewport(Viewport {0.5f, 0.0f, 0.5f, 1.0f, 0.0f, 1.0f});
            TResult ret = mDraw.draw(ctx);

            ctx->setRenderTarget(mDstRT.get());
            ctx->setViewport(kFullViewport);
            ctx->clearColor(ColorRGB::BLACK);
            ctx->beginPass();

            PixelBuffers textures;
            textures.push_back(mSrcTex->getPixelBuffer());
            ctx->setPixelShader(mSamplePS.get());
            ctx->setPSPixelBuffers(0, textures);

            for (uint32_t s = 0; s < kStripes && T3D_SUCCEEDED(ret); ++s)
            {
                Samplers samplers;
                samplers.push_back(s == 0 ? mPoint : mLinear);
                samplers.push_back(s == 0 ? mLinear : mPoint);
                ctx->setPSSamplers(0, samplers);
                ctx->setViewport(stripe(s));
                ret = mDraw.draw(ctx);
            }

            ctx->endPass();

            if (T3D_SUCCEEDED(ret))
            {
                mHandle = mapColorRT(mDstTex.get());
                ret = mHandle.isValid() ? T3D_OK : T3D_ERR_FAIL;
            }
            ctx->reset();
            return ret;
        }

        TResult verify() override
        {
            Buffer data;
            TResult ret = unmapColorRT(mDstTex.get(), mHandle, data);
            if (T3D_FAILED(ret)) return ret;

            const char *names[kStripes] = { "[point, linear]", "[linear, point]" };
            const int expected[kStripes][2] = { { 0, 64 }, { 64, 0 } };
            const uint32_t stripeW = kWidth / kStripes;
            uint32_t bad = 0;
            for (uint32_t s = 0; s < kStripes; ++s)
            {
                bool reported = false;
                for (uint32_t y = 0; y < kHeight; ++y)
                {
                    for (uint32_t x = s * stripeW + 2; x < (s + 1) * stripeW - 2; ++x)
                    {
                        const uint8_t *p = pixelAt(data, kWidth, kHeight, x, y);
                        if (p != nullptr && std::abs(int(p[0]) - expected[s][0]) <= 8
                            && std::abs(int(p[1]) - expected[s][1]) <= 8)
                        {
                            continue;
                        }

                        ++bad;
                        if (!reported)
                        {
                            reported = true;
                            if (p == nullptr)
                            {
                                APP_LOG_DEBUG("[%s] FAILED : %s stripe, readback row pitch too small", getName(), names[s]);
                            }
                            else
                            {
                                APP_LOG_DEBUG("[%s] FAILED : samplers %s got (R %d, G %d) at (%u, %u) expected about (%d, %d)",
                                    getName(), names[s], int(p[0]), int(p[1]), x, y, expected[s][0], expected[s][1]);
                            }
                        }
                    }
                }
            }
            data.release();

            return bad == 0 ? T3D_OK : T3D_ERR_FAIL;
        }

    private:
        static Viewport stripe(uint32_t index)
        {
            const Real w = Real(1) / Real(kStripes);
            return Viewport {w * Real(index), 0.0f, w, 1.0f, 0.0f, 1.0f};
        }

        FullscreenDraw      mDraw;
        ShaderVariantPtr    mSamplePS;
        ShaderPtr           mDummyPS;
        SamplerStatePtr     mPoint;
        SamplerStatePtr     mLinear;
        RenderTexturePtr    mSrcTex;
        RenderTargetPtr     mSrcRT;
        RenderTexturePtr    mDstTex;
        RenderTargetPtr     mDstRT;
        ReadbackHandle      mHandle {};
    };


    /**
     * \brief K26：槽位按 HLSL 寄存器号编号，与声明和使用的顺序无关
     * \remarks t0 左黑右白、t1 左白右黑，s0 点采样、s1 线性，都是 2x1、Clamp。PS 先用 t1 + s1，再用 t0 + s0，
     *          最后对 t0 做 Load(1, 0)，三个结果写到 R、G、B，调用方按寄存器号绑定。
     *          期望 R = t1 线性采样 u = 0.375 ≈ 191，G = t0 点采样 = 0，B = t0 的右纹素 = 255。
     *          GL 版是 scc 的原样输出。按 glslang 首次出现的顺序编号时 texA 落到 t0、samplerLinear 落到 s0，读回约 (0, 191, 0)。
     */
    class CaseRegisterOrder : public ComputeCase
    {
    public:
        ~CaseRegisterOrder() override
        {
            releaseColorRT(mTex0, mRT0);
            releaseColorRT(mTex1, mRT1);
            releaseColorRT(mDstTex, mDstRT);
        }

        static const uint32_t kWidth = 16;
        static const uint32_t kHeight = 8;

        const char *getName() const override { return "K26 slots follow HLSL registers"; }
        uint32_t getRequiredCaps() const override { return kNeedReadback; }
        uint32_t getBackendMask() const override { return kBackendD3D11 | kBackendGL4; }

        TResult setup() override
        {
            if (T3D_FAILED(mDraw.setup())) return T3D_ERR_FAIL;

            mSamplePS = ComputeKernel::compileGraphics("RegisterOrderPS", SHADER_STAGE::kPixel,
                registerOrderPSSource(), mDummyPS);
            if (mSamplePS == nullptr) return T3D_ERR_FAIL;

            SamplerDesc desc;
            desc.AddressU = TextureAddressMode::kClamp;
            desc.AddressV = TextureAddressMode::kClamp;
            desc.AddressW = TextureAddressMode::kClamp;
            desc.MipFilter = FilterOptions::kPoint;
            desc.MinFilter = FilterOptions::kPoint;
            desc.MagFilter = FilterOptions::kPoint;
            mPoint = T3D_RENDER_STATE_MGR.loadSamplerState(desc);
            desc.MinFilter = FilterOptions::kLinear;
            desc.MagFilter = FilterOptions::kLinear;
            mLinear = T3D_RENDER_STATE_MGR.loadSamplerState(desc);

            mTex0 = createColorRT("K26-T0", 2, 1, PixelFormat::E_PF_R8G8B8A8, 1, kCPUNone, true);
            mTex1 = createColorRT("K26-T1", 2, 1, PixelFormat::E_PF_R8G8B8A8, 1, kCPUNone, true);
            mDstTex = createColorRT("K26-Dst", kWidth, kHeight, PixelFormat::E_PF_R8G8B8A8, 1, kCPURead);
            if (mPoint == nullptr || mLinear == nullptr || mTex0 == nullptr || mTex1 == nullptr || mDstTex == nullptr)
            {
                return T3D_ERR_FAIL;
            }
            mRT0 = RenderTarget::create(mTex0);
            mRT1 = RenderTarget::create(mTex1);
            mDstRT = RenderTarget::create(mDstTex);
            return (mRT0 && mRT1 && mDstRT) ? T3D_OK : T3D_ERR_FAIL;
        }

        TResult record() override
        {
            RHIContext *ctx = T3D_AGENT.getActiveRHIContext();

            // t0 右半边白，t1 左半边白
            TResult ret = fillHalfWhite(ctx, mRT0.get(), 0.5f);
            if (T3D_SUCCEEDED(ret))
            {
                ret = fillHalfWhite(ctx, mRT1.get(), 0.0f);
            }

            ctx->setRenderTarget(mDstRT.get());
            ctx->setViewport(kFullViewport);
            ctx->clearColor(ColorRGB::BLACK);
            ctx->beginPass();

            PixelBuffers textures;
            textures.push_back(mTex0->getPixelBuffer());
            textures.push_back(mTex1->getPixelBuffer());
            Samplers samplers;
            samplers.push_back(mPoint);
            samplers.push_back(mLinear);

            ctx->setVertexShader(mDraw.vs());
            ctx->setPixelShader(mSamplePS.get());
            ctx->setPSPixelBuffers(0, textures);
            ctx->setPSSamplers(0, samplers);
            if (T3D_SUCCEEDED(ret))
            {
                ret = mDraw.draw(ctx);
            }

            ctx->endPass();

            if (T3D_SUCCEEDED(ret))
            {
                mHandle = mapColorRT(mDstTex.get());
                ret = mHandle.isValid() ? T3D_OK : T3D_ERR_FAIL;
            }
            ctx->reset();
            return ret;
        }

        TResult verify() override
        {
            Buffer data;
            TResult ret = unmapColorRT(mDstTex.get(), mHandle, data);
            if (T3D_FAILED(ret)) return ret;

            const int expected[3] = { 191, 0, 255 };
            uint32_t bad = 0;
            for (uint32_t y = 0; y < kHeight; ++y)
            {
                for (uint32_t x = 0; x < kWidth; ++x)
                {
                    const uint8_t *p = pixelAt(data, kWidth, kHeight, x, y);
                    if (p != nullptr && std::abs(int(p[0]) - expected[0]) <= 8
                        && std::abs(int(p[1]) - expected[1]) <= 8 && std::abs(int(p[2]) - expected[2]) <= 8)
                    {
                        continue;
                    }

                    if (bad++ == 0)
                    {
                        if (p == nullptr)
                        {
                            APP_LOG_DEBUG("[%s] FAILED : readback row pitch too small", getName());
                        }
                        else
                        {
                            APP_LOG_DEBUG("[%s] FAILED : got (R %d, G %d, B %d) at (%u, %u) expected about (%d, %d, %d)",
                                getName(), int(p[0]), int(p[1]), int(p[2]), x, y, expected[0], expected[1], expected[2]);
                        }
                    }
                }
            }
            data.release();

            return bad == 0 ? T3D_OK : T3D_ERR_FAIL;
        }

    private:
        TResult fillHalfWhite(RHIContext *ctx, RenderTarget *rt, Real left)
        {
            ctx->setRenderTarget(rt);
            mDraw.bindStates(ctx);
            ctx->setViewport(kFullViewport);
            ctx->clearColor(ColorRGB::BLACK);
            ctx->setVertexShader(mDraw.vs());
            ctx->setPixelShader(mDraw.whitePS());
            ctx->setViewport(Viewport {left, 0.0f, 0.5f, 1.0f, 0.0f, 1.0f});
            return mDraw.draw(ctx);
        }

        FullscreenDraw      mDraw;
        ShaderVariantPtr    mSamplePS;
        ShaderPtr           mDummyPS;
        SamplerStatePtr     mPoint;
        SamplerStatePtr     mLinear;
        RenderTexturePtr    mTex0;
        RenderTargetPtr     mRT0;
        RenderTexturePtr    mTex1;
        RenderTargetPtr     mRT1;
        RenderTexturePtr    mDstTex;
        RenderTargetPtr     mDstRT;
        ReadbackHandle      mHandle {};
    };


    /**
     * \brief K27：材质路径中的独立采样器按名字推断，只做 Load 的纹理也有槽位
     * \remarks PS 有 t0 + 自带的 samplertex（s0）、独立的 samplerPointClamp（s1）、只做 Load 的 t1。
     *          用 ShaderVariantInstance 建纹理表与采样器表，按 PassInstance::bindSamplers 的方式把采样器表交给 RHI。
     *          t0 左黑右白、t1 左白右黑。期望 R = 点采样 u = 0.375 → 0，G = Clamp 下 u = 1.25 → 255，
     *          B = t1 的 (0, 0) → 255，A = samplertex 在纹理设置前的默认采样器（线性）u = 0.375 → 约 64。
     *          独立采样器若仍用默认的线性 + Wrap，读回约 (64, 0, 255, 64)。
     *          另外直接检查两张表的起点与大小、s1 的描述，以及 SamplerState::inferDescFromName 的几种写法。
     */
    class CaseInlineSampler : public ComputeCase
    {
    public:
        ~CaseInlineSampler() override
        {
            mInstance = nullptr;
            releaseColorRT(mTex0, mRT0);
            releaseColorRT(mTex1, mRT1);
            releaseColorRT(mDstTex, mDstRT);
        }

        static const uint32_t kWidth = 16;
        static const uint32_t kHeight = 8;

        const char *getName() const override { return "K27 inline sampler by name"; }
        uint32_t getRequiredCaps() const override { return kNeedReadback; }
        uint32_t getBackendMask() const override { return kBackendD3D11 | kBackendGL4; }

        TResult setup() override
        {
            if (T3D_FAILED(mDraw.setup())) return T3D_ERR_FAIL;

            mSamplePS = ComputeKernel::compileGraphics("InlineSamplerPS", SHADER_STAGE::kPixel,
                inlineSamplerPSSource(), mDummyPS);
            if (mSamplePS == nullptr) return T3D_ERR_FAIL;

            mInstance = ShaderVariantInstance::create(nullptr, mSamplePS);
            mTex0 = createColorRT("K27-T0", 2, 1, PixelFormat::E_PF_R8G8B8A8, 1, kCPUNone, true);
            mTex1 = createColorRT("K27-T1", 2, 1, PixelFormat::E_PF_R8G8B8A8, 1, kCPUNone, true);
            mDstTex = createColorRT("K27-Dst", kWidth, kHeight, PixelFormat::E_PF_R8G8B8A8, 1, kCPURead);
            if (mInstance == nullptr || mTex0 == nullptr || mTex1 == nullptr || mDstTex == nullptr)
            {
                return T3D_ERR_FAIL;
            }
            mRT0 = RenderTarget::create(mTex0);
            mRT1 = RenderTarget::create(mTex1);
            mDstRT = RenderTarget::create(mDstTex);
            return (mRT0 && mRT1 && mDstRT) ? T3D_OK : T3D_ERR_FAIL;
        }

        TResult record() override
        {
            mTableErrors = checkTables();

            RHIContext *ctx = T3D_AGENT.getActiveRHIContext();

            TResult ret = fillHalfWhite(ctx, mRT0.get(), 0.5f);
            if (T3D_SUCCEEDED(ret))
            {
                ret = fillHalfWhite(ctx, mRT1.get(), 0.0f);
            }

            ctx->setRenderTarget(mDstRT.get());
            ctx->setViewport(kFullViewport);
            ctx->clearColor(ColorRGB::BLACK);
            ctx->beginPass();

            PixelBuffers textures;
            textures.push_back(mTex0->getPixelBuffer());
            textures.push_back(mTex1->getPixelBuffer());

            ctx->setVertexShader(mDraw.vs());
            ctx->setPixelShader(mSamplePS.get());
            ctx->setPSPixelBuffers(mInstance->getPixelBufferStartSlot(), textures);
            if (!mInstance->getSamplers().empty())
            {
                ctx->setPSSamplers(mInstance->getSamplerStartSlot(), mInstance->getSamplers());
            }
            if (T3D_SUCCEEDED(ret))
            {
                ret = mDraw.draw(ctx);
            }

            ctx->endPass();

            if (T3D_SUCCEEDED(ret))
            {
                mHandle = mapColorRT(mDstTex.get());
                ret = mHandle.isValid() ? T3D_OK : T3D_ERR_FAIL;
            }
            ctx->reset();
            return ret;
        }

        TResult verify() override
        {
            Buffer data;
            TResult ret = unmapColorRT(mDstTex.get(), mHandle, data);
            if (T3D_FAILED(ret)) return ret;

            const int expected[4] = { 0, 255, 255, 64 };
            uint32_t bad = 0;
            for (uint32_t y = 0; y < kHeight; ++y)
            {
                for (uint32_t x = 0; x < kWidth; ++x)
                {
                    const uint8_t *p = pixelAt(data, kWidth, kHeight, x, y);
                    bool ok = (p != nullptr);
                    for (int c = 0; ok && c < 4; ++c)
                    {
                        ok = std::abs(int(p[c]) - expected[c]) <= 8;
                    }
                    if (ok) continue;

                    if (bad++ == 0)
                    {
                        if (p == nullptr)
                        {
                            APP_LOG_DEBUG("[%s] FAILED : readback row pitch too small", getName());
                        }
                        else
                        {
                            APP_LOG_DEBUG("[%s] FAILED : got (%d, %d, %d, %d) at (%u, %u) expected about (%d, %d, %d, %d)",
                                getName(), int(p[0]), int(p[1]), int(p[2]), int(p[3]), x, y,
                                expected[0], expected[1], expected[2], expected[3]);
                        }
                    }
                }
            }
            data.release();

            const uint32_t nameErrors = checkNames();
            return (bad == 0 && mTableErrors == 0 && nameErrors == 0) ? T3D_OK : T3D_ERR_FAIL;
        }

    private:
        uint32_t checkTables() const
        {
            uint32_t errors = 0;
            auto expect = [this, &errors](bool cond, const char *what)
            {
                if (!cond)
                {
                    ++errors;
                    APP_LOG_DEBUG("[%s] FAILED : %s", getName(), what);
                }
            };

            auto usage = [this](const char *name, bool texture, bool sampler)
            {
                const ShaderSamplerParams &params = mSamplePS->getShaderSamplerParams();
                const auto itr = params.find(name);
                return itr != params.end() && itr->second->hasTexture() == texture && itr->second->hasSampler() == sampler;
            };
            expect(usage("tex", true, true), "'tex' should have both a texture and a sampler");
            expect(usage("dataTex", true, false), "'dataTex' should be a texture without sampler");
            expect(usage("PointClamp", false, true), "'PointClamp' should be a sampler without texture");

            expect(mInstance->getPixelBufferStartSlot() == 0 && mInstance->getPixelBuffers().size() == 2,
                "texture table should cover t0..t1 (the Load-only texture included)");
            expect(mInstance->getSamplerStartSlot() == 0 && mInstance->getSamplers().size() == 2,
                "sampler table should cover s0..s1");
            if (mInstance->getSamplers().size() == 2 && mInstance->getSamplers()[1] != nullptr)
            {
                const SamplerDesc &desc = mInstance->getSamplers()[1]->getStateDesc();
                expect(desc.MinFilter == FilterOptions::kPoint && desc.MagFilter == FilterOptions::kPoint
                    && desc.AddressU == TextureAddressMode::kClamp && desc.AddressV == TextureAddressMode::kClamp,
                    "s1 (samplerPointClamp) should be point + clamp");
            }
            else
            {
                expect(false, "s1 has no sampler state");
            }
            return errors;
        }

        uint32_t checkNames() const
        {
            uint32_t errors = 0;
            auto expect = [this, &errors](bool cond, const char *name)
            {
                if (!cond)
                {
                    ++errors;
                    APP_LOG_DEBUG("[%s] FAILED : inferDescFromName(\"%s\") gives a wrong desc", getName(), name);
                }
            };

            SamplerDesc d;
            expect(SamplerState::inferDescFromName("samplerLinearClampU_RepeatV", d)
                && d.MinFilter == FilterOptions::kLinear && d.MipFilter == FilterOptions::kPoint
                && d.AddressU == TextureAddressMode::kClamp && d.AddressV == TextureAddressMode::kWrap,
                "samplerLinearClampU_RepeatV");

            d = SamplerDesc();
            expect(SamplerState::inferDescFromName("point_mirror_once", d)
                && d.MinFilter == FilterOptions::kPoint && d.AddressU == TextureAddressMode::kMirrorOnce
                && d.AddressW == TextureAddressMode::kMirrorOnce,
                "point_mirror_once");

            d = SamplerDesc();
            expect(SamplerState::inferDescFromName("TrilinearBorderUVCompare", d)
                && d.MipFilter == FilterOptions::kLinear && d.AddressU == TextureAddressMode::kBorder
                && d.AddressV == TextureAddressMode::kBorder && d.AddressW == TextureAddressMode::kWrap
                && d.IsComparison && d.CompareFunc == CompareFunction::kLessEqual,
                "TrilinearBorderUVCompare");

            d = SamplerDesc();
            expect(!SamplerState::inferDescFromName("samplerShadowMap", d), "samplerShadowMap");
            return errors;
        }

        TResult fillHalfWhite(RHIContext *ctx, RenderTarget *rt, Real left)
        {
            ctx->setRenderTarget(rt);
            mDraw.bindStates(ctx);
            ctx->setViewport(kFullViewport);
            ctx->clearColor(ColorRGB::BLACK);
            ctx->setVertexShader(mDraw.vs());
            ctx->setPixelShader(mDraw.whitePS());
            ctx->setViewport(Viewport {left, 0.0f, 0.5f, 1.0f, 0.0f, 1.0f});
            return mDraw.draw(ctx);
        }

        FullscreenDraw              mDraw;
        ShaderVariantPtr            mSamplePS;
        ShaderPtr                   mDummyPS;
        ShaderVariantInstancePtr    mInstance;
        RenderTexturePtr            mTex0;
        RenderTargetPtr             mRT0;
        RenderTexturePtr            mTex1;
        RenderTargetPtr             mRT1;
        RenderTexturePtr            mDstTex;
        RenderTargetPtr             mDstRT;
        ReadbackHandle              mHandle {};
        uint32_t                    mTableErrors {0};
    };


    /**
     * \brief K28：Texture2DArray 与 Texture2D 混用时的槽位与采样器
     * \remarks t1 是 2 层 2x1 的纹理数组，第 0 层左白右黑、第 1 层左黑右白；t0 是 2x1 的 RT，左黑右白。
     *          s0 点采样、s1 线性，都是 Clamp，声明顺序与寄存器号相反。
     *          期望 R = 第 1 层线性采样 u = 0.375 ≈ 64（读错层约 191，用错采样器为 0），
     *          G = 第 0 层点采样 u = 0.375 = 255（读错层为 0），B = Load 第 1 层右纹素 = 255，
     *          A = t0 线性采样 u = 0.625 ≈ 191（用错采样器为 255，纹理没绑上为 0）。
     *          GL 版是 scc 的原样输出，三个 sampler2DArray uniform 指向同一个纹理槽位、不同的采样器。
     */
    class CaseTextureArray : public ComputeCase
    {
    public:
        ~CaseTextureArray() override
        {
            releaseColorRT(mPlainTex, mPlainRT);
            releaseColorRT(mDstTex, mDstRT);
        }

        static const uint32_t kWidth = 16;
        static const uint32_t kHeight = 8;

        const char *getName() const override { return "K28 texture array slots"; }
        uint32_t getRequiredCaps() const override { return kNeedReadback; }
        uint32_t getBackendMask() const override { return kBackendD3D11 | kBackendGL4; }

        TResult setup() override
        {
            if (T3D_FAILED(mDraw.setup())) return T3D_ERR_FAIL;

            mSamplePS = ComputeKernel::compileGraphics("TextureArrayPS", SHADER_STAGE::kPixel,
                textureArrayPSSource(), mDummyPS);
            if (mSamplePS == nullptr) return T3D_ERR_FAIL;

            SamplerDesc desc;
            desc.AddressU = TextureAddressMode::kClamp;
            desc.AddressV = TextureAddressMode::kClamp;
            desc.AddressW = TextureAddressMode::kClamp;
            desc.MipFilter = FilterOptions::kPoint;
            desc.MinFilter = FilterOptions::kPoint;
            desc.MagFilter = FilterOptions::kPoint;
            mPoint = T3D_RENDER_STATE_MGR.loadSamplerState(desc);
            desc.MinFilter = FilterOptions::kLinear;
            desc.MagFilter = FilterOptions::kLinear;
            mLinear = T3D_RENDER_STATE_MGR.loadSamplerState(desc);

            // PixelBuffer 只保存描述的指针，描述不能放在栈上；初始数据由 RenderBuffer 接管并在析构时释放，
            // 必须从堆上分配，且每个 PixelBuffer 各用一份
            static const uint8_t kLayers[2][2][4] =
            {
                { { 255, 255, 255, 255 }, {   0,   0,   0, 255 } },
                { {   0,   0,   0, 255 }, { 255, 255, 255, 255 } },
            };
            PixelBuffer2DDesc *arrayDesc = T3D_NEW PixelBuffer2DDesc();
            arrayDesc->width = 2;
            arrayDesc->height = 1;
            arrayDesc->mipmaps = 1;
            arrayDesc->arraySize = 2;
            arrayDesc->format = PixelFormat::E_PF_R8G8B8A8;
            arrayDesc->shaderReadable = true;
            arrayDesc->buffer = allocBytes(sizeof(kLayers));
            memcpy(arrayDesc->buffer.Data, kLayers, sizeof(kLayers));
            mArrayTex = T3D_RENDER_BUFFER_MGR.loadPixelBuffer2D(arrayDesc, MemoryType::kVRAM, Usage::kStatic, kCPUNone);
            if (mArrayTex == nullptr || mArrayTex->getRHIResource() == nullptr)
            {
                APP_LOG_DEBUG("[%s] FAILED : cannot create texture array", getName());
                return T3D_ERR_FAIL;
            }

            mPlainTex = createColorRT("K28-Plain", 2, 1, PixelFormat::E_PF_R8G8B8A8, 1, kCPUNone, true);
            mDstTex = createColorRT("K28-Dst", kWidth, kHeight, PixelFormat::E_PF_R8G8B8A8, 1, kCPURead);
            if (mPoint == nullptr || mLinear == nullptr || mPlainTex == nullptr || mDstTex == nullptr)
            {
                return T3D_ERR_FAIL;
            }
            mPlainRT = RenderTarget::create(mPlainTex);
            mDstRT = RenderTarget::create(mDstTex);
            return (mPlainRT && mDstRT) ? T3D_OK : T3D_ERR_FAIL;
        }

        TResult record() override
        {
            RHIContext *ctx = T3D_AGENT.getActiveRHIContext();

            // t0 右半边白
            ctx->setRenderTarget(mPlainRT.get());
            mDraw.bindStates(ctx);
            ctx->setViewport(kFullViewport);
            ctx->clearColor(ColorRGB::BLACK);
            ctx->setVertexShader(mDraw.vs());
            ctx->setPixelShader(mDraw.whitePS());
            ctx->setViewport(Viewport {0.5f, 0.0f, 0.5f, 1.0f, 0.0f, 1.0f});
            TResult ret = mDraw.draw(ctx);

            ctx->setRenderTarget(mDstRT.get());
            ctx->setViewport(kFullViewport);
            ctx->clearColor(ColorRGB::BLACK);
            ctx->beginPass();

            PixelBuffers textures;
            textures.push_back(mPlainTex->getPixelBuffer());
            textures.push_back(mArrayTex);
            Samplers samplers;
            samplers.push_back(mPoint);
            samplers.push_back(mLinear);

            ctx->setVertexShader(mDraw.vs());
            ctx->setPixelShader(mSamplePS.get());
            ctx->setPSPixelBuffers(0, textures);
            ctx->setPSSamplers(0, samplers);
            if (T3D_SUCCEEDED(ret))
            {
                ret = mDraw.draw(ctx);
            }

            ctx->endPass();

            if (T3D_SUCCEEDED(ret))
            {
                mHandle = mapColorRT(mDstTex.get());
                ret = mHandle.isValid() ? T3D_OK : T3D_ERR_FAIL;
            }
            ctx->reset();
            return ret;
        }

        TResult verify() override
        {
            Buffer data;
            TResult ret = unmapColorRT(mDstTex.get(), mHandle, data);
            if (T3D_FAILED(ret)) return ret;

            const int expected[4] = { 64, 255, 255, 191 };
            uint32_t bad = 0;
            for (uint32_t y = 0; y < kHeight; ++y)
            {
                for (uint32_t x = 0; x < kWidth; ++x)
                {
                    const uint8_t *p = pixelAt(data, kWidth, kHeight, x, y);
                    bool ok = (p != nullptr);
                    for (int c = 0; ok && c < 4; ++c)
                    {
                        ok = std::abs(int(p[c]) - expected[c]) <= 8;
                    }
                    if (ok) continue;

                    if (bad++ == 0)
                    {
                        if (p == nullptr)
                        {
                            APP_LOG_DEBUG("[%s] FAILED : readback row pitch too small", getName());
                        }
                        else
                        {
                            APP_LOG_DEBUG("[%s] FAILED : got (%d, %d, %d, %d) at (%u, %u) expected about (%d, %d, %d, %d)",
                                getName(), int(p[0]), int(p[1]), int(p[2]), int(p[3]), x, y,
                                expected[0], expected[1], expected[2], expected[3]);
                        }
                    }
                }
            }
            data.release();

            return bad == 0 ? T3D_OK : T3D_ERR_FAIL;
        }

    private:
        FullscreenDraw      mDraw;
        ShaderVariantPtr    mSamplePS;
        ShaderPtr           mDummyPS;
        SamplerStatePtr     mPoint;
        SamplerStatePtr     mLinear;
        PixelBuffer2DPtr    mArrayTex;
        RenderTexturePtr    mPlainTex;
        RenderTargetPtr     mPlainRT;
        RenderTexturePtr    mDstTex;
        RenderTargetPtr     mDstRT;
        ReadbackHandle      mHandle {};
    };


    /**
     * \brief K29：DepthBiasClamp（E.2.3）
     * \remarks 48x8 的颜色 RT 外挂 D24S8 深度，深度清成 0.6，在深度 0.5 处以 LESS、不写深度画绿，分 3 条竖带：
     *          竖带 0 不加偏移（绿）；竖带 1 常量偏移约 0.25（0.75 不通过，黑）；竖带 2 同样的偏移、
     *          DepthBiasClamp = 0.05（0.55 通过，绿）。UNORM24 的最小可分辨深度为 2^-24，常量偏移取 0.25 * 2^24。
     *          GL4 曾不读 DepthBiasClamp，竖带 2 读回黑。
     */
    class CaseDepthBiasClamp : public ComputeCase
    {
    public:
        ~CaseDepthBiasClamp() override
        {
            RenderTargetPtr none;
            releaseColorRT(mColor, mRT);
            releaseColorRT(mDepth, none);
        }

        static const uint32_t kWidth = 48;
        static const uint32_t kHeight = 8;
        static const uint32_t kStripes = 3;

        const char *getName() const override { return "K29 depth bias clamp"; }
        uint32_t getRequiredCaps() const override { return kNeedReadback; }
        uint32_t getBackendMask() const override { return kBackendD3D11 | kBackendGL4; }

        TResult setup() override
        {
            if (T3D_FAILED(mDraw.setup())) return T3D_ERR_FAIL;

            mVB = createFullscreenVB(ndcZ(0.5f));

            DepthStencilDesc less;
            less.DepthWriteEnable = false;
            less.DepthFunc = CompareFunction::kLess;
            mDepthLess = T3D_RENDER_STATE_MGR.loadDepthStencilState(less);

            const Real bias = Real(0.25 * 16777216.0);
            mRasterizers[0] = loadRasterizer(0.0f, 0.0f);
            mRasterizers[1] = loadRasterizer(bias, 0.0f);
            mRasterizers[2] = loadRasterizer(bias, 0.05f);

            mColor = createColorRT("K29-Color", kWidth, kHeight, PixelFormat::E_PF_R8G8B8A8, 1, kCPURead);
            mDepth = T3D_TEXTURE_MGR.createRenderTexture("K29-Depth", kWidth, kHeight,
                PixelFormat::E_PF_D24_UNORM_S8_UINT, 1, 1, 0, false, kCPUNone);
            if (!mVB || !mDepthLess || !mRasterizers[0] || !mRasterizers[1] || !mRasterizers[2]
                || mColor == nullptr || mDepth == nullptr)
            {
                return T3D_ERR_FAIL;
            }
            mRT = RenderTarget::create(mColor, mDepth);
            return (mRT != nullptr) ? T3D_OK : T3D_ERR_FAIL;
        }

        TResult record() override
        {
            RHIContext *ctx = T3D_AGENT.getActiveRHIContext();

            ctx->setRenderTarget(mRT.get());
            mDraw.bindStates(ctx);
            ctx->setViewport(kFullViewport);
            ctx->clearColor(ColorRGB::BLACK);
            ctx->clearDepthStencil(0.6f, 0);
            ctx->beginPass();
            ctx->setVertexShader(mDraw.vs());
            ctx->setPixelShader(mDraw.greenPS());
            ctx->setDepthStencilState(mDepthLess.get());
            bindVertexBuffer(ctx, mVB.get());

            TResult ret = T3D_OK;
            const Real w = Real(1) / Real(kStripes);
            for (uint32_t s = 0; s < kStripes && T3D_SUCCEEDED(ret); ++s)
            {
                ctx->setRasterizerState(mRasterizers[s].get());
                ctx->setViewport(Viewport {w * Real(s), 0.0f, w, 1.0f, 0.0f, 1.0f});
                ret = mDraw.draw(ctx);
            }
            ctx->endPass();

            if (T3D_SUCCEEDED(ret))
            {
                mHandle = mapColorRT(mColor.get());
                ret = mHandle.isValid() ? T3D_OK : T3D_ERR_FAIL;
            }
            ctx->reset();
            return ret;
        }

        TResult verify() override
        {
            Buffer data;
            TResult ret = unmapColorRT(mColor.get(), mHandle, data);
            if (T3D_FAILED(ret)) return ret;

            const char *expected = "GKG";
            const uint32_t stripeW = kWidth / kStripes;
            char got[kStripes + 1] = {};
            uint32_t bad = 0;
            for (uint32_t s = 0; s < kStripes; ++s)
            {
                const uint32_t x0 = s * stripeW + 2;
                const uint32_t x1 = (s + 1) * stripeW - 2;
                got[s] = '?';
                for (uint32_t y = 0; y < kHeight; ++y)
                {
                    for (uint32_t x = x0; x < x1; ++x)
                    {
                        const uint8_t *p = pixelAt(data, kWidth, kHeight, x, y);
                        const PixelClass c = (p != nullptr) ? classifyPixel(p) : PixelClass::kOther;
                        if (x == x0 && y == 0) got[s] = char(c);
                        if (char(c) != expected[s]) ++bad;
                    }
                }
            }
            data.release();

            if (bad != 0)
            {
                APP_LOG_DEBUG("[%s] FAILED : stripes got \"%s\" expected \"%s\" (%u pixels mismatch)",
                    getName(), got, expected, bad);
                return T3D_ERR_FAIL;
            }
            return T3D_OK;
        }

    private:
        static RasterizerStatePtr loadRasterizer(Real bias, Real clamp)
        {
            RasterizerDesc desc;
            desc.CullMode = CullingMode::kNone;
            desc.DepthBias = bias;
            desc.DepthBiasClamp = clamp;
            return T3D_RENDER_STATE_MGR.loadRasterizerState(desc);
        }

        FullscreenDraw          mDraw;
        VertexBufferPtr         mVB;
        DepthStencilStatePtr    mDepthLess;
        RasterizerStatePtr      mRasterizers[kStripes];
        RenderTexturePtr        mColor;
        RenderTexturePtr        mDepth;
        RenderTargetPtr         mRT;
        ReadbackHandle          mHandle {};
    };


    /**
     * \brief K30：缓冲种类的反射（E.2.4）
     * \remarks 只反射不派发。按名字逐个检查种类、绑定、元素字节数与 hasCounter，两个后端期望相同；
     *          GL 版的计数器块不应作为资源出现。GL4 曾只能反射出 Structured SRV / UAV，
     *          ByteAddress、Append、Consume 全部错判，scc 生成的块名也不是 HLSL 的资源名。
     */
    class CaseBufferKinds : public ComputeCase
    {
    public:
        const char *getName() const override { return "K30 buffer kinds reflect"; }
        uint32_t getRequiredCaps() const override { return kNeedCompute; }
        uint32_t getBackendMask() const override { return kBackendD3D11 | kBackendGL4; }

        TResult setup() override
        {
            mKernel = ComputeKernel::create("ComputeBufferKinds", bufferKindsSource(), 1, 1, 1);
            return mKernel != nullptr ? T3D_OK : T3D_ERR_FAIL;
        }

        TResult record() override { return T3D_OK; }

        TResult verify() override
        {
            using Kind = ShaderResourceParam::Kind;

            struct Expected
            {
                const char *name;
                Kind        kind;
                uint32_t    binding;
                uint32_t    stride;
                bool        hasCounter;
            };

            static const Expected kExpected[] =
            {
                { "gStructuredSRV", Kind::kStructuredSRV,  0, 16, false },
                { "gByteSRV",       Kind::kByteAddressSRV, 1, 0,  false },
                { "gByteSRV2",      Kind::kByteAddressSRV, 2, 0,  false },
                { "gStructuredUAV", Kind::kStructuredUAV,  0, 4,  false },
                { "gByteUAV",       Kind::kByteAddressUAV, 1, 0,  false },
                { "gCounted",       Kind::kStructuredUAV,  2, 4,  true  },
                { "gAppend",        Kind::kAppendUAV,      3, 16, true  },
                { "gConsume",       Kind::kConsumeUAV,     4, 16, true  },
            };

            const ShaderResourceParams &params = mKernel->getVariant()->getShaderResourceParams();

            uint32_t bad = 0;
            for (const Expected &e : kExpected)
            {
                auto it = params.find(e.name);
                if (it == params.end() || it->second == nullptr)
                {
                    APP_LOG_DEBUG("[%s] FAILED : '%s' not reflected", getName(), e.name);
                    ++bad;
                    continue;
                }

                const ShaderResourceParam *p = it->second.get();
                if (p->getKind() != e.kind || p->getBinding() != e.binding
                    || p->getElementStride() != e.stride || p->hasCounter() != e.hasCounter)
                {
                    APP_LOG_DEBUG("[%s] FAILED : '%s' got kind %u binding %u stride %u counter %d, expected %u / %u / %u / %d",
                        getName(), e.name, static_cast<uint32_t>(p->getKind()), p->getBinding(),
                        p->getElementStride(), p->hasCounter() ? 1 : 0,
                        static_cast<uint32_t>(e.kind), e.binding, e.stride, e.hasCounter ? 1 : 0);
                    ++bad;
                }
            }

            if (params.size() != std::size(kExpected))
            {
                for (const auto &kv : params)
                {
                    APP_LOG_DEBUG("[%s] reflected '%s'", getName(), kv.first.c_str());
                }
                APP_LOG_DEBUG("[%s] FAILED : %zu resources reflected, expected %zu",
                    getName(), params.size(), std::size(kExpected));
                ++bad;
            }

            return bad == 0 ? T3D_OK : T3D_ERR_FAIL;
        }

    private:
        ComputeKernelPtr mKernel;
    };


    /**
     * \brief K31：scc 生成的 compute shader 按 HLSL 寄存器号绑定 SRV / UAV 与计数器（E.2.4 遗留）
     * \remarks 256 个线程：gOutput[i] = gInput[i] ^ 0x5A5A5A5A；偶数线程 Append(i)；
     *          每个线程 Consume 一个值 c，再 IncrementCounter 写入 c + 1。gConsume 初始为 1000 + k，计数 256。
     *          两个后端都按 HLSL 寄存器号设置槽位（不走 uavSlot），期望 Append 计数 128、Consume 计数 0、
     *          IncrementCounter 计数 256，内容按集合校验。
     *          GL 版是 scc 原样输出，t0 与 u0 的 SSBO binding 都是 0，计数器是 DXC 另行分配 binding 的 SSBO。
     *          GL4 曾直接把槽位当 SSBO 绑定点、把计数器绑成 atomic counter，gInput 被 gOutput 覆盖，三个计数器都读写不到
     */
    class CaseBufferCounters : public ComputeCase
    {
    public:
        static const uint32_t kElemCount = 256;

        const char *getName() const override { return "K31 scc buffer slots and counters"; }
        uint32_t getRequiredCaps() const override
        {
            return kNeedCompute | kNeedUnorderedAccess | kNeedStructuredBuffer
                | kNeedAppendConsume | kNeedReadback;
        }
        uint32_t getBackendMask() const override { return kBackendD3D11 | kBackendGL4; }

        TResult setup() override
        {
            mKernel = ComputeKernel::create("ComputeBufferCounters", bufferCountersSource(), 64);
            if (mKernel == nullptr) return T3D_ERR_FAIL;

            Buffer input = allocBytes(sizeof(uint32_t) * kElemCount);
            Buffer consume = allocBytes(sizeof(uint32_t) * kElemCount);
            uint32_t *in = reinterpret_cast<uint32_t *>(input.Data);
            uint32_t *co = reinterpret_cast<uint32_t *>(consume.Data);
            for (uint32_t i = 0; i < kElemCount; ++i)
            {
                in[i] = i * 3u + 1u;
                co[i] = 1000u + i;
            }

            mInput = loadSB(StructuredBufferKind::kStructured, 4, kElemCount, kGPUShaderResource, kCPUNone, input);
            mOutput = loadSB(StructuredBufferKind::kStructured, 4, kElemCount, kGPUUnorderedAccess, kCPURead);
            mAppend = loadAppendConsume(kCPURead, Buffer());
            mConsume = loadAppendConsume(kCPUNone, consume);
            mCounted = loadSB(StructuredBufferKind::kStructured, 4, kElemCount, kGPUUnorderedAccess, kCPURead,
                Buffer(), PixelFormat::E_PF_UNKNOWN, true);
            mCounts = loadSB(StructuredBufferKind::kByteAddress, 4, 3, kGPUIndirectArgs | kGPUShaderResource, kCPURead);

            input.Data = nullptr;
            input.DataSize = 0;
            consume.Data = nullptr;
            consume.DataSize = 0;
            return (mInput && mOutput && mAppend && mConsume && mCounted && mCounts) ? T3D_OK : T3D_ERR_FAIL;
        }

        TResult record() override
        {
            RHIContext *ctx = T3D_AGENT.getActiveRHIContext();
            TResult ret = ctx->setComputeShader(mKernel->getVariant());
            if (T3D_FAILED(ret)) return ret;

            ShaderResourceBuffers srvs; srvs.push_back(mInput);
            ret = ctx->setCSStructuredBuffers(0, srvs);
            if (T3D_FAILED(ret)) return ret;

            UnorderedAccessBuffers uavs;
            uavs.push_back(mOutput);
            uavs.push_back(mAppend);
            uavs.push_back(mConsume);
            uavs.push_back(mCounted);
            UAVInitialCounts counts;
            counts.push_back(kKeepUAVCounter);
            counts.push_back(0);
            counts.push_back(kElemCount);
            counts.push_back(0);
            ret = ctx->setCSUnorderedAccessBuffers(0, uavs, counts);
            if (T3D_FAILED(ret)) return ret;

            ret = mKernel->dispatchThreads(kElemCount);
            if (T3D_FAILED(ret)) return ret;
            ctx->uavBarrier(uavs);

            ret = ctx->copyStructureCount(mCounts.get(), 0, mAppend.get());
            if (T3D_SUCCEEDED(ret)) ret = ctx->copyStructureCount(mCounts.get(), 4, mConsume.get());
            if (T3D_SUCCEEDED(ret)) ret = ctx->copyStructureCount(mCounts.get(), 8, mCounted.get());
            if (T3D_FAILED(ret)) return ret;

            UnorderedAccessBuffers none(uavs.size());
            ctx->setCSUnorderedAccessBuffers(0, none);
            ShaderResourceBuffers noSrv(1);
            ctx->setCSStructuredBuffers(0, noSrv);
            ctx->setComputeShader(nullptr);

            ret = mapWhole(mOutput.get(), mOutputHandle);
            if (T3D_SUCCEEDED(ret)) ret = mapWhole(mAppend.get(), mAppendHandle);
            if (T3D_SUCCEEDED(ret)) ret = mapWhole(mCounted.get(), mCountedHandle);
            if (T3D_SUCCEEDED(ret)) ret = mapWhole(mCounts.get(), mCountsHandle);
            return ret;
        }

        TResult verify() override
        {
            const size_t bytes = sizeof(uint32_t) * kElemCount;
            Buffer output, append, counted, counts;
            TResult ret = unmapCheckSize(mOutput.get(), mOutputHandle, output, bytes);
            if (T3D_SUCCEEDED(ret)) ret = unmapCheckSize(mAppend.get(), mAppendHandle, append, bytes);
            if (T3D_SUCCEEDED(ret)) ret = unmapCheckSize(mCounted.get(), mCountedHandle, counted, bytes);
            if (T3D_SUCCEEDED(ret)) ret = unmapCheckSize(mCounts.get(), mCountsHandle, counts, sizeof(uint32_t) * 3);

            if (T3D_SUCCEEDED(ret))
            {
                ret = check(reinterpret_cast<const uint32_t *>(output.Data),
                    reinterpret_cast<const uint32_t *>(append.Data),
                    reinterpret_cast<const uint32_t *>(counted.Data),
                    reinterpret_cast<const uint32_t *>(counts.Data));
            }

            output.release();
            append.release();
            counted.release();
            counts.release();
            return ret;
        }

    private:
        static StructuredBufferPtr loadAppendConsume(uint32_t cpuAccess, const Buffer &init)
        {
            StructuredBufferDesc desc;
            desc.kind = StructuredBufferKind::kStructured;
            desc.elementSize = sizeof(uint32_t);
            desc.elementCount = kElemCount;
            desc.isAppendConsume = true;
            desc.buffer = init;
            return T3D_RENDER_BUFFER_MGR.loadStructuredBuffer(
                desc, MemoryType::kVRAM, Usage::kStatic, cpuAccess, kGPUUnorderedAccess);
        }

        TResult check(const uint32_t *output, const uint32_t *append, const uint32_t *counted, const uint32_t *counts) const
        {
            uint32_t bad = 0;
            for (uint32_t i = 0; i < kElemCount; ++i)
            {
                const uint32_t expected = (i * 3u + 1u) ^ 0x5A5A5A5Au;
                if (output[i] != expected)
                {
                    if (bad < 4)
                    {
                        APP_LOG_DEBUG("[%s] FAILED : output[%u]=0x%08X expected 0x%08X", getName(), i, output[i], expected);
                    }
                    ++bad;
                }
            }

            const uint32_t appendCount = counts[0];
            const uint32_t consumeCount = counts[1];
            const uint32_t countedCount = counts[2];
            if (appendCount != kElemCount / 2 || consumeCount != 0 || countedCount != kElemCount)
            {
                APP_LOG_DEBUG("[%s] FAILED : counters append=%u consume=%u counted=%u, expected %u / 0 / %u",
                    getName(), appendCount, consumeCount, countedCount, kElemCount / 2, kElemCount);
                ++bad;
            }

            // 顺序由线程调度决定，按集合比对
            std::vector<uint32_t> got(append, append + kElemCount / 2);
            std::sort(got.begin(), got.end());
            for (uint32_t k = 0; k < kElemCount / 2; ++k)
            {
                if (got[k] != k * 2u)
                {
                    APP_LOG_DEBUG("[%s] FAILED : sorted append[%u]=%u expected %u", getName(), k, got[k], k * 2u);
                    ++bad;
                    break;
                }
            }

            got.assign(counted, counted + kElemCount);
            std::sort(got.begin(), got.end());
            for (uint32_t k = 0; k < kElemCount; ++k)
            {
                if (got[k] != 1001u + k)
                {
                    APP_LOG_DEBUG("[%s] FAILED : sorted counted[%u]=%u expected %u", getName(), k, got[k], 1001u + k);
                    ++bad;
                    break;
                }
            }

            return bad == 0 ? T3D_OK : T3D_ERR_FAIL;
        }

        ComputeKernelPtr    mKernel;
        StructuredBufferPtr mInput;
        StructuredBufferPtr mOutput;
        StructuredBufferPtr mAppend;
        StructuredBufferPtr mConsume;
        StructuredBufferPtr mCounted;
        StructuredBufferPtr mCounts;
        ReadbackHandle      mOutputHandle {};
        ReadbackHandle      mAppendHandle {};
        ReadbackHandle      mCountedHandle {};
        ReadbackHandle      mCountsHandle {};
    };


    /**
     * \brief K32：顶点属性按语义对到 VS 输入（E.2.5）
     * \remarks 交错顶点为 POSITION(float3) + TEXCOORD0(float2) + COLOR0(float4)，顶点色为绿。
     *          声明按 COLOR0、TEXCOORD0、POSITION 的顺序排列，TEXCOORD0 是 VS 不读的多余属性。
     *          同一个声明画 3 条竖带，依次配 VS1（输入 POSITION、COLOR0，输出顶点色）、VS2（只读 POSITION，
     *          输出常量绿）、VS1，覆盖 GL4 换输入签名后重设 VAO、再切回来重新启用 COLOR0。GL 版都是 scc 的原样输出。
     *          D3D11 的输入布局按创建时关联的 VS 的签名把语义对到寄存器，VS2 的签名是 VS1 的前缀，可以复用；
     *          输入顺序与 VS1 不同的 VS 在 D3D11 上也会读错，不在本用例范围内。
     *          GL4 曾把属性下标当作 location，VS 读到的位置都是 COLOR0 这个常量，三角形退化，读回 "KKK"。
     */
    class CaseVertexSemantics : public ComputeCase
    {
    public:
        ~CaseVertexSemantics() override
        {
            releaseColorRT(mTex, mRT);
        }

        static const uint32_t kWidth = 24;
        static const uint32_t kHeight = 8;
        static const uint32_t kStripes = 3;
        static const uint32_t kFloatsPerVertex = 9;

        const char *getName() const override { return "K32 vertex attributes by semantic"; }
        uint32_t getRequiredCaps() const override { return kNeedReadback; }
        uint32_t getBackendMask() const override { return kBackendD3D11 | kBackendGL4; }

        TResult setup() override
        {
            if (T3D_FAILED(mDraw.setup())) return T3D_ERR_FAIL;

            mVS[0] = ComputeKernel::compileGraphics("SemanticPositionColorVS", SHADER_STAGE::kVertex,
                semanticPositionColorVSSource(), mDummyVS[0]);
            mVS[1] = ComputeKernel::compileGraphics("SemanticPositionOnlyVS", SHADER_STAGE::kVertex,
                semanticPositionOnlyVSSource(), mDummyVS[1]);
            mPS = ComputeKernel::compileGraphics("VertexColorPS", SHADER_STAGE::kPixel,
                vertexColorPSSource(), mDummyPS);
            if (mVS[0] == nullptr || mVS[1] == nullptr || mPS == nullptr) return T3D_ERR_FAIL;

            VertexAttributes attribs;
            attribs.push_back(VertexAttribute(0, sizeof(float) * 5, VertexAttribute::Type::E_VAT_FLOAT4,
                VertexAttribute::Semantic::E_VAS_DIFFUSE, 0));
            attribs.push_back(VertexAttribute(0, sizeof(float) * 3, VertexAttribute::Type::E_VAT_FLOAT2,
                VertexAttribute::Semantic::E_VAS_TEXCOORD, 0));
            attribs.push_back(VertexAttribute(0, 0, VertexAttribute::Type::E_VAT_FLOAT3,
                VertexAttribute::Semantic::E_VAS_POSITION, 0));
            mDecl = T3D_RENDER_BUFFER_MGR.addVertexDeclaration(attribs, mVS[0].get());

            const float verts[3 * kFloatsPerVertex] = {
                -1.0f, -1.0f, 0.0f,   0.5f, 0.5f,   0.0f, 1.0f, 0.0f, 1.0f,
                 3.0f, -1.0f, 0.0f,   0.5f, 0.5f,   0.0f, 1.0f, 0.0f, 1.0f,
                -1.0f,  3.0f, 0.0f,   0.5f, 0.5f,   0.0f, 1.0f, 0.0f, 1.0f,
            };
            Buffer vb = allocBytes(sizeof(verts));
            memcpy(vb.Data, verts, sizeof(verts));
            mVB = T3D_RENDER_BUFFER_MGR.loadVertexBuffer(sizeof(float) * kFloatsPerVertex, 3, vb,
                MemoryType::kBoth, Usage::kImmutable, kCPUNone);
            vb.release();

            mTex = createColorRT("K32-Color", kWidth, kHeight, PixelFormat::E_PF_R8G8B8A8, 1, kCPURead);
            if (mDecl == nullptr || mVB == nullptr || mTex == nullptr) return T3D_ERR_FAIL;
            mRT = RenderTarget::create(mTex);
            return mRT ? T3D_OK : T3D_ERR_FAIL;
        }

        TResult record() override
        {
            RHIContext *ctx = T3D_AGENT.getActiveRHIContext();

            ctx->setRenderTarget(mRT.get());
            mDraw.bindStates(ctx);
            ctx->setViewport(kFullViewport);
            ctx->clearColor(ColorRGB::BLACK);
            ctx->beginPass();

            ctx->setVertexDeclaration(mDecl.get());
            VertexBuffers vbs;
            vbs.push_back(mVB);
            VertexStrides strides;
            strides.push_back(sizeof(float) * kFloatsPerVertex);
            VertexOffsets offsets;
            offsets.push_back(0);
            ctx->setVertexBuffers(0, vbs, strides, offsets);
            ctx->setPixelShader(mPS.get());

            TResult ret = T3D_OK;
            const Real w = Real(1) / Real(kStripes);
            for (uint32_t s = 0; s < kStripes && T3D_SUCCEEDED(ret); ++s)
            {
                ctx->setVertexShader(mVS[s == 1 ? 1 : 0].get());
                ctx->setViewport(Viewport {w * Real(s), 0.0f, w, 1.0f, 0.0f, 1.0f});
                ret = ctx->render(3, 0);
            }
            ctx->endPass();

            if (T3D_SUCCEEDED(ret))
            {
                mHandle = mapColorRT(mTex.get());
                ret = mHandle.isValid() ? T3D_OK : T3D_ERR_FAIL;
            }
            ctx->reset();
            return ret;
        }

        TResult verify() override
        {
            Buffer data;
            TResult ret = unmapColorRT(mTex.get(), mHandle, data);
            if (T3D_FAILED(ret)) return ret;

            const char *expected = "GGG";
            const uint32_t stripeW = kWidth / kStripes;
            char got[kStripes + 1] = {};
            uint32_t bad = 0;
            for (uint32_t s = 0; s < kStripes; ++s)
            {
                const uint32_t x0 = s * stripeW + 1;
                const uint32_t x1 = (s + 1) * stripeW - 1;
                got[s] = '?';
                for (uint32_t y = 0; y < kHeight; ++y)
                {
                    for (uint32_t x = x0; x < x1; ++x)
                    {
                        const uint8_t *p = pixelAt(data, kWidth, kHeight, x, y);
                        const PixelClass c = (p != nullptr) ? classifyPixel(p) : PixelClass::kOther;
                        if (x == x0 && y == 0) got[s] = char(c);
                        if (char(c) != expected[s]) ++bad;
                    }
                }
            }
            data.release();

            if (bad != 0)
            {
                APP_LOG_DEBUG("[%s] FAILED : stripes got \"%s\" expected \"%s\" (%u pixels mismatch)",
                    getName(), got, expected, bad);
                return T3D_ERR_FAIL;
            }
            return T3D_OK;
        }

    private:
        FullscreenDraw          mDraw;
        ShaderVariantPtr        mVS[2];
        ShaderVariantPtr        mPS;
        ShaderPtr               mDummyVS[2];
        ShaderPtr               mDummyPS;
        VertexDeclarationPtr    mDecl;
        VertexBufferPtr         mVB;
        RenderTexturePtr        mTex;
        RenderTargetPtr         mRT;
        ReadbackHandle          mHandle {};
    };


    /**
     * \brief K33：创建资源时的参数校验（E.2.6）
     * \remarks 非法描述期望后端返回空的 RHI 资源，合法的对照组期望非空：
     *          1. Usage / CPUAccessMode 组合：VB kDynamic + kCPURead、VB kImmutable + kCPUWrite、IB kCopy + kCPUNone、
     *             CB kDynamic + kCPUReadWrite、2D 纹理 kDynamic + kCPURead、RT kImmutable + kCPUWrite；
     *             对照组 VB kDynamic + kCPUWrite、IB kStatic + kCPURead、CB kDynamic + kCPUWrite。
     *          2. 尺寸为 0：1D 宽 0、2D 高 0、3D 深 0、cubemap 宽 0、RT 宽 0，0 字节的 VB / IB / CB；
     *             对照组 4x4 的 2D 纹理与 RT。
     *          3. 格式：2D 纹理 E_PF_UNKNOWN 两个后端都拒绝；E_PF_PALETTE8 在 D3D11 上没有对应的 DXGI 格式，
     *             期望拒绝，GL4 映射为 GL_R8，期望创建成功。
     *          4. 把被拒绝的 VB / IB 交给 setVertexBuffers / setIndexBuffer，期望按解绑处理，不崩溃。
     *          5. 采样数：颜色 RT、深度 RT、2D 纹理与 2D 纹理数组请求 64x 期望拒绝，颜色 RT、深度 RT、2D 纹理 4x 期望成功。
     *             修复前 D3D11 的 RT 在 RHI 线程上因 quality 下溢创建失败、2D 纹理在 RHI 线程上 CreateTexture2D 失败，
     *             GL4 的 RT 不检查采样数，主线程都拿到非空对象。
     *          修复前 GL4 不读 CPUAccessMode、不查尺寸，未知格式回落为 GL_RGBA8，全部返回非空；
     *          D3D11 的 VB / IB / CB 校验失败时同样返回空壳对象，RT 的 Usage 校验与纹理格式错误在 RHI 线程上才暴露。
     */
    class CaseCreateValidation : public ComputeCase
    {
    public:
        const char *getName() const override { return "K33 create parameter validation"; }
        uint32_t getRequiredCaps() const override { return 0; }
        uint32_t getBackendMask() const override { return kBackendD3D11 | kBackendGL4; }

        TResult setup() override
        {
            mFailures = 0;

            auto &mgr = T3D_RENDER_BUFFER_MGR;

            mRejectedVB = checkVB(Usage::kDynamic, kCPURead, false, "VB kDynamic + kCPURead");
            checkVB(Usage::kImmutable, kCPUWrite, false, "VB kImmutable + kCPUWrite");
            checkVB(Usage::kDynamic, kCPUWrite, true, "VB kDynamic + kCPUWrite");

            mRejectedIB = checkIB(Usage::kCopy, kCPUNone, false, "IB kCopy + kCPUNone");
            checkIB(Usage::kStatic, kCPURead, true, "IB kStatic + kCPURead");

            checkCB(Usage::kDynamic, kCPUReadWrite, false, "CB kDynamic + kCPUReadWrite");
            checkCB(Usage::kDynamic, kCPUWrite, true, "CB kDynamic + kCPUWrite");

            // 空 Buffer 的 Data 为空，kVRAM 下引擎接管的也是空指针
            keep(mgr.loadVertexBuffer(sizeof(float) * 3, 0, Buffer(), MemoryType::kVRAM, Usage::kStatic, kCPUNone),
                false, "VB 0 bytes");
            keep(mgr.loadIndexBuffer(IndexType::E_IT_16BITS, 0, Buffer(), MemoryType::kVRAM, Usage::kStatic, kCPUNone),
                false, "IB 0 bytes");
            keep(mgr.loadConstantBuffer("K33CBEmpty", 0, Buffer(), MemoryType::kVRAM, Usage::kDynamic, kCPUWrite),
                false, "CB 0 bytes");

            // PixelBuffer 只保存描述的指针，缓存里的对象可能活得比本用例久，描述用静态存储
            static PixelBuffer2DDesc sTex2D = make2D(4, 4, PixelFormat::E_PF_R8G8B8A8);
            static PixelBuffer2DDesc sTex2DZero = make2D(4, 0, PixelFormat::E_PF_R8G8B8A8);
            static PixelBuffer2DDesc sTex2DUnknown = make2D(4, 4, PixelFormat::E_PF_UNKNOWN);
            static PixelBuffer2DDesc sTex2DPalette = make2D(4, 4, PixelFormat::E_PF_PALETTE8);
            static PixelBuffer2DDesc sCubeZero = make2D(0, 0, PixelFormat::E_PF_R8G8B8A8, PixelBufferCubemap::FACE_COUNT);
            static PixelBuffer2DDesc sRT = make2D(4, 4, PixelFormat::E_PF_R8G8B8A8);
            static PixelBuffer2DDesc sRTZero = make2D(0, 4, PixelFormat::E_PF_R8G8B8A8);
            static PixelBuffer1DDesc sTex1DZero = []()
            {
                PixelBuffer1DDesc desc;
                desc.width = 0;
                desc.mipmaps = 1;
                desc.arraySize = 1;
                desc.format = PixelFormat::E_PF_R8G8B8A8;
                return desc;
            }();
            static PixelBuffer3DDesc sTex3DZero = []()
            {
                PixelBuffer3DDesc desc;
                desc.width = 4;
                desc.height = 4;
                desc.depth = 0;
                desc.format = PixelFormat::E_PF_R8G8B8A8;
                return desc;
            }();

            keep(mgr.loadPixelBuffer2D(&sTex2D, MemoryType::kVRAM, Usage::kStatic, kCPUNone), true, "2D 4x4");
            keep(mgr.loadPixelBuffer2D(&sTex2D, MemoryType::kVRAM, Usage::kDynamic, kCPURead), false, "2D kDynamic + kCPURead");
            keep(mgr.loadPixelBuffer2D(&sTex2DZero, MemoryType::kVRAM, Usage::kStatic, kCPUNone), false, "2D 4x0");
            keep(mgr.loadPixelBuffer1D(&sTex1DZero, MemoryType::kVRAM, Usage::kStatic, kCPUNone), false, "1D width 0");
            keep(mgr.loadPixelBuffer3D(&sTex3DZero, MemoryType::kVRAM, Usage::kStatic, kCPUNone), false, "3D 4x4x0");
            keep(mgr.loadPixelBufferCubemap(&sCubeZero, MemoryType::kVRAM, Usage::kStatic, kCPUNone), false, "cubemap 0x0");
            keep(mgr.loadRenderTexture(&sRT, MemoryType::kVRAM, Usage::kStatic, kCPURead), true, "RT 4x4");
            keep(mgr.loadRenderTexture(&sRT, MemoryType::kVRAM, Usage::kImmutable, kCPUWrite), false, "RT kImmutable + kCPUWrite");
            keep(mgr.loadRenderTexture(&sRTZero, MemoryType::kVRAM, Usage::kStatic, kCPUNone), false, "RT 0x4");

            keep(mgr.loadPixelBuffer2D(&sTex2DUnknown, MemoryType::kVRAM, Usage::kStatic, kCPUNone), false, "2D E_PF_UNKNOWN");
            keep(mgr.loadPixelBuffer2D(&sTex2DPalette, MemoryType::kVRAM, Usage::kStatic, kCPUNone),
                currentBackendBit() == kBackendGL4, "2D E_PF_PALETTE8");

            // 采样数：两个后端都没有支持 64x 的格式，期望拒绝；4x 为对照组
            static PixelBuffer2DDesc sRTColorMS4 = make2D(4, 4, PixelFormat::E_PF_R8G8B8A8, 1, 4);
            static PixelBuffer2DDesc sRTColorMS64 = make2D(4, 4, PixelFormat::E_PF_R8G8B8A8, 1, 64);
            static PixelBuffer2DDesc sRTDepthMS4 = make2D(4, 4, PixelFormat::E_PF_D24_UNORM_S8_UINT, 1, 4);
            static PixelBuffer2DDesc sRTDepthMS64 = make2D(4, 4, PixelFormat::E_PF_D24_UNORM_S8_UINT, 1, 64);
            static PixelBuffer2DDesc sTex2DMS4 = make2D(4, 4, PixelFormat::E_PF_R8G8B8A8, 1, 4);
            static PixelBuffer2DDesc sTex2DMS64 = make2D(4, 4, PixelFormat::E_PF_R8G8B8A8, 1, 64);
            static PixelBuffer2DDesc sTex2DArrayMS64 = make2D(4, 4, PixelFormat::E_PF_R8G8B8A8, 2, 64);

            keep(mgr.loadRenderTexture(&sRTColorMS4, MemoryType::kVRAM, Usage::kStatic, kCPUNone), true, "color RT 4x MSAA");
            keep(mgr.loadRenderTexture(&sRTColorMS64, MemoryType::kVRAM, Usage::kStatic, kCPUNone), false, "color RT 64x MSAA");
            keep(mgr.loadRenderTexture(&sRTDepthMS4, MemoryType::kVRAM, Usage::kStatic, kCPUNone), true, "depth RT 4x MSAA");
            keep(mgr.loadRenderTexture(&sRTDepthMS64, MemoryType::kVRAM, Usage::kStatic, kCPUNone), false, "depth RT 64x MSAA");
            keep(mgr.loadPixelBuffer2D(&sTex2DMS4, MemoryType::kVRAM, Usage::kStatic, kCPUNone), true, "2D 4x MSAA");
            keep(mgr.loadPixelBuffer2D(&sTex2DMS64, MemoryType::kVRAM, Usage::kStatic, kCPUNone), false, "2D 64x MSAA");
            keep(mgr.loadPixelBuffer2D(&sTex2DArrayMS64, MemoryType::kVRAM, Usage::kStatic, kCPUNone), false, "2D array 64x MSAA");

            return T3D_OK;
        }

        TResult record() override
        {
            if (mRejectedVB == nullptr || mRejectedIB == nullptr)
            {
                APP_LOG_DEBUG("[%s] FAILED : rejected VB / IB engine objects are missing", getName());
                ++mFailures;
                return T3D_OK;
            }

            RHIContext *ctx = T3D_AGENT.getActiveRHIContext();
            VertexBuffers vbs;
            vbs.push_back(mRejectedVB);
            VertexStrides strides;
            strides.push_back(sizeof(float) * 3);
            VertexOffsets offsets;
            offsets.push_back(0);
            expect(ctx->setVertexBuffers(0, vbs, strides, offsets), "setVertexBuffers with rejected VB");
            expect(ctx->setIndexBuffer(mRejectedIB.get()), "setIndexBuffer with rejected IB");
            ctx->reset();
            return T3D_OK;
        }

        TResult verify() override
        {
            return mFailures == 0 ? T3D_OK : T3D_ERR_FAIL;
        }

    private:
        static PixelBuffer2DDesc make2D(uint32_t width, uint32_t height, PixelFormat format, uint32_t arraySize = 1,
            uint32_t samples = 1)
        {
            PixelBuffer2DDesc desc;
            desc.width = width;
            desc.height = height;
            desc.mipmaps = 1;
            desc.arraySize = arraySize;
            desc.format = format;
            desc.sampleDesc.Count = samples;
            return desc;
        }

        void keep(const RenderBufferPtr &buffer, bool expectCreated, const char *what)
        {
            const bool created = (buffer != nullptr && buffer->getRHIResource() != nullptr);
            if (created != expectCreated)
            {
                APP_LOG_DEBUG("[%s] FAILED : %s %s", getName(), what,
                    expectCreated ? "was rejected, expected created" : "was created, expected rejected");
                ++mFailures;
            }
            mBuffers.push_back(buffer);
        }

        void expect(TResult got, const char *what)
        {
            if (got != T3D_OK)
            {
                APP_LOG_DEBUG("[%s] FAILED : %s returned %d, expected T3D_OK", getName(), what, (int)got);
                ++mFailures;
            }
        }

        VertexBufferPtr checkVB(Usage usage, uint32_t accMode, bool expectCreated, const char *what)
        {
            Buffer data = allocBytes(sizeof(float) * 3 * 3);
            VertexBufferPtr vb = T3D_RENDER_BUFFER_MGR.loadVertexBuffer(sizeof(float) * 3, 3, data,
                MemoryType::kBoth, usage, accMode);
            data.release();
            keep(vb, expectCreated, what);
            return vb;
        }

        IndexBufferPtr checkIB(Usage usage, uint32_t accMode, bool expectCreated, const char *what)
        {
            Buffer data = allocBytes(sizeof(uint16_t) * 3);
            IndexBufferPtr ib = T3D_RENDER_BUFFER_MGR.loadIndexBuffer(IndexType::E_IT_16BITS, 3, data,
                MemoryType::kBoth, usage, accMode);
            data.release();
            keep(ib, expectCreated, what);
            return ib;
        }

        void checkCB(Usage usage, uint32_t accMode, bool expectCreated, const char *what)
        {
            Buffer data = allocBytes(16);
            keep(T3D_RENDER_BUFFER_MGR.loadConstantBuffer("K33CB", 0, data, MemoryType::kBoth, usage, accMode),
                expectCreated, what);
            data.release();
        }

        std::vector<RenderBufferPtr>    mBuffers;
        VertexBufferPtr                 mRejectedVB;
        IndexBufferPtr                  mRejectedIB;
        uint32_t                        mFailures {0};
    };


    /**
     * \brief K34：多个读回同时在途时 staging 互不覆盖（E.2.7）
     * \remarks GL4 与 D3D11 的读回都从 staging 池租中转资源，map 时租、unmap 时还。
     *          同一帧内发起 5 个读回：16 B、1000 B、16 KB 三个结构化缓冲各读整块，16 KB 那块再读
     *          [4 KB, 4 KB + 400 B) 的部分区间，外加 compute（复用 K5 的 kernel）写满的 8x8 纹理；
     *          unmap 按与 map 不同的顺序进行，逐个校验内容。若同时在途的请求租到同一块，后拷入的数据
     *          会覆盖先拷入的。自检时全部用例在同一帧 map，首轮读回都是新建的 staging；复用发生在
     *          按 R 重跑时，那时池里留有上一轮归还的条目。
     */
    class CaseReadbackStagingPool : public ComputeCase
    {
    public:
        static const uint32_t kTexSize = 8;

        const char *getName() const override { return "K34 readback staging pool"; }
        uint32_t getRequiredCaps() const override
        {
            return kNeedCompute | kNeedUnorderedAccess | kNeedStructuredBuffer | kNeedReadback;
        }
        uint32_t getBackendMask() const override { return kBackendD3D11 | kBackendGL4; }

        TResult setup() override
        {
            mKernel = ComputeKernel::create("ComputeTextureWrite", textureWriteSource(), 8, 8, 1);
            if (mKernel == nullptr) return T3D_ERR_FAIL;

            const uint32_t counts[kBufferCount] = { 4, 250, 4096 };
            for (uint32_t i = 0; i < kBufferCount; ++i)
            {
                mCounts[i] = counts[i];
                Buffer init = allocBytes(sizeof(uint32_t) * counts[i]);
                uint32_t *p = reinterpret_cast<uint32_t *>(init.Data);
                for (uint32_t j = 0; j < counts[i]; ++j)
                {
                    p[j] = pattern(i, j);
                }
                mBuffers[i] = loadSB(StructuredBufferKind::kStructured, sizeof(uint32_t), counts[i],
                    kGPUShaderResource, kCPURead, init);
                // kVRAM 会接管 init.Data，这里只把指针摘掉以免 double-free
                init.Data = nullptr;
                init.DataSize = 0;
                if (mBuffers[i] == nullptr) return T3D_ERR_FAIL;
            }

            static PixelBuffer2DDesc sDesc = []()
            {
                PixelBuffer2DDesc desc;
                desc.width = kTexSize;
                desc.height = kTexSize;
                desc.mipmaps = 1;
                desc.arraySize = 1;
                desc.format = PixelFormat::E_PF_R8G8B8A8;
                desc.gpuAccess = kGPUUnorderedAccess;
                desc.shaderReadable = true;
                return desc;
            }();
            mTex = T3D_RENDER_BUFFER_MGR.loadPixelBuffer2D(&sDesc, MemoryType::kVRAM, Usage::kStatic, kCPURead);
            return mTex != nullptr ? T3D_OK : T3D_ERR_FAIL;
        }

        TResult record() override
        {
            RHIContext *ctx = T3D_AGENT.getActiveRHIContext();
            TResult ret = ctx->setComputeShader(mKernel->getVariant());
            if (T3D_FAILED(ret)) return ret;

            UnorderedAccessBuffers uavs; uavs.push_back(mTex);
            ret = ctx->setCSUnorderedAccessBuffers(0, uavs);
            if (T3D_FAILED(ret)) return ret;
            ret = mKernel->dispatchThreads(kTexSize, kTexSize);
            if (T3D_FAILED(ret)) return ret;
            ctx->uavBarrier(uavs);
            ctx->setComputeShader(nullptr);

            mHandles[0] = mBuffers[0]->map(0, 0);

            ReadbackRegion region;
            region.size = Vector3(Real(kTexSize), Real(kTexSize), Real(1));
            mTexHandle = ctx->map(mTex.get(), region);

            mHandles[1] = mBuffers[1]->map(0, 0);
            mHandles[2] = mBuffers[2]->map(0, 0);
            mPartialHandle = mBuffers[2]->map(kPartialOffset, kPartialSize);

            if (!mHandles[0].isValid() || !mHandles[1].isValid() || !mHandles[2].isValid()
                || !mTexHandle.isValid() || !mPartialHandle.isValid())
            {
                return T3D_ERR_FAIL;
            }
            return T3D_OK;
        }

        TResult verify() override
        {
            uint32_t failures = 0;

            failures += verifyBuffer(2, mHandles[2], 0, mCounts[2]);
            failures += verifyBuffer(0, mHandles[0], 0, mCounts[0]);
            failures += verifyTexture();
            failures += verifyBuffer(2, mPartialHandle, kPartialOffset / sizeof(uint32_t), kPartialSize / sizeof(uint32_t));
            failures += verifyBuffer(1, mHandles[1], 0, mCounts[1]);

            return failures == 0 ? T3D_OK : T3D_ERR_FAIL;
        }

    private:
        static const uint32_t kBufferCount = 3;
        static const size_t kPartialOffset = 4096;
        static const size_t kPartialSize = 400;

        static uint32_t pattern(uint32_t buffer, uint32_t index)
        {
            return ((buffer + 1) << 24) | index;
        }

        uint32_t verifyBuffer(uint32_t buffer, ReadbackHandle &handle, uint32_t first, uint32_t count)
        {
            Buffer data;
            TResult ret = unmapCheckSize(mBuffers[buffer].get(), handle, data, sizeof(uint32_t) * count);
            if (T3D_FAILED(ret))
            {
                APP_LOG_DEBUG("[%s] FAILED : buffer %u [%u, +%u) unmap returned %d", getName(), buffer, first, count, (int)ret);
                return 1;
            }

            const uint32_t *out = reinterpret_cast<const uint32_t *>(data.Data);
            uint32_t mismatches = 0;
            uint32_t bad = 0, got = 0;
            for (uint32_t j = 0; j < count; ++j)
            {
                if (out[j] != pattern(buffer, first + j))
                {
                    if (mismatches == 0) { bad = j; got = out[j]; }
                    ++mismatches;
                }
            }
            data.release();

            if (mismatches != 0)
            {
                APP_LOG_DEBUG("[%s] FAILED : buffer %u [%u, +%u) %u mismatches, first [%u] got 0x%08x expected 0x%08x",
                    getName(), buffer, first, count, mismatches, first + bad, got, pattern(buffer, first + bad));
                return 1;
            }
            return 0;
        }

        uint32_t verifyTexture()
        {
            Buffer data;
            TResult ret = mTex->unmap(mTexHandle, data);
            mTexHandle = ReadbackHandle::invalid();
            const size_t expected = size_t(kTexSize) * kTexSize * 4;
            if (T3D_FAILED(ret) || data.Data == nullptr || data.DataSize < expected)
            {
                APP_LOG_DEBUG("[%s] FAILED : texture unmap returned %d", getName(), (int)ret);
                data.release();
                return 1;
            }

            uint32_t bad = 0;
            for (uint32_t y = 0; y < kTexSize; ++y)
            {
                for (uint32_t x = 0; x < kTexSize; ++x)
                {
                    const uint8_t *p = data.Data + (y * kTexSize + x) * 4;
                    if (p[0] != uint8_t(x) || p[1] != uint8_t(y) || p[2] != 128 || p[3] != 255)
                    {
                        ++bad;
                    }
                }
            }
            data.release();

            if (bad != 0)
            {
                APP_LOG_DEBUG("[%s] FAILED : texture %u pixels mismatch", getName(), bad);
                return 1;
            }
            return 0;
        }

        ComputeKernelPtr    mKernel;
        StructuredBufferPtr mBuffers[kBufferCount];
        uint32_t            mCounts[kBufferCount] {};
        PixelBuffer2DPtr    mTex;
        ReadbackHandle      mHandles[kBufferCount] {};
        ReadbackHandle      mPartialHandle {};
        ReadbackHandle      mTexHandle {};
    };


    /**
     * \brief K35：超过入池上限的大读回（E.2.7 遗留 ②）
     * \remarks 单次超过 16 MB 的读回不进 staging 池，按实际大小建一次性 staging，unmap 后立即释放。
     *          同一帧内读回一块 18 MB 的结构化缓冲与一块 64 B 的小缓冲，逐元素校验。小缓冲走池，
     *          确认两条路径同时在途时互不干扰。池的淘汰（闲置超时、总量上限）对外不可见，
     *          由后端的 debug 日志确认。
     */
    class CaseLargeReadback : public ComputeCase
    {
    public:
        static const uint32_t kLargeCount = 18u * 1024u * 1024u / sizeof(uint32_t);
        static const uint32_t kSmallCount = 16;

        const char *getName() const override { return "K35 large readback"; }
        uint32_t getRequiredCaps() const override
        {
            return kNeedStructuredBuffer | kNeedReadback;
        }
        uint32_t getBackendMask() const override { return kBackendD3D11 | kBackendGL4; }

        TResult setup() override
        {
            mLarge = makeBuffer(kLargeCount, 0xA5000000u);
            mSmall = makeBuffer(kSmallCount, 0x5A000000u);
            return (mLarge != nullptr && mSmall != nullptr) ? T3D_OK : T3D_ERR_FAIL;
        }

        TResult record() override
        {
            mLargeHandle = mLarge->map(0, 0);
            mSmallHandle = mSmall->map(0, 0);
            return (mLargeHandle.isValid() && mSmallHandle.isValid()) ? T3D_OK : T3D_ERR_FAIL;
        }

        TResult verify() override
        {
            uint32_t failures = 0;
            failures += check(mLarge.get(), mLargeHandle, kLargeCount, 0xA5000000u, "large");
            failures += check(mSmall.get(), mSmallHandle, kSmallCount, 0x5A000000u, "small");
            return failures == 0 ? T3D_OK : T3D_ERR_FAIL;
        }

    private:
        static StructuredBufferPtr makeBuffer(uint32_t count, uint32_t tag)
        {
            Buffer init = allocBytes(sizeof(uint32_t) * size_t(count));
            uint32_t *p = reinterpret_cast<uint32_t *>(init.Data);
            for (uint32_t i = 0; i < count; ++i)
            {
                p[i] = tag ^ i;
            }
            StructuredBufferPtr sb = loadSB(StructuredBufferKind::kStructured, sizeof(uint32_t), count,
                kGPUShaderResource, kCPURead, init);
            // kVRAM 会接管 init.Data，这里只把指针摘掉以免 double-free
            init.Data = nullptr;
            init.DataSize = 0;
            return sb;
        }

        uint32_t check(RenderBuffer *buf, ReadbackHandle &handle, uint32_t count, uint32_t tag, const char *what)
        {
            Buffer data;
            TResult ret = unmapCheckSize(buf, handle, data, sizeof(uint32_t) * size_t(count));
            if (T3D_FAILED(ret))
            {
                APP_LOG_DEBUG("[%s] FAILED : %s unmap returned %d", getName(), what, (int)ret);
                return 1;
            }

            const uint32_t *out = reinterpret_cast<const uint32_t *>(data.Data);
            uint32_t mismatches = 0;
            uint32_t bad = 0, got = 0;
            for (uint32_t i = 0; i < count; ++i)
            {
                if (out[i] != (tag ^ i))
                {
                    if (mismatches == 0) { bad = i; got = out[i]; }
                    ++mismatches;
                }
            }
            data.release();

            if (mismatches != 0)
            {
                APP_LOG_DEBUG("[%s] FAILED : %s %u mismatches, first [%u] got 0x%08x expected 0x%08x",
                    getName(), what, mismatches, bad, got, tag ^ bad);
                return 1;
            }
            return 0;
        }

        StructuredBufferPtr mLarge;
        StructuredBufferPtr mSmall;
        ReadbackHandle      mLargeHandle {};
        ReadbackHandle      mSmallHandle {};
    };
}


void buildComputeCases(std::vector<ComputeCasePtr> &cases)
{
    cases.clear();
    cases.emplace_back(new CaseLinearWrite());
    cases.emplace_back(new CaseSaxpy());
    cases.emplace_back(new CaseRawTyped());
    cases.emplace_back(new CaseReduce());
    cases.emplace_back(new CaseTextureWrite());
    cases.emplace_back(new CaseReflect());
    cases.emplace_back(new CaseDispatchIndirect());
    cases.emplace_back(new CaseCopyCount());
    cases.emplace_back(new CaseVertexBufferSRV());
    cases.emplace_back(new CaseMsaaScaledBlit());
    cases.emplace_back(new CaseShaderBindOrder());
    cases.emplace_back(new CaseWindowViewport());
    cases.emplace_back(new CaseSamplerBorderColor());
    cases.emplace_back(new CaseLargeDispatch());
    cases.emplace_back(new CaseDynamicStructuredWrite());
    cases.emplace_back(new CaseDepthStencilFormats());
    cases.emplace_back(new CaseCrossStageTextures());
    cases.emplace_back(new CaseWindowExternalDepth());
    cases.emplace_back(new CaseMsaaTexture());
    cases.emplace_back(new CaseTextureWrite(true));
    cases.emplace_back(new CaseRenderTextureUAVTarget());
    cases.emplace_back(new CaseUAVSlotRange());
    cases.emplace_back(new CaseConstantBufferSlots());
    cases.emplace_back(new CaseTwoSamplersOneTexture());
    cases.emplace_back(new CaseRegisterOrder());
    cases.emplace_back(new CaseInlineSampler());
    cases.emplace_back(new CaseTextureArray());
    cases.emplace_back(new CaseDepthBiasClamp());
    cases.emplace_back(new CaseBufferKinds());
    cases.emplace_back(new CaseBufferCounters());
    cases.emplace_back(new CaseVertexSemantics());
    cases.emplace_back(new CaseCreateValidation());
    cases.emplace_back(new CaseReadbackStagingPool());
    cases.emplace_back(new CaseLargeReadback());
}

uint32_t currentBackendBit()
{
    RHIRendererPtr renderer = T3D_AGENT.getActiveRHIRenderer();
    if (renderer == nullptr)
    {
        return kBackendOther;
    }

    const String &name = renderer->getName();
    if (name == RHIRenderer::DIRECT3D11 || name == RHIRenderer::DIRECT3D11_CONSOLE)
    {
        return kBackendD3D11;
    }
    if (name == RHIRenderer::OPENGL4 || name == RHIRenderer::OPENGL4_CONSOLE)
    {
        return kBackendGL4;
    }
    if (name == RHIRenderer::OPENGLES3)
    {
        return kBackendGLES3;
    }

    return kBackendOther;
}

bool isCaseRunnable(const ComputeCase &c, const RHICapabilities &caps,
    uint32_t backendBit, String &reason)
{
    if ((c.getBackendMask() & backendBit) == 0)
    {
        reason = "backend not in case's backendMask";
        return false;
    }

    const uint32_t need = c.getRequiredCaps();

    struct CapCheck
    {
        uint32_t    bit;
        bool        supported;
        const char *name;
    };

    const CapCheck checks[] =
    {
        { kNeedCompute,          caps.supportsCompute,             "supportsCompute"             },
        { kNeedUnorderedAccess,  caps.supportsUnorderedAccess,     "supportsUnorderedAccess"     },
        { kNeedStructuredBuffer, caps.supportsStructuredBuffer,    "supportsStructuredBuffer"    },
        { kNeedReadback,         caps.supportsReadback,            "supportsReadback"            },
        { kNeedIndirectDraw,     caps.supportsIndirectDraw,        "supportsIndirectDraw"        },
        { kNeedIndirectDispatch, caps.supportsIndirectDispatch,    "supportsIndirectDispatch"    },
        { kNeedAppendConsume,    caps.supportsAppendConsumeBuffer, "supportsAppendConsumeBuffer" },
    };

    for (const CapCheck &check : checks)
    {
        if ((need & check.bit) != 0 && !check.supported)
        {
            reason = String("missing capability : ") + check.name;
            return false;
        }
    }

    return true;
}
