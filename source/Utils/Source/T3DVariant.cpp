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

#include "T3DVariant.h"


namespace Tiny3D
{
#if 0
    VariantArray    Variant::INVALID_ARRAY;
    VariantList     Variant::INVALID_LIST;
    VariantMap      Variant::INVALID_MAP;

    void Variant::copy(const Variant &other)
    {
        mType = other.mType;
        mValueSize = other.mValueSize;

        switch (mType)
        {
        case Type::E_BOOL:
        case Type::E_INT8:
        case Type::E_UINT8:
        case Type::E_INT16:
        case Type::E_UINT16:
        case Type::E_INT32:
        case Type::E_UINT32:
        case Type::E_INT64:
        case Type::E_UINT64:
        case Type::E_LONG:
        case Type::E_ULONG:
        case Type::E_FLOAT32:
        case Type::E_FLOAT64:
        case Type::E_FIX32:
        case Type::E_FIX64:
        case Type::E_CHAR:
        case Type::E_WCHAR:
            {
                memcpy(mValue, other.mValue, mValueSize);
            }
            break;
        case Type::E_STRING:
            {
                mStrValue = new char[mValueSize];
                memcpy(mStrValue, other.mStrValue, mValueSize);
            }
            break;
        case Type::E_ARRAY:
            {
                mArrayValue = new VariantArray(*other.mArrayValue);
            }
            break;
        case Type::E_LIST:
            {
                mListValue = new VariantList(*other.mListValue);
            }
            break;
        case Type::E_MAP:
            {
                mMapValue = new VariantMap(*other.mMapValue);
            }
            break;
        default:
            break;
        }
    }

    void Variant::releaseMemory()
    {
        switch (mType)
        {
        case Type::E_STRING:
            {
                delete[]mStrValue;
                mStrValue = nullptr;
            }
            break;
        case Type::E_ARRAY:
            {
                delete mArrayValue;
                mArrayValue = nullptr;
            }
            break;
        case Type::E_LIST:
            {
                delete mListValue;
                mListValue = nullptr;
            }
            break;
        case Type::E_MAP:
            {
                delete mMapValue;
                mMapValue = nullptr;
            }
            break;
        default:
            break;
        }

        memset(mValue, 0, sizeof(mValue));
    }

    int32_t Variant::compare(const Variant &other) const
    {
        int32_t ret = 0;
        if (mType == other.mType)
        {
            if (mType == Type::E_STRING)
            {
                size_t size = std::max(mValueSize, other.mValueSize);
                ret = strncmp(mStrValue, other.mStrValue, size);
            }
            else if (mType == Type::E_ARRAY)
            {
                if (*mArrayValue < *other.mArrayValue)
                    ret = -1;
                else if (*mArrayValue > *other.mArrayValue)
                    ret = 1;
                else
                    ret = 0;
            }
            else if (mType == Type::E_LIST)
            {
                if (*mListValue < *other.mListValue)
                    ret = -1;
                else if (*mListValue > *other.mListValue)
                    ret = 1;
                else
                    ret = 0;
            }
            else if (mType == Type::E_MAP)
            {
                if (*mMapValue < *other.mMapValue)
                    ret = -1;
                else if (*mMapValue > *other.mMapValue)
                    ret = 1;
                else
                    ret = 0;
            }
            else
            {
                ret = memcmp(mValue, other.mValue, mValueSize);
            }
        }
        else
        {
            ret = (static_cast<int32_t>(mType) 
                - static_cast<int32_t>(other.mType));
        }

        return ret;
    }
#endif
}
