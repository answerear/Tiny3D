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


#include "RHI/T3DRHIVertexAttribute.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    RHIVertexAttribute::RHIVertexAttribute(size_t stream, size_t offset, Type type, Semantic semantic, size_t semanticIndex /* = 0 */)
        : mType(type)
        , mSemantic(semantic)
        , mOffset(offset)
        , mStream(stream)
        , mSemanticIndex(semanticIndex)
    {

    }

    //--------------------------------------------------------------------------

    RHIVertexAttribute::~RHIVertexAttribute()
    {

    }

    //--------------------------------------------------------------------------

    size_t RHIVertexAttribute::getSize() const
    {
        size_t s = 0;
        switch (mType)
        {
        case Type::E_VAT_FLOAT1:
            s = sizeof(float);
            break;
        case Type::E_VAT_FLOAT2:
            s = sizeof(float) * 2;
            break;
        case Type::E_VAT_FLOAT3:
            s = sizeof(float) * 3;
            break;
        case Type::E_VAT_FLOAT4:
            s = sizeof(float) * 4;
            break;
        case Type::E_VAT_COLOR:
            s = sizeof(uint32_t);
            break;
        case Type::E_VAT_BYTE4:
        case Type::E_VAT_BYTE4_NORM:
            s = sizeof(int8_t);
            break;
        case Type::E_VAT_UBYTE4:
        case Type::E_VAT_UBYTE4_NORM:
            s = sizeof(uint8_t) * 4;
            break;
        case Type::E_VAT_SHORT2:
        case Type::E_VAT_SHORT2_NORM:
            s = sizeof(int16_t) * 2;
            break;
        case Type::E_VAT_SHORT4:
        case Type::E_VAT_SHORT4_NORM:
            s = sizeof(int16_t) * 4;
            break;
        case Type::E_VAT_USHORT2:
        case Type::E_VAT_USHORT2_NORM:
            s = sizeof(uint16_t) * 2;
            break;
        case Type::E_VAT_USHORT4:
        case Type::E_VAT_USHORT4_NORM:
            s = sizeof(uint16_t) * 4;
            break;
        case Type::E_VAT_DOUBLE1:
            s = sizeof(double);
            break;
        case Type::E_VAT_DOUBLE2:
            s = sizeof(double) * 2;
            break;
        case Type::E_VAT_DOUBLE3:
            s = sizeof(double) * 3;
            break;
        case Type::E_VAT_DOUBLE4:
            s = sizeof(double) * 4;
            break;
        case Type::E_VAT_INT1:
            s = sizeof(int32_t);
            break;
        case Type::E_VAT_INT2:
            s = sizeof(int32_t) * 2;
            break;
        case Type::E_VAT_INT3:
            s = sizeof(int32_t) * 3;
            break;
        case Type::E_VAT_INT4:
            s = sizeof(int32_t) * 4;
            break;
        case Type::E_VAT_UINT1:
            s = sizeof(uint32_t);
            break;
        case Type::E_VAT_UINT2:
            s = sizeof(uint32_t) * 2;
            break;
        case Type::E_VAT_UINT3:
            s = sizeof(uint32_t) * 3;
            break;
        case Type::E_VAT_UINT4:
            s = sizeof(uint32_t) * 4;
            break;
        case Type::E_VAT_FLOAT16_2:
            s = sizeof(float);
            break;
        case Type::E_VAT_FLOAT16_4:
            s = sizeof(float) * 2;
            break;
        }

        return s;
    }
    
    //--------------------------------------------------------------------------
}

