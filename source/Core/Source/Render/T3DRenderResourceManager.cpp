/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2020  Answer Wong
 * For latest info, see https://github.com/answerear/Tiny3D
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/


#include "Render/T3DRenderResourceManager.h"
#include "Render/T3DBlendState.h"
#include "Render/T3DDepthStencilState.h"
#include "Render/T3DRasterizerState.h"
#include "Render/T3DSamplerState.h"
#include "Render/T3DVertexDeclaration.h"
#include "Render/T3DVertexBuffer.h"
#include "Render/T3DIndexBuffer.h"
#include "Render/T3DPixelBuffer.h"
#include "Render/T3DConstantBuffer.h"
#include "Render/T3DTextureDesc.h"
#include "RHI/T3DRHIResource.h"


namespace  Tiny3D
{
    //--------------------------------------------------------------------------

    bool RenderResourceManager::PostLoad(RenderResourcePtr res)
    {
        bool ret = false;
        if (res != nullptr)
        {
            ret = res->onLoad();
        }
        return ret;
    }

    //--------------------------------------------------------------------------

    bool RenderResourceManager::PreUnload(RenderResourcePtr res)
    {
        bool ret = false;
        if (res != nullptr)
        {
            ret = res->onUnload();
        }
        return ret;
    }

    //--------------------------------------------------------------------------

    RenderStateManagerPtr RenderStateManager::create()
    {
        return new RenderStateManager();
    }
    
    //--------------------------------------------------------------------------

    template<typename STATE_TYPE, typename DESC_TYPE>
    SmartPtr<STATE_TYPE> RenderStateManager::loadState(StateCache &states, const DESC_TYPE &desc, uint32_t hash /* = 0 */)
    {
        uint32_t code = hash;
        
        if (hash == 0)
        {
            code = CRC::crc32((uint8_t*)&desc, sizeof(desc));
        }

        SmartPtr<STATE_TYPE> state;
        auto itr = states.find(code);
        
        if (itr == states.end())
        {
            state = STATE_TYPE::create(desc, code);
            auto rval = states.emplace(code, state);
            
            if (rval.second)
            {
                if (!PostLoad(state))
                {
                    states.erase(code);
                    state = nullptr;
                }
            }
            else
            {
                T3D_ASSERT(rval.second, "insert state must succeed !");
            }
        }
        else
        {
            state = itr->second;
        }
        

        return state;
    }

    //--------------------------------------------------------------------------

    BlendStatePtr RenderStateManager::loadBlendState(const BlendDesc &desc, uint32_t hash)
    {
        return loadState<BlendState, BlendDesc>(mBlendStates, desc, hash);
    }
    
    //--------------------------------------------------------------------------

    DepthStencilStatePtr RenderStateManager::loadDepthStencilState(const DepthStencilDesc &desc, uint32_t hash)
    {
        return loadState<DepthStencilState, DepthStencilDesc>(mDepthStencilStates, desc, hash);
    }

    //--------------------------------------------------------------------------

    RasterizerStatePtr RenderStateManager::loadRasterizerState(const RasterizerDesc &desc, uint32_t hash)
    {
        return loadState<RasterizerState, RasterizerDesc>(mRasterizerStates, desc, hash);
    }

    //--------------------------------------------------------------------------
    
    SamplerStatePtr RenderStateManager::loadSamplerState(const SamplerDesc &desc, uint32_t hash)
    {
        return loadState<SamplerState, SamplerDesc>(mSamplerStates, desc, hash);
    }
    
    //--------------------------------------------------------------------------
    
    TResult RenderStateManager::GC()
    {
        TResult ret = T3D_OK;

        do
        {
            ret = _GC(mBlendStates);
            if (T3D_FAILED(ret))
            {
                break;
            }

            ret = _GC(mDepthStencilStates);
            if (T3D_FAILED(ret))
            {
                break;
            }
            
            ret = _GC(mRasterizerStates);
            if (T3D_FAILED(ret))
            {
                break;
            }
            
            ret = _GC(mSamplerStates);
            if (T3D_FAILED(ret))
            {
                break;
            }
        } while (false);

        return ret;
    }
    
    //--------------------------------------------------------------------------

    RenderBufferManagerPtr RenderBufferManager::create()
    {
        return new RenderBufferManager();
    }
    
    //--------------------------------------------------------------------------

    TResult RenderBufferManager::GC()
    {
        TResult ret = T3D_OK;

        do
        {
            ret = _GC(mVertexDeclarations);
            if (T3D_FAILED(ret))
            {
                break;
            }
            
            ret = _GC(mVBufferCache);
            if (T3D_FAILED(ret))
            {
                break;
            }

            ret = _GC(mIBufferCache);
            if (T3D_FAILED(ret))
            {
                break;
            }

            ret = _GC(mPBufferCache);
            if (T3D_FAILED(ret))
            {
                break;
            }

            ret = _GC(mCBufferCache);
            if (T3D_FAILED(ret))
            {
                break;
            }
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    VertexDeclarationPtr RenderBufferManager::addVertexDeclaration(VertexDeclarationPtr decl)
    {
        auto itr = mVertexDeclarations.find(decl->hash());
        if (itr != mVertexDeclarations.end())
        {
            // 已经存在，返回一个已有的
            decl = itr->second;
        }
        else
        {
            // 不存在，那就放到缓存中
            mVertexDeclarations.emplace(decl->hash(), decl);

            if (!PostLoad(decl))
            {
                mVertexDeclarations.erase(decl->hash());
                decl = nullptr;
            }
        }

        return decl;
    }

    //--------------------------------------------------------------------------

    template<typename BUFFER_TYPE, typename CREATOR, typename... ARGS>
    SmartPtr<BUFFER_TYPE> RenderBufferManager::loadBuffer(BufferCache &buffers, const UUID &uuid, CREATOR creator, ARGS... args)
    {
        SmartPtr<BUFFER_TYPE> rb;
        auto itr = buffers.find(uuid);
        if (itr != buffers.end())
        {
            // 已经存在，返回已有的
            rb = itr->second;
        }
        else
        {
            // 没有，新建一个
            rb = creator(args...);
            auto rval = buffers.emplace(uuid, rb);
            if (rval.second)
            {
                if (!PostLoad(rb))
                {
                    buffers.erase(uuid);
                    rb = nullptr;
                }
            }
            else
            {
                T3D_ASSERT(rval.second, "insert buffers must succeed !");
            }
        }

        return rb;
    }

    //--------------------------------------------------------------------------
    
    VertexBufferPtr RenderBufferManager::loadVertexBuffer(size_t vertexSize, size_t vertexCount,
        const Buffer &buffer, MemoryType memType, Usage usage, CPUAccessMode accMode, const UUID &uuid)
    {
        UUID uid = uuid;
        if (uuid == UUID::INVALID)
        {
            uid = UUID::generate();
        }

        return loadBuffer<VertexBuffer>(mVBufferCache, uid,
            [](const Buffer &buffer, size_t vertexSize, size_t vertexCount, MemoryType memType, Usage usage, CPUAccessMode accMode) -> RenderBufferPtr
            {
                return VertexBuffer::create(vertexSize, vertexCount, buffer, memType, usage, accMode);
            },
            buffer, vertexSize, vertexCount, memType, usage, accMode);
    }

    //--------------------------------------------------------------------------

    IndexBufferPtr RenderBufferManager::loadIndexBuffer(IndexType indexType, size_t indexCount,
        const Buffer &buffer, MemoryType memType, Usage usage, CPUAccessMode accMode, const UUID &uuid)
    {
        UUID uid = uuid;
        if (uuid == UUID::INVALID)
        {
            uid = UUID::generate();
        }

        return loadBuffer<IndexBuffer>(mIBufferCache, uid,
            [](IndexType indexType, size_t indexCount, const Buffer &buffer, MemoryType memType, Usage usage, uint32_t accMode)
            {
                return IndexBuffer::create(indexType, indexCount, buffer, memType, usage, accMode);
            },
            indexType, indexCount, buffer, memType, usage, accMode);
    }

    //--------------------------------------------------------------------------

    ConstantBufferPtr RenderBufferManager::loadConstantBuffer(const Buffer &buffer, MemoryType memType, Usage usage, CPUAccessMode accMode, const UUID &uuid)
    {
        UUID uid = uuid;
        if (uuid == UUID::INVALID)
        {
            uid = UUID::generate();
        }

        return loadBuffer<ConstantBuffer>(mCBufferCache, uid,
            [](const Buffer &buffer, MemoryType memType, Usage usage, CPUAccessMode accMode)
            {
                return ConstantBuffer::create(buffer, memType, usage, accMode);
            },
            buffer, memType, usage, accMode);
    }

    //--------------------------------------------------------------------------

    PixelBuffer1DPtr RenderBufferManager::loadPixelBuffer1D(PixelBuffer1DDesc *desc, MemoryType memType, Usage usage, CPUAccessMode accMode, const UUID &uuid)
    {
        UUID uid = uuid;
        if (uuid == UUID::INVALID)
        {
            uid = UUID::generate();
        }

        return loadBuffer<PixelBuffer1D>(mPBufferCache, uid,
            [](PixelBuffer1DDesc *desc, MemoryType memType, Usage usage, CPUAccessMode accMode)
            {
                return PixelBuffer1D::create(desc, memType, usage, accMode);
            },
            desc, memType, usage, accMode);
    }
    
    //--------------------------------------------------------------------------

    PixelBuffer2DPtr RenderBufferManager::loadPixelBuffer2D(PixelBuffer2DDesc *desc, MemoryType memType, Usage usage, CPUAccessMode accMode, const UUID &uuid)
    {
        UUID uid = uuid;
        if (uuid == UUID::INVALID)
        {
            uid = UUID::generate();
        }

        return loadBuffer<PixelBuffer2D>(mPBufferCache, uid,
            [](PixelBuffer2DDesc *desc, MemoryType memType, Usage usage, CPUAccessMode accMode)
            {
                return PixelBuffer2D::create(desc, memType, usage, accMode, false);
            },
            desc, memType, usage, accMode);
    }

    //--------------------------------------------------------------------------

    PixelBuffer3DPtr RenderBufferManager::loadPixelBuffer3D(PixelBuffer3DDesc *desc, MemoryType memType, Usage usage, CPUAccessMode accMode, const UUID &uuid)
    {
        UUID uid = uuid;
        if (uuid == UUID::INVALID)
        {
            uid = UUID::generate();
        }

        return loadBuffer<PixelBuffer3D>(mPBufferCache, uid,
            [](PixelBuffer3DDesc *desc, MemoryType memType, Usage usage, CPUAccessMode accMode)
            {
                return PixelBuffer3D::create(desc, memType, usage, accMode);
            },
            desc, memType, usage, accMode);
    }
    
    //--------------------------------------------------------------------------

    PixelBuffer2DPtr RenderBufferManager::loadRenderTexture(PixelBuffer2DDesc *desc, MemoryType memType, Usage usage, CPUAccessMode accMode, const UUID &uuid)
    {
        UUID uid = uuid;
        if (uuid == UUID::INVALID)
        {
            uid = UUID::generate();
        }

        return loadBuffer<PixelBuffer2D>(mPBufferCache, uid,
            [](PixelBuffer2DDesc *desc, MemoryType memType, Usage usage, CPUAccessMode accMode)
            {
                return PixelBuffer2D::create(desc, memType, usage, accMode, true);
            },
            desc, memType, usage, accMode);
    }

    //--------------------------------------------------------------------------

    RenderTargetManagerPtr RenderTargetManager::create()
    {
        return new RenderTargetManager();
    }

    //--------------------------------------------------------------------------
    
    TResult RenderTargetManager::GC()
    {
        return T3D_OK;
    }

    //--------------------------------------------------------------------------
}
