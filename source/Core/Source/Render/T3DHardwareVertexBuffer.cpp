/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Aaron Wong
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
    HardwareVertexBuffer::HardwareVertexBuffer(size_t vertexSize, size_t vertexCount, Usage usage, bool useSystemMemory, bool useShadowBuffer)
        : HardwareBuffer(usage, useSystemMemory, useShadowBuffer)
        , mVertexCount(vertexCount)
        , mVertexSize(vertexSize)
    {
        mBufferSize = mVertexSize * mVertexCount;
    }

    HardwareVertexBuffer::~HardwareVertexBuffer()
    {

    }

    ////////////////////////////////////////////////////////////////////////////

    VertexElement::VertexElement()
        : mType(E_VET_FLOAT1)
        , mSemantic(E_VES_POSITION)
        , mOffset(0)
        , mStream(0)
    {

    }

    VertexElement::VertexElement(size_t stream, size_t offset, Type type, Semantic semantic)
        : mType(type)
        , mSemantic(semantic)
        , mOffset(offset)
        , mStream(stream)
    {

    }

    VertexElement::~VertexElement()
    {

    }

    size_t VertexElement::getSize() const
    {
        size_t s = 0;
        switch (mType)
        {
        case E_VET_FLOAT1:
            s = sizeof(float);
            break;
        case E_VET_FLOAT2:
            s = sizeof(float) * 2;
            break;
        case E_VET_FLOAT3:
            s = sizeof(float) * 3;
            break;
        case E_VET_FLOAT4:
            s = sizeof(float) * 4;
            break;
        case E_VET_COLOR:
            s = sizeof(uint32_t);
            break;
        case E_VET_BYTE4:
        case E_VET_BYTE4_NORM:
            s = sizeof(int8_t);
            break;
        case E_VET_UBYTE4:
        case E_VET_UBYTE4_NORM:
            s = sizeof(uint8_t) * 4;
            break;
        case E_VET_SHORT2:
        case E_VET_SHORT2_NORM:
            s = sizeof(int16_t) * 2;
            break;
        case E_VET_SHORT4:
        case E_VET_SHORT4_NORM:
            s = sizeof(int16_t) * 4;
            break;
        case E_VET_USHORT2:
        case E_VET_USHORT2_NORM:
            s = sizeof(uint16_t) * 2;
            break;
        case E_VET_USHORT4:
        case E_VET_USHORT4_NORM:
            s = sizeof(uint16_t) * 4;
            break;
        case E_VET_DOUBLE1:
            s = sizeof(double);
            break;
        case E_VET_DOUBLE2:
            s = sizeof(double) * 2;
            break;
        case E_VET_DOUBLE3:
            s = sizeof(double) * 3;
            break;
        case E_VET_DOUBLE4:
            s = sizeof(double) * 4;
            break;
        case E_VET_INT1:
            s = sizeof(int32_t);
            break;
        case E_VET_INT2:
            s = sizeof(int32_t) * 2;
            break;
        case E_VET_INT3:
            s = sizeof(int32_t) * 3;
            break;
        case E_VET_INT4:
            s = sizeof(int32_t) * 4;
            break;
        case E_VET_UINT1:
            s = sizeof(uint32_t);
            break;
        case E_VET_UINT2:
            s = sizeof(uint32_t) * 2;
            break;
        case E_VET_UINT3:
            s = sizeof(uint32_t) * 3;
            break;
        case E_VET_UINT4:
            s = sizeof(uint32_t) * 4;
            break;
        case E_VET_FLOAT16_2:
            s = sizeof(float);
            break;
        case E_VET_FLOAT16_4:
            s = sizeof(float) * 2;
            break;
        }

        return s;
    }

    ////////////////////////////////////////////////////////////////////////////

    VertexDeclaration::VertexDeclaration()
    {

    }

    VertexDeclaration::~VertexDeclaration()
    {

    }

    const VertexElement &VertexDeclaration::getElement(size_t index) const
    {
        T3D_ASSERT(index < mVertexElements.size());

        VertexElementListConstItr itr = mVertexElements.begin();

        size_t i = 0;
        while (itr != mVertexElements.end())
        {
            if (i == index)
                break;
            ++itr;
            ++i;
        }

        return *itr;
    }

    const VertexElement &VertexDeclaration::addElement(size_t stream, size_t offset, VertexElement::Type type, VertexElement::Semantic semantic)
    {
        VertexElement element(stream, offset, type, semantic);

        mVertexElements.push_back(element);
        return mVertexElements.back();
    }

    const VertexElement &VertexDeclaration::insertElement(size_t pos, size_t stream, size_t offset, VertexElement::Type type, VertexElement::Semantic semantic)
    {
        if (pos >= mVertexElements.size())
        {
            return addElement(stream, offset, type, semantic);
        }

        auto itr = mVertexElements.begin();
        size_t i = 0;
        for (i = 0; i < pos; ++i)
        {
            ++itr;
        }

        itr = mVertexElements.insert(itr, VertexElement(stream, offset, type, semantic));
        return *itr;
    }

    bool VertexDeclaration::addElement(const VertexElement &vertexElement)
    {
        mVertexElements.push_back(vertexElement);
        return true;
    }

    bool VertexDeclaration::insertElement(size_t pos, const VertexElement &vertexElement)
    {
        if (pos >= mVertexElements.size())
        {
            return addElement(vertexElement);
        }

        auto itr = mVertexElements.begin();
        size_t i = 0;
        for (i = 0; i < pos; ++i)
        {
            ++itr;
        }

        itr = mVertexElements.insert(itr, vertexElement);

        return true;
    }

    void VertexDeclaration::removeElement(size_t pos)
    {
        if (pos >= mVertexElements.size())
            return;

        VertexElementListItr itr = mVertexElements.begin();
        size_t i = 0;
        for (i = 0; i < pos; ++i)
        {
            ++itr;
        }

        mVertexElements.erase(itr);
    }

    void VertexDeclaration::removeElement(VertexElement::Semantic semantic)
    {
        VertexElementListItr itr = mVertexElements.begin();
        while (itr != mVertexElements.end())
        {
            if (itr->getSemantic() == semantic)
            {
                mVertexElements.erase(itr);
                break;
            }
            ++itr;
        }
    }

    void VertexDeclaration::removeAllElements()
    {
        mVertexElements.clear();
    }

    void VertexDeclaration::updateElement(size_t pos, size_t stream, size_t offset, VertexElement::Type type, VertexElement::Semantic semantic)
    {
        if (pos >= mVertexElements.size())
            return;

        VertexElementListItr itr = mVertexElements.begin();
        size_t i = 0;
        for (i = 0; i < pos; ++i)
        {
            ++itr;
        }

        *itr = VertexElement(stream, offset, type, semantic);
    }

    const VertexElement *VertexDeclaration::findElementBySemantic(VertexElement::Semantic semantic) const
    {
        VertexElementListConstItr itr = mVertexElements.begin();
        while (itr != mVertexElements.end())
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
        VertexElementListConstItr itr = mVertexElements.begin();
        while (itr != mVertexElements.end())
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
        VertexDeclarationPtr decl = T3D_HARDWARE_BUFFER_MGR.createVertexDeclaration();
        VertexElementListConstItr itr = mVertexElements.begin();

        while (itr != mVertexElements.end())
        {
            decl->addElement(itr->getStream(), itr->getOffset(), itr->getType(), itr->getSemantic());
            ++itr;
        }

        return decl;
    }

//     VertexStreamPtr VertexStream::create(VertexDeclaration *decl, HardwareVertexBuffer *buffer)
//     {
//         VertexStream *stream = new VertexStream(decl, buffer);
//         VertexStreamPtr ptr(stream);
//         stream->release();
//         return ptr;
//     }
// 
//     VertexStream::VertexStream(VertexDeclaration *decl, HardwareVertexBuffer *buffer)
//         : mDeclaration(decl)
//         , mVertexBuffer(buffer)
//     {
// 
//     }
// 
//     VertexStream::~VertexStream()
//     {
//     }
// 
//     VertexStreamPtr VertexStream::clone(bool copyData) const
//     {
//         VertexStreamPtr ptr;
// 
//         if (copyData)
//         {
//             HardwareVertexBufferPtr vertexBuffer = T3D_HARDWARE_BUFFER_MGR.createVertexBuffer(
//                 mVertexBuffer->getVertexSize(), mVertexBuffer->getVertexCount(), mVertexBuffer->getUsage(), mVertexBuffer->hasShadowBuffer());
// 
//             vertexBuffer->copyData((HardwareBufferPtr)mVertexBuffer);
// 
//             VertexDeclaration *decl = new VertexDeclaration();
//             *decl = *mDeclaration;
//             VertexStream *stream = new VertexStream(decl, vertexBuffer);
//             decl->release();
//             ptr = stream;
//             stream->release();
//         }
//         else
//         {
//             VertexStream *stream = new VertexStream(mDeclaration, mVertexBuffer);
//             ptr = stream;
//             stream->release();
//         }
// 
//         return ptr;
//     }
}
