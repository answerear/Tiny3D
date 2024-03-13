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

    bool RenderResourceManager::postLoad(RenderResourcePtr res)
    {
        bool ret = false;
        if (res != nullptr)
        {
            ret = res->onLoad();
        }
        return ret;
    }

    //--------------------------------------------------------------------------

    bool RenderResourceManager::preUnload(RenderResourcePtr res)
    {
        bool ret = false;
        if (res != nullptr)
        {
            ret = res->onUnload();
        }
        return ret;
    }
    
    //--------------------------------------------------------------------------

    template<typename CONTAINER_T>
    TResult RenderResourceManager::_GC(CONTAINER_T &container)
    {
        auto itr = container.begin();
            
        while (itr != container.end())
        {
            RenderResourcePtr res = itr->second;
            if (res->referCount() == 1)
            {
                // 没有外部引用，删掉
                if (preUnload(res))
                {
                    itr = container.erase(itr);
                }
                else
                {
                    ++itr;
                }
            }
            else
            {
                ++itr;
            }
        }
        
        return T3D_OK;
    }
    
    //--------------------------------------------------------------------------

    template <typename CONTAINER_T>
    void RenderResourceManager::clear(CONTAINER_T &container)
    {
        for (auto itr = container.begin(); itr != container.end(); ++itr)
        {
            RenderResourcePtr res = itr->second;
            preUnload(res);
        }

        container.clear();
    }
    
    //--------------------------------------------------------------------------

    RenderStateManagerPtr RenderStateManager::create()
    {
        return new RenderStateManager();
    }
    
    //--------------------------------------------------------------------------

    RenderStateManager::~RenderStateManager()
    {
        clear(mBlendStates);
        clear(mDepthStencilStates);
        clear(mRasterizerStates);
        clear(mSamplerStates);
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
                if (!postLoad(state))
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

    RenderBufferManager::~RenderBufferManager()
    {
        clear(mVBufferCache);
        clear(mIBufferCache);
        clear(mPBufferCache);
        clear(mCBufferCache);
        clear(mVertexDeclarations);
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

            if (!postLoad(decl))
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
                if (!postLoad(rb))
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
