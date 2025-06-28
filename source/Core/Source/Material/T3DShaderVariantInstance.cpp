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


#include "Material/T3DShaderVariantInstance.h"

#include "Material/T3DShaderSamplerParam.h"
#include "Material/T3DShaderVariant.h"
#include "Render/T3DConstantBuffer.h"
#include "Render/T3DPixelBuffer.h"
#include "Render/T3DRenderResourceManager.h"
#include "Resource/T3DTexture.h"
#include "Resource/T3DTextureManager.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    ShaderVariantInstancePtr ShaderVariantInstance::create(PassInstance *parent, ShaderVariantPtr shaderVariant)
    {
        return new ShaderVariantInstance(parent, shaderVariant);
    }

    //--------------------------------------------------------------------------

    ShaderVariantInstance::ShaderVariantInstance(PassInstance *parent, ShaderVariantPtr shaderVariant)
        : mPassInstance(parent)
        , mShaderVariant(shaderVariant)
    {
        struct CBufferInfo
        {
            CBufferInfo(const String &n, uint32_t s) : name(n), size(s) {}

            String      name {};
            uint32_t    size {0};
        };
        
        TMap<uint32_t, CBufferInfo> cbuffers;  // 常量缓冲区数量以及每个缓冲区的大小

        // 统计缓冲区数量和计算每个缓冲区的大小
        for (const auto &item : mShaderVariant->getShaderConstantParams())
        {
            uint32_t bp = item.second->getBindingPoint();
            auto itr = cbuffers.find(bp);
            if (itr == cbuffers.end())
            {
                // 没有，插入一个
                CBufferInfo info(item.second->getCBufferName(), item.second->getDataSize());
                cbuffers.emplace(bp, info);
            }
            else
            {
                itr->second.size += item.second->getDataSize();
            }
        }

        // 根据收集和计算的缓冲区信息，创建缓冲区数据和RHI缓冲区对象
        for (const auto &item : cbuffers)
        {
            // 创建缓冲区数据，用于读写
            Buffer buffer;
            buffer.DataSize = item.second.size;
            buffer.Data = new uint8_t[buffer.DataSize];
            mConstBuffers.emplace_back(buffer);

            // 创建 RHI 相关缓冲区对象
            ConstantBufferPtr cbuffer = T3D_RENDER_BUFFER_MGR.loadConstantBuffer(item.second.name, item.first, buffer, MemoryType::kVRAM, Usage::kDynamic, CPUAccessMode::kCPUWrite);
            mConstantBuffers.emplace_back(cbuffer);

            // 创建 LUT，用于快速查找
            mConstBuffersLUT.emplace(item.second.name, buffer);
        }

        T3D_ASSERT(mConstantBuffers.size() == mConstBuffers.size());

        // 以防万一 sampler start index 不是 0 开始，先找出最小的 index ，作为 start index
        uint32_t startIdx = std::numeric_limits<uint32_t>::max();
        uint32_t maxIdx = std::numeric_limits<uint32_t>::min();
        TMap<uint32_t, ShaderSamplerParamPtr> samplers;
        for (const auto &item : mShaderVariant->getShaderSamplerParams())
        {
            uint32_t index = item.second->getSamplerBinding();
            samplers.emplace(index, item.second);
            startIdx = std::min(startIdx, index);
            maxIdx = std::max(maxIdx, index);
        }

        // 记录 samplers 起始索引
        mSamplerStartSlot = startIdx;
        uint32_t samplerCount = maxIdx - startIdx + 1;

        // 建立 sampler LUT
        uint32_t texCount = 0;
        for (const auto &item : samplers)
        {
            TexLUTItem itemLUT;
            itemLUT.samplerIndex = item.first;
            itemLUT.textureIndex = texCount++;
            mTexturesLUT.emplace(item.second->getName(), itemLUT);
        }

        // 以防万一 texture start index 不是 0 开始，先找出最小的 index ，作为 start index
        startIdx = std::numeric_limits<uint32_t>::max();
        maxIdx = std::numeric_limits<uint32_t>::min();
        samplers.clear();
        for (const auto &item : mShaderVariant->getShaderSamplerParams())
        {
            uint32_t index = item.second->getTexBinding();
            samplers.emplace(index, item.second);
            startIdx = std::min(startIdx, index);
            maxIdx = std::max(maxIdx, index);
        }

        // 记录 pixel buffers 起始索引
        mPixelBufferStartSlot = startIdx;
        uint32_t pbCount = maxIdx - startIdx + 1;

        // 建立 pixel buffer LUT
        for (const auto &item : samplers)
        {
            const auto itr = mTexturesLUT.find(item.second->getName());
            T3D_ASSERT(itr != mTexturesLUT.end());
            itr->second.pixelBufferIndex = item.first;
        }

        // 预分配纹理、纹理采样器、像素缓冲对象大小
        if (texCount > 0)
        {
            mTextures.resize(texCount, nullptr);
            if (samplerCount > 0)
            {
                mSamplers.resize(samplerCount, nullptr);
            }
            if (pbCount > 0)
            {
                mPixelBuffers.resize(pbCount, nullptr);
            }
        }
    }

    //--------------------------------------------------------------------------

    ShaderVariantInstancePtr ShaderVariantInstance::clone(PassInstance *parent) const
    {
        ShaderVariantInstancePtr newInstance = new ShaderVariantInstance();

        if (newInstance != nullptr && T3D_FAILED(newInstance->cloneProperties(parent, this)))
        {
            newInstance = nullptr;
        }

        return newInstance;
    }

    //--------------------------------------------------------------------------

    TResult ShaderVariantInstance::cloneProperties(PassInstance *parent, const ShaderVariantInstance *const src)
    {
        TResult ret = T3D_OK;

        do
        {
            if (parent != nullptr)
            {
                mPassInstance = parent;
            }
            else
            {
                mPassInstance = src->getPassInstance();
            }

            mShaderVariant = src->getShaderVariant();

            // 复制常量缓冲区及其数据
            auto itrBuf = src->mConstBuffers.begin();
            auto itrCBuf = src->getConstantBuffers().begin();
            while (itrBuf != src->mConstBuffers.end() && itrCBuf != src->getConstantBuffers().end())
            {
                // 创建缓冲区数据，用于读写
                Buffer buffer;
                buffer.DataSize = itrBuf->DataSize;
                buffer.Data = new uint8_t[buffer.DataSize];
                memcpy(buffer.Data, itrBuf->Data, buffer.DataSize);
                mConstBuffers.emplace_back(buffer);

                // 创建 RHI 相关缓冲区对象
                ConstantBuffer *cb = *itrCBuf;
                ConstantBufferPtr cbuffer = T3D_RENDER_BUFFER_MGR.loadConstantBuffer(cb->getName(), cb->getBinding(), buffer, cb->getMemoryType(), cb->getUsage(), (CPUAccessMode)cb->getCPUAccessMode());
                mConstantBuffers.emplace_back(cbuffer);

                // 创建 LUT，用于快速查找
                mConstBuffersLUT.emplace(cb->getName(), buffer);
                
                ++itrBuf;
                ++itrCBuf;
            }

            // samplers 起始索引
            mSamplerStartSlot = src->getSamplerStartSlot();

            // pixel buffers 起始索引
            mPixelBufferStartSlot = src->getPixelBufferStartSlot();

            // 纹理
            for (const auto &texture : src->mTextures)
            {
                mTextures.emplace_back(texture);
            }

            // 纹理 LUT
            for (const auto &item : src->mTexturesLUT)
            {
                TexLUTItem itemLUT;
                itemLUT.samplerIndex = item.second.samplerIndex;
                itemLUT.pixelBufferIndex = item.second.pixelBufferIndex;
                itemLUT.textureIndex = item.second.textureIndex;
                mTexturesLUT.emplace(item.first, itemLUT);
            }

            // 纹理采样器
            for (const auto &sampler : src->getSamplers())
            {
                mSamplers.emplace_back(sampler);
            }

            // 像素缓冲区
            for (const auto &pb : src->getPixelBuffers())
            {
                mPixelBuffers.emplace_back(pb);
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ShaderVariantInstance::updateConstantBuffers(uint32_t &startSlot)
    {
        T3D_ASSERT(mConstantBuffers.size() == mConstBuffers.size());

        startSlot = std::numeric_limits<uint32_t>::max();
        
        for (size_t i = 0; i < mConstBuffers.size(); ++i)
        {
            const auto &buffer = mConstBuffers[i];
            const auto &cbuffer = mConstantBuffers[i];
            cbuffer->writeData(0, buffer, true);
            startSlot = std::min(startSlot, cbuffer->getBinding());
        }

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    bool ShaderVariantInstance::getConstantBufferInfo(const String &name, uint8_t *&data, uint32_t &dataSize) const
    {
        bool ret = false;

        const auto it = mShaderVariant->getShaderConstantParams().find(name);
        if (it != mShaderVariant->getShaderConstantParams().end())
        {
            const auto itr = mConstBuffersLUT.find(it->second->getCBufferName());
            if (itr != mConstBuffersLUT.end())
            {
                uint32_t offset = it->second->getDataOffset();
                dataSize = it->second->getDataSize();
                T3D_ASSERT(offset + dataSize <= itr->second.DataSize);
                data = itr->second.Data + offset;
                ret = true;
            }
        }
        
        return ret;
    }

    //--------------------------------------------------------------------------

    void ShaderVariantInstance::setTexture(const String &name, const UUID &uuid)
    {
        const auto itr = mTexturesLUT.find(name);
        if (itr != mTexturesLUT.end())
        {
            // 获取纹理
            TexturePtr texture = static_cast<Texture*>(T3D_TEXTURE_MGR.getResource(uuid));
            if (texture != nullptr)
            {
                // 设置纹理
                // uint32_t index = itr->second.pixelBufferIndex - mPixelBufferStartSlot;
                uint32_t index = itr->second.textureIndex;
                mTextures[index] = texture;

                // 设置像素缓冲区
                index = itr->second.pixelBufferIndex - mPixelBufferStartSlot;
                mPixelBuffers[index] = texture->getPixelBuffer();
            
                // 设置纹理采样器
                index = itr->second.samplerIndex - mSamplerStartSlot;
                mSamplers[index] = texture->getSamplerState();
            }
        }
    }

    //--------------------------------------------------------------------------

    Texture *ShaderVariantInstance::getTexture(const String &name) const
    {
        Texture *texture = nullptr;
        
        const auto itr = mTexturesLUT.find(name);
        if (itr != mTexturesLUT.end())
        {
            // uint32_t index = itr->second.pixelBufferIndex - mPixelBufferStartSlot;
            uint32_t index = itr->second.textureIndex;
            texture = mTextures[index]; 
        }

        return texture;
    }
    
    //--------------------------------------------------------------------------
}


