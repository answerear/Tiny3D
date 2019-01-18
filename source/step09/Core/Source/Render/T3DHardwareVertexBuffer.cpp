/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2019  Answer Wong
 * For latest info, see https://github.com/asnwerear/Tiny3D
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


#include "Render/T3DHardwareVertexBuffer.h"
#include "Render/T3DHardwareBufferManager.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    HardwareVertexBuffer::HardwareVertexBuffer(size_t vertexSize, 
        size_t vertexCount, Usage usage, bool useSystemMemory, 
        bool useShadowBuffer)
        : HardwareBuffer(usage, useSystemMemory, useShadowBuffer)
        , mVertexCount(vertexCount)
        , mVertexSize(vertexSize)
    {
        mBufferSize = mVertexSize * mVertexCount;
    }

    HardwareVertexBuffer::~HardwareVertexBuffer()
    {

    }

    //--------------------------------------------------------------------------

    VertexAttribute::VertexAttribute()
    {

    }

    VertexAttribute::VertexAttribute(size_t stream, size_t offset, Type type, 
        Semantic semantic)
        : mType(type)
        , mSemantic(semantic)
        , mOffset(offset)
        , mStream(stream)
    {

    }

    VertexAttribute::~VertexAttribute()
    {

    }

    size_t VertexAttribute::getSize() const
    {
        size_t s = 0;
        switch (mType)
        {
        case E_VAT_FLOAT1:
            s = sizeof(float);
            break;
        case E_VAT_FLOAT2:
            s = sizeof(float) * 2;
            break;
        case E_VAT_FLOAT3:
            s = sizeof(float) * 3;
            break;
        case E_VAT_FLOAT4:
            s = sizeof(float) * 4;
            break;
        case E_VAT_COLOR:
            s = sizeof(uint32_t);
            break;
        case E_VAT_BYTE4:
        case E_VAT_BYTE4_NORM:
            s = sizeof(int8_t);
            break;
        case E_VAT_UBYTE4:
        case E_VAT_UBYTE4_NORM:
            s = sizeof(uint8_t) * 4;
            break;
        case E_VAT_SHORT2:
        case E_VAT_SHORT2_NORM:
            s = sizeof(int16_t) * 2;
            break;
        case E_VAT_SHORT4:
        case E_VAT_SHORT4_NORM:
            s = sizeof(int16_t) * 4;
            break;
        case E_VAT_USHORT2:
        case E_VAT_USHORT2_NORM:
            s = sizeof(uint16_t) * 2;
            break;
        case E_VAT_USHORT4:
        case E_VAT_USHORT4_NORM:
            s = sizeof(uint16_t) * 4;
            break;
        case E_VAT_DOUBLE1:
            s = sizeof(double);
            break;
        case E_VAT_DOUBLE2:
            s = sizeof(double) * 2;
            break;
        case E_VAT_DOUBLE3:
            s = sizeof(double) * 3;
            break;
        case E_VAT_DOUBLE4:
            s = sizeof(double) * 4;
            break;
        case E_VAT_INT1:
            s = sizeof(int32_t);
            break;
        case E_VAT_INT2:
            s = sizeof(int32_t) * 2;
            break;
        case E_VAT_INT3:
            s = sizeof(int32_t) * 3;
            break;
        case E_VAT_INT4:
            s = sizeof(int32_t) * 4;
            break;
        case E_VAT_UINT1:
            s = sizeof(uint32_t);
            break;
        case E_VAT_UINT2:
            s = sizeof(uint32_t) * 2;
            break;
        case E_VAT_UINT3:
            s = sizeof(uint32_t) * 3;
            break;
        case E_VAT_UINT4:
            s = sizeof(uint32_t) * 4;
            break;
        case E_VAT_FLOAT16_2:
            s = sizeof(float);
            break;
        case E_VAT_FLOAT16_4:
            s = sizeof(float) * 2;
            break;
        }

        return s;
    }

    //--------------------------------------------------------------------------

    VertexDeclaration::VertexDeclaration()
    {

    }

    VertexDeclaration::~VertexDeclaration()
    {

    }

    const VertexAttribute &VertexDeclaration::getAttribute(size_t index) const
    {
        T3D_ASSERT(index < mVertexAttributes.size());

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

    const VertexAttribute &VertexDeclaration::addAttribute(size_t stream, 
        size_t offset, VertexAttribute::Type type, 
        VertexAttribute::Semantic semantic)
    {
        VertexAttribute element(stream, offset, type, semantic);

        mVertexAttributes.push_back(element);
        return mVertexAttributes.back();
    }

    const VertexAttribute &VertexDeclaration::insertAttribute(size_t pos, 
        size_t stream, size_t offset, VertexAttribute::Type type, 
        VertexAttribute::Semantic semantic)
    {
        if (pos >= mVertexAttributes.size())
        {
            return addAttribute(stream, offset, type, semantic);
        }

        auto itr = mVertexAttributes.begin();
        size_t i = 0;
        for (i = 0; i < pos; ++i)
        {
            ++itr;
        }

        itr = mVertexAttributes.insert(itr, 
            VertexAttribute(stream, offset, type, semantic));
        return *itr;
    }

    TResult VertexDeclaration::addAttribute(
        const VertexAttribute &vertexAttribute)
    {
        mVertexAttributes.push_back(vertexAttribute);
        return T3D_ERR_OK;
    }

    TResult VertexDeclaration::insertAttribute(size_t pos, 
        const VertexAttribute &vertexAttribute)
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

        return T3D_ERR_OK;
    }

    TResult VertexDeclaration::removeAttribute(size_t pos)
    {
        if (pos >= mVertexAttributes.size())
        {
            T3D_LOG_ERROR("Remove attribute but pos is out of bound !!!");
            return T3D_ERR_OUT_OF_BOUND;
        }

        VertexAttriListItr itr = mVertexAttributes.begin();
        size_t i = 0;
        for (i = 0; i < pos; ++i)
        {
            ++itr;
        }

        mVertexAttributes.erase(itr);
        return T3D_ERR_OK;
    }

    TResult VertexDeclaration::removeAttribute(
        VertexAttribute::Semantic semantic)
    {
        TResult ret = T3D_ERR_NOT_FOUND;
        VertexAttriListItr itr = mVertexAttributes.begin();
        while (itr != mVertexAttributes.end())
        {
            if (itr->getSemantic() == semantic)
            {
                mVertexAttributes.erase(itr);
                ret = T3D_ERR_OK;
                break;
            }
            ++itr;
        }

        return ret;
    }

    TResult VertexDeclaration::removeAllAttributes()
    {
        mVertexAttributes.clear();
        return T3D_ERR_OK;
    }

    TResult VertexDeclaration::updateAttribute(size_t pos, size_t stream, 
        size_t offset, VertexAttribute::Type type, 
        VertexAttribute::Semantic semantic)
    {
        if (pos >= mVertexAttributes.size())
        {
            T3D_LOG_ERROR("Update attribute but pos is out of bound !!!");
            return T3D_ERR_OUT_OF_BOUND;
        }

        VertexAttriListItr itr = mVertexAttributes.begin();
        size_t i = 0;
        for (i = 0; i < pos; ++i)
        {
            ++itr;
        }

        *itr = VertexAttribute(stream, offset, type, semantic);

        return T3D_ERR_OK;
    }

    const VertexAttribute *VertexDeclaration::findAttributeBySemantic(
        VertexAttribute::Semantic semantic) const
    {
        VertexAttriListConstItr itr = mVertexAttributes.begin();
        while (itr != mVertexAttributes.end())
        {
            if (itr->getSemantic() == semantic)
            {
                return &(*itr);
                break;
            }
            ++itr;
        }

        return nullptr;
    }

    size_t VertexDeclaration::getVertexSize(size_t source) const
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

    VertexDeclarationPtr VertexDeclaration::clone() const
    {
        VertexDeclarationPtr decl 
            = T3D_HARDWARE_BUFFER_MGR.createVertexDeclaration();
        VertexAttriListConstItr itr = mVertexAttributes.begin();

        while (itr != mVertexAttributes.end())
        {
            decl->addAttribute(itr->getStream(), itr->getOffset(), 
                itr->getType(), itr->getSemantic());
            ++itr;
        }

        return decl;
    }
}

