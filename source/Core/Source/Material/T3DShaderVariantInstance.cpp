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
#include "Render/T3DSamplerState.h"
#include "Resource/T3DTexture.h"
#include "Resource/T3DTextureManager.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    ShaderVariantInstancePtr ShaderVariantInstance::create(PassInstance *parent, ShaderVariantPtr shaderVariant)
    {
        return T3D_NEW ShaderVariantInstance(parent, shaderVariant);
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

        // 统计缓冲区数量和计算每个缓冲区的大小。缓冲区大小取所有变量末尾的最大值，
        // 不能简单累加各变量大小，否则遇到 HLSL 的 16 字节对齐填充会算小
        for (const auto &item : mShaderVariant->getShaderConstantParams())
        {
            uint32_t bp = item.second->getBindingPoint();
            uint32_t end = item.second->getDataOffset() + item.second->getDataSize();
            auto itr = cbuffers.find(bp);
            if (itr == cbuffers.end())
            {
                // 没有，插入一个
                CBufferInfo info(item.second->getCBufferName(), end);
                cbuffers.emplace(bp, info);
            }
            else if (end > itr->second.size)
            {
                itr->second.size = end;
            }
        }

        // 根据收集和计算的缓冲区信息，创建缓冲区数据和RHI缓冲区对象
        for (const auto &item : cbuffers)
        {
            // 创建缓冲区数据，用于读写
            Buffer buffer;
            // D3D11 要求常量缓冲区大小是 16 字节的整数倍，否则 CreateBuffer 直接失败
            buffer.DataSize = (item.second.size + 15) & ~size_t(15);
            buffer.Data = T3D_POD_NEW_ARRAY(uint8_t, buffer.DataSize);
            mConstBuffers.emplace_back(buffer);

            // 创建 RHI 相关缓冲区对象
            ConstantBufferPtr cbuffer = T3D_RENDER_BUFFER_MGR.loadConstantBuffer(item.second.name, item.first, buffer, MemoryType::kVRAM, Usage::kDynamic, CPUAccessMode::kCPUWrite);
            mConstantBuffers.emplace_back(cbuffer);

            // 创建 LUT，用于快速查找
            mConstBuffersLUT.emplace(item.second.name, buffer);
        }

        T3D_ASSERT(mConstantBuffers.size() == mConstBuffers.size());

        // 纹理槽位与采样器槽位相互独立：只用 Load 的纹理没有采样器，独立采样器没有同名纹理，
        // 两张表分开建，不能用一个槽位同时索引两者
        TMap<uint32_t, ShaderSamplerParam*> texParams;
        TMap<uint32_t, ShaderSamplerParam*> samplerParams;
        for (const auto &item : mShaderVariant->getShaderSamplerParams())
        {
            ShaderSamplerParam *param = item.second.get();
            if (param->hasTexture())
            {
                const auto result = texParams.emplace(param->getTexBinding(), param);
                if (!result.second)
                {
                    T3D_LOG_WARNING(LOG_TAG_RENDER, "Texture '%s' at slot %u conflicts with texture '%s', ignored !",
                        param->getName().c_str(), param->getTexBinding(), result.first->second->getName().c_str());
                }
            }
            if (param->hasSampler())
            {
                const auto result = samplerParams.emplace(param->getSamplerBinding(), param);
                if (!result.second)
                {
                    T3D_LOG_WARNING(LOG_TAG_RENDER, "Sampler '%s' at slot %u conflicts with sampler '%s', ignored !",
                        param->getName().c_str(), param->getSamplerBinding(), result.first->second->getName().c_str());
                }
            }
        }

        // 槽位不一定从 0 开始，数组从最小槽位排到最大槽位
        if (!samplerParams.empty())
        {
            mSamplerStartSlot = samplerParams.begin()->first;
            mSamplers.resize(samplerParams.rbegin()->first - mSamplerStartSlot + 1, nullptr);

            // 纹理自带的采样器在纹理设置之前先用默认采样器；独立采样器（没有同名纹理）按名字推断，
            // 推断不出时用默认采样器。不能留空：GL 的 sampler object 为 0 时用纹理对象自身的采样参数，默认要求完整的 mip 链
            SamplerStatePtr defaultSampler = T3D_RENDER_STATE_MGR.loadSamplerState(SamplerDesc());
            for (const auto &item : samplerParams)
            {
                const ShaderSamplerParam *param = item.second;
                SamplerStatePtr sampler = defaultSampler;

                const auto itrTex = param->hasTexture() ? texParams.find(param->getTexBinding()) : texParams.end();
                const bool ownedByTexture = (itrTex != texParams.end() && itrTex->second == param);
                if (!ownedByTexture)
                {
                    SamplerDesc desc;
                    if (SamplerState::inferDescFromName(param->getName(), desc))
                    {
                        sampler = T3D_RENDER_STATE_MGR.loadSamplerState(desc);
                    }
                    else
                    {
                        T3D_LOG_DEBUG(LOG_TAG_RENDER, "Sampler '%s' has no texture and no filter/address keyword in its name, use default (linear, wrap) !",
                            param->getName().c_str());
                    }
                }

                mSamplers[item.first - mSamplerStartSlot] = sampler;
            }
        }

        if (!texParams.empty())
        {
            mPixelBufferStartSlot = texParams.begin()->first;
            mPixelBuffers.resize(texParams.rbegin()->first - mPixelBufferStartSlot + 1, nullptr);
            mTextures.resize(texParams.size(), nullptr);

            uint32_t texCount = 0;
            for (const auto &item : texParams)
            {
                const ShaderSamplerParam *param = item.second;

                TexLUTItem itemLUT;
                itemLUT.pixelBufferIndex = item.first;
                itemLUT.textureIndex = texCount++;

                // 纹理自带的采样器（sampler<纹理名>）跟随纹理的 SamplerState
                if (param->hasSampler())
                {
                    const auto itr = samplerParams.find(param->getSamplerBinding());
                    if (itr != samplerParams.end() && itr->second == param)
                    {
                        itemLUT.samplerIndex = param->getSamplerBinding();
                    }
                }

                mTexturesLUT.emplace(param->getName(), itemLUT);
            }
        }
    }

    //--------------------------------------------------------------------------

    ShaderVariantInstancePtr ShaderVariantInstance::clone(PassInstance *parent) const
    {
        ShaderVariantInstancePtr newInstance = T3D_NEW ShaderVariantInstance();

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
                buffer.Data = T3D_POD_NEW_ARRAY(uint8_t, buffer.DataSize);
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
            
                // 设置纹理自带的采样器
                if (itr->second.samplerIndex != UINT32_MAX)
                {
                    index = itr->second.samplerIndex - mSamplerStartSlot;
                    mSamplers[index] = texture->getSamplerState();
                }
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


