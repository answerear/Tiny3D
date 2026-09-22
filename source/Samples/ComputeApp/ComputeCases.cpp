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
        uint32_t getBackendMask() const override { return kBackendD3D11; }

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
        uint32_t getBackendMask() const override { return kBackendD3D11; }

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
        uint32_t getBackendMask() const override { return kBackendD3D11; }

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
