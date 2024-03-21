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


#include "Render/T3DVertexAttribute.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    VertexAttribute::VertexAttribute(uint32_t slot, uint32_t offset, Type type, Semantic semantic, uint32_t semanticIndex /* = 0 */)
        : mType(type)
        , mSemantic(semantic)
        , mOffset(offset)
        , mSlot(slot)
        , mSemanticIndex(semanticIndex)
    {

    }

    //--------------------------------------------------------------------------

    uint32_t VertexAttribute::getSize() const
    {
        uint32_t s = 0;
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

