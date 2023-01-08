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

#ifndef __T3D_UUID_H__
#define __T3D_UUID_H__


#include "T3DSystemPrerequisites.h"
#include "T3DSystem.h"
#include "T3DObject.h"
#include "T3DSmartPtr.h"

namespace Tiny3D
{
    typedef SmartPtr<class TestObject> TestObjectPtr;
    
    TCLASS()
    class T3D_SYSTEM_API TestObject : public Object
    {
        TRTTI_ENABLE(Object)
        TRTTI_FRIEND
        
    public:
        static TestObjectPtr create()
        {
            return new TestObject();
        }

        virtual ~TestObject() = default;

        TPROPERTY(RTTRFuncName="value", RTTRFuncType="getter")
        int getValue() const { return mValue; }

        TPROPERTY(RTTRFuncName="value", RTTRFuncType="setter")
        void setValue(int val) { mValue = val; }
        
    protected:
        TestObject() = default;

        int mValue = 10;
    };
    
    TCLASS()
    struct T3D_SYSTEM_API UUID
    {
        TRTTI_ENABLE()
        
    public:
        enum
        {
            kDefaultLength = 16
        };

        //static String generate(uint32_t len);
        //static String generate();

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
            TestObj = other.TestObj;
            ArrayData = other.ArrayData;
            ListData = other.ListData;
            ItemData = other.ItemData;
        }

        UUID& operator =(const UUID& other)
        {
            values.low = other.values.low;
            values.high = other.values.high;
            TestObj = other.TestObj;
            ArrayData = other.ArrayData;
            ListData = other.ListData;
            ItemData = other.ItemData;
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

        //const char *get_value() const
        //{
        //    static const size_t len = kDefaultLength * 4;
        //    static char temp[len+1];
        //    String str = toString();
        //    strncpy(temp, str.c_str(), str.length());
        //    temp[str.length()] = 0;
        //    return temp;
        //}

        //void set_value(const char *str)
        //{
        //    fromString(str);
        //}

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

        TPROPERTY(RTTRFuncName="TestObj", RTTRFuncType="getter")
        TestObject *getTestObj() const
        {
            return TestObj.get();
        }

        TPROPERTY(RTTRFuncName="TestObj", RTTRFuncType="setter")
        void setTestObj(TestObject *obj)
        {
            TestObj = obj;
        }

        TestObjectPtr TestObj;

        TPROPERTY()
        TArray<int> ArrayData;

        TPROPERTY()
        TList<float> ListData;

        TCLASS()
        struct Item
        {
            TPROPERTY()
            int key;

            TPROPERTY()
            int value;

            TRTTI_ENABLE()
        };

        TPROPERTY()
        TArray<Item> ItemData;

        TPROPERTY()
        TMap<int, int> MapData;
    };
}


#endif  /*__T3D_UUID_H__*/
