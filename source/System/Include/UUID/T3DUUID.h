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

#ifndef __T3D_UUID_H__
#define __T3D_UUID_H__


#include "T3DSystemPrerequisites.h"
#include "T3DSystem.h"
#include "Object/T3DObject.h"
#include "Object/T3DSmartPtr.h"

namespace Tiny3D
{
    TSTRUCT()
    struct T3D_SYSTEM_API UUID
    {
        enum
        {
            kDefaultLength = 16
        };

        static const UUID INVALID;

        static UUID generate();

        UUID()
        {
            values.low = values.high = 0;
        }

        UUID(const UUID& other)
        {
            values.low = other.values.low;
            values.high = other.values.high;
        }

        UUID& operator =(const UUID& other)
        {
            values.low = other.values.low;
            values.high = other.values.high;
            return *this;
        }

        String toString() const;

        void fromString(const String& str);

        TPROPERTY(RTTRFuncName="value", RTTRFuncType="getter", "Description"="UUID value")
        String getValue() const
        {
            return toString();
        }

        TPROPERTY(RTTRFuncName="value", RTTRFuncType="setter", "Description"="UUID value")
        void setValue(String str)
        {
            fromString(str);
        }

        bool operator ==(const UUID& other) const
        {
            return (this->values.low == other.values.low 
                && this->values.high == other.values.high);
        }

        bool operator !=(const UUID& other) const
        {
            return !operator ==(other);
        }

        bool operator <(const UUID& other) const
        {
            return (this->values.high < other.values.high
                || this->values.high == other.values.high && this->values.low < other.values.low);
        }

        union
        {
            uint8_t bytes[kDefaultLength];

            struct Value
            {
                uint64_t    low;
                uint64_t    high;
            } values;
        };
    };

    struct T3D_SYSTEM_API UUIDHash
    {
        std::size_t operator()(const UUID& uuid) const
        {
            // 使用std::hash来计算每个字节的哈希值，然后将它们组合起来
            std::size_t hash = 0;
            for (int i = 0; i < 16; ++i)
            {
                hash ^= std::hash<char>()(uuid.bytes[i]) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
            }
            return hash;
        }
    };

    struct T3D_SYSTEM_API UUIDEqual
    {
        bool operator()(const UUID& lhs, const UUID& rhs) const
        {
            // 使用std::equal来比较两个UUID是否相等
            return lhs == rhs;
        }
    };
}


#endif  /*__T3D_UUID_H__*/
