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


#include "Render/T3DVertexDeclaration.h"
#include "Kernel/T3DAgent.h"
#include "RHI/T3DRHIContext.h"
#include "RHI/T3DRHIVertexDeclaration.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    VertexDeclarationPtr VertexDeclaration::create()
    {
        return new VertexDeclaration();
    }

    //--------------------------------------------------------------------------

    VertexDeclaration::VertexDeclaration()
    {

    }

    //--------------------------------------------------------------------------

    VertexDeclaration::~VertexDeclaration()
    {

    }

    //--------------------------------------------------------------------------

    RenderResource::Type VertexDeclaration::getType() const 
    { 
        return Type::kVertexDeclaration;
    }

    //--------------------------------------------------------------------------

    const VertexAttribute &VertexDeclaration::getAttribute(size_t index) const
    {
        T3D_ASSERT(index < mVertexAttributes.size(), "VertexDeclaration the index of attribute is out of bound !");

        VertexAttriListConstItr itr = mVertexAttributes.begin();

        size_t i = 0;
        while (itr != mVertexAttributes.end())
        {
            if (i == index)
                break;
            ++itr;
            ++i;
        }

        return *itr;
    }

    //--------------------------------------------------------------------------

    const VertexAttribute &VertexDeclaration::addAttribute(uint32_t stream, uint32_t offset, VertexAttribute::Type type, VertexAttribute::Semantic semantic, uint32_t semanticIndex)
    {
        mIsDirty = true;
        return mVertexAttributes.emplace_back(stream, offset, type, semantic, semanticIndex);
    }

    //--------------------------------------------------------------------------

    const VertexAttribute &VertexDeclaration::insertAttribute(uint32_t pos, uint32_t stream, uint32_t offset, VertexAttribute::Type type, VertexAttribute::Semantic semantic, uint32_t semanticIndex)
    {
        if (pos >= mVertexAttributes.size())
        {
            return addAttribute(stream, offset, type, semantic, semanticIndex);
        }

        auto itr = mVertexAttributes.begin();
        size_t i = 0;
        for (i = 0; i < pos; ++i)
        {
            ++itr;
        }

        itr = mVertexAttributes.emplace(itr, stream, offset, type, semantic, semanticIndex);
        mIsDirty = true;
        
        return *itr;
    }

    //--------------------------------------------------------------------------

    TResult VertexDeclaration::addAttribute(const VertexAttribute &vertexAttribute)
    {
        mVertexAttributes.emplace_back(vertexAttribute);
        mIsDirty = true;
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VertexDeclaration::insertAttribute(uint32_t pos, const VertexAttribute &vertexAttribute)
    {
        if (pos >= mVertexAttributes.size())
        {
            return addAttribute(vertexAttribute);
        }

        auto itr = mVertexAttributes.begin();
        size_t i = 0;
        for (i = 0; i < pos; ++i)
        {
            ++itr;
        }

        itr = mVertexAttributes.insert(itr, vertexAttribute);
        mIsDirty = true;
        
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VertexDeclaration::removeAttribute(uint32_t pos)
    {
        if (pos >= mVertexAttributes.size())
        {
            T3D_LOG_ERROR(LOG_TAG_RENDER,
                "Remove attribute but pos is out of bound !!!");
            return T3D_ERR_OUT_OF_BOUND;
        }

        VertexAttriListItr itr = mVertexAttributes.begin();
        size_t i = 0;
        for (i = 0; i < pos; ++i)
        {
            ++itr;
        }

        mVertexAttributes.erase(itr);
        mIsDirty = true;
        
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VertexDeclaration::removeAttribute(VertexAttribute::Semantic semantic, uint32_t sematicIndex)
    {
        TResult ret = T3D_ERR_NOT_FOUND;
        VertexAttriListItr itr = mVertexAttributes.begin();
        while (itr != mVertexAttributes.end())
        {
            if (itr->getSemantic() == semantic 
                && itr->getSemanticIndex() == sematicIndex)
            {
                mVertexAttributes.erase(itr);
                mIsDirty = true;
                ret = T3D_OK;
                break;
            }
            ++itr;
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult VertexDeclaration::removeAllAttributes()
    {
        mVertexAttributes.clear();
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult VertexDeclaration::updateAttribute(uint32_t pos, uint32_t stream, uint32_t offset, VertexAttribute::Type type, VertexAttribute::Semantic semantic, uint32_t semanticIndex)
    {
        if (pos >= mVertexAttributes.size())
        {
            T3D_LOG_ERROR(LOG_TAG_RENDER, "Update attribute but pos is out of bound !!!");
            return T3D_ERR_OUT_OF_BOUND;
        }

        VertexAttriListItr itr = mVertexAttributes.begin();
        size_t i = 0;
        for (i = 0; i < pos; ++i)
        {
            ++itr;
        }

        *itr = VertexAttribute(stream, offset, type, semantic, semanticIndex);
        mIsDirty = true;
        
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    const VertexAttribute *VertexDeclaration::findAttributeBySemantic(VertexAttribute::Semantic semantic, uint32_t semanticIndex) const
    {
        VertexAttriListConstItr itr = mVertexAttributes.begin();
        while (itr != mVertexAttributes.end())
        {
            if (itr->getSemantic() == semantic 
                && itr->getSemanticIndex() == semanticIndex)
            {
                return &(*itr);
                break;
            }
            ++itr;
        }

        return nullptr;
    }

    //--------------------------------------------------------------------------

    uint32_t VertexDeclaration::getVertexSize(uint32_t source) const
    {
        size_t s = 0;
        VertexAttriListConstItr itr = mVertexAttributes.begin();
        while (itr != mVertexAttributes.end())
        {
            if (source == itr->getStream())
            {
                s += itr->getSize();
            }

            ++itr;
        }

        return s;
    }

    //--------------------------------------------------------------------------

    uint32_t VertexDeclaration::hash()
    {
        if (mIsDirty)
        {
            // 数据更新了，重新计算 hash 值
            mHash = CRC::crc32((uint8_t*)mVertexAttributes.data(), (uint32_t)mVertexAttributes.size() * sizeof(VertexAttribute));
            mIsDirty = false;
        }
        
        return mHash;
    }

    //--------------------------------------------------------------------------
    
    bool VertexDeclaration::onLoad() 
    {
        mRHIResource = T3D_AGENT.getActiveRHIContext()->createVertexDeclaration(this);
        return true;
    }

    //--------------------------------------------------------------------------
    
    bool VertexDeclaration::onUnload() 
    {
        mRHIResource = nullptr;
        return true;
    }

    //--------------------------------------------------------------------------
}

