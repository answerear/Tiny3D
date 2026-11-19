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


    class CaseTextureWrite : public ComputeCase
    {
    public:
        static const uint32_t kSize = 256;

        const char *getName() const override { return "K5 texture UAV"; }
        uint32_t getRequiredCaps() const override
        {
            return kNeedCompute | kNeedUnorderedAccess | kNeedReadback;
        }
        uint32_t getBackendMask() const override { return kBackendD3D11 | kBackendGL4 | kBackendGLES3; }

        TResult setup() override
        {
            mKernel = ComputeKernel::create("ComputeTextureWrite", textureWriteSource(), 8, 8, 1);
            if (mKernel == nullptr) return T3D_ERR_FAIL;

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


    // ---- 图形回归用例（K11～K13）：验证 GL4 后端行为级修复，判定同样靠 readback ----

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
        PixelFormat format, uint32_t msaaCount, uint32_t cpuAccess)
    {
        return T3D_TEXTURE_MGR.createRenderTexture(name, width, height, format,
            1, msaaCount, 0, false, cpuAccess);
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
