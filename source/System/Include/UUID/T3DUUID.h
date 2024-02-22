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
    // typedef SmartPtr<class TestObject> TestObjectPtr;
    //
    // TCLASS()
    // class T3D_SYSTEM_API TestObject : public Object
    // {
    //     TRTTI_ENABLE(Object)
    //     TRTTI_FRIEND
    //     
    // public:
    //     static TestObjectPtr create()
    //     {
    //         return new TestObject();
    //     }
    //
    //     virtual ~TestObject() = default;
    //
    //     TPROPERTY(RTTRFuncName="value", RTTRFuncType="getter")
    //     int getValue() const { return mValue; }
    //
    //     TPROPERTY(RTTRFuncName="value", RTTRFuncType="setter")
    //     void setValue(int val) { mValue = val; }
    //
    //     TFUNCTION()
    //     void aaTest(int aaiVal = 10, const String &aaName = "aaron")
    //     {
    //         
    //     }
    //     
    // protected:
    //     TestObject() = default;
    //
    //     int mValue = 10;
    // };
    //
    // /**
    //  * @brief 有向包围盒类（Oriented Bounding Box）
    //  */
    // TCLASS()
    // template <typename T>
    // class TTestObb
    // {
    //     TRTTI_ENABLE()
    //     
    // public:
    //     TENUM()
    //     enum BuildOption
    //     {
    //         E_BUILD_AABB = 0,       /**< 通过AABB直接构造 */
    //         E_BUILD_COVARIANCE = 1, /**< 利用协方差矩阵构造 */
    //     };
    //     
    //     /// 默认构造函数
    //     TTestObb() = default;
    //     
    //     TFUNCTION()
    //     void build(BuildOption option = E_BUILD_AABB)
    //     {
    //         BO = option;
    //     }
    //
    //     TPROPERTY()
    //     BuildOption BO;
    // };
    
    TSTRUCT()
    struct T3D_SYSTEM_API UUID
    {
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
            // TestObj = other.TestObj;
            // ArrayData = other.ArrayData;
            // ListData = other.ListData;
            // ItemData = other.ItemData;
            // MapData = other.MapData;
            // MapItemData = other.MapItemData;
            // TestObb =  other.TestObb;
        }

        UUID& operator =(const UUID& other)
        {
            values.low = other.values.low;
            values.high = other.values.high;
            // TestObj = other.TestObj;
            // ArrayData = other.ArrayData;
            // ListData = other.ListData;
            // ItemData = other.ItemData;
            // MapData = other.MapData;
            // MapItemData = other.MapItemData;
            // TestObb =  other.TestObb;
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

        // TPROPERTY(RTTRFuncName="TestObj", RTTRFuncType="getter")
        // TestObject *getTestObj() const
        // {
        //     return TestObj.get();
        // }
        //
        // TPROPERTY(RTTRFuncName="TestObj", RTTRFuncType="setter")
        // void setTestObj(TestObject *obj)
        // {
        //     TestObj = obj;
        // }
        //
        // TestObjectPtr TestObj;
        //
        // TPROPERTY()
        // TArray<int> ArrayData;
        //
        // TPROPERTY()
        // TList<float> ListData;
        //
        // TCLASS()
        // struct Item
        // {
        //     TPROPERTY()
        //     int key;
        //
        //     TPROPERTY()
        //     int value;
        //
        //     TRTTI_ENABLE()
        // };
        //
        // TPROPERTY()
        // TArray<Item> ItemData;
        //
        // TPROPERTY()
        // TMap<int, int> MapData;
        //
        // TCLASS()
        // struct Data
        // {
        //     TPROPERTY()
        //     int ival;
        //     
        //     TPROPERTY()
        //     float fval;
        //
        //     TRTTI_ENABLE()
        // };
        //
        // TPROPERTY()
        // TMap<String, Data> MapItemData;

        // TPROPERTY()
        // TTestObb<float> TestObb;
    };
}


#endif  /*__T3D_UUID_H__*/
