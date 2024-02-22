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

#ifndef __T3D_VARIANT_H__
#define __T3D_VARIANT_H__


#include "T3DUtilsPrerequisites.h"
#include "../../../dependencies/sdl2/include/SDL2/SDL_syswm.h"
// #include "T3DFix32.h"
// #include "T3DFix64.h"


namespace Tiny3D
{
    enum VariantType
    {
        kBool = 0,
        kInt8,
        kUInt8,
        kInt16,
        kUInt16,
        kInt32,
        kUInt32,
        kInt64, 
        kUInt64,
        kLong,
        kSingle,
        kDouble,
        kChar,
        kWChar,
        kString,
        kSequenceContainer,
        kAssociativeContainer,
        kMax
    };

    struct SequenceContainer;
    struct AssociativeContainer;
    
    typedef TVariant
    < bool
    , int8_t
    , uint8_t
    , int16_t
    , uint16_t
    , int32_t
    , uint32_t
    , int64_t
    , uint64_t
    , long_t
    , float32_t
    , float64_t
    , char
    , wchar_t
    , String
    , SequenceContainer
    , AssociativeContainer
    > Variant;

    struct SequenceContainer
    {
        std::list<Variant> values;
    };

    struct AssociativeContainer
    {
        std::map<Variant, Variant> values;
    };
    
#if 0
    class T3D_UTILS_API Variant
    {
    public:
        enum class Type : size_t
        {
            E_NONE = 0,
            E_BOOL,         /**< bool */
            E_INT8,         /**< int8_t */
            E_UINT8,        /**< uint8_t */
            E_INT16,        /**< int16_t */
            E_UINT16,       /**< uint16_t */
            E_INT32,        /**< int32_t */
            E_UINT32,       /**< uint32_t */
            E_INT64,        /**< in64_t */
            E_UINT64,       /**< uint64_t */
            E_LONG,         /**< long_t */
            E_ULONG,        /**< ulong_t */
            E_FLOAT32,      /**< double */
            E_FLOAT64,      /**< float */
            E_FIX32,        /**< fix32_t */
            E_FIX64,        /**< fix64_t */
            E_CHAR,         /**< char */
            E_WCHAR,        /**< wchar_t */
            E_STRING,       /**< string */
            E_ARRAY,        /**< array */
            E_LIST,         /**< list */
            E_MAP,          /**< map */
        };

        /** 构造函数 */
        Variant();
        Variant(bool value);
        Variant(int8_t value);
        Variant(uint8_t value);
        Variant(int16_t value);
        Variant(uint16_t value);
        Variant(int32_t value);
        Variant(uint32_t value);
        Variant(int64_t value);
        Variant(uint64_t value);
        Variant(float32_t value);
        Variant(float64_t value);
        // Variant(const fix32_t &value);
        // Variant(const fix64_t &value);
        Variant(char value);
        Variant(wchar_t value);
        Variant(const char *value);
        Variant(const String &value);
        Variant(const VariantArray &value);
        Variant(const VariantList &value);
        Variant(const VariantMap &value);

        /** 拷贝构造函数*/
        Variant(const Variant &other);

        /** 重载赋值操作符 */
        Variant &operator =(const Variant &other);

        /** 析构函数 */
        ~Variant();

        /** 设置数值 */
        void setBool(bool value);
        void setInt8(int8_t value);
        void setUInt8(uint8_t value);
        void setInt16(int16_t value);
        void setUInt16(uint16_t value);
        void setInt32(int32_t value);
        void setUInt32(uint32_t value);
        void setInt64(int64_t value);
        void setUInt64(uint64_t value);
        void setLong(long_t value);
        void setULong(ulong_t value);
        void setFloat32(float32_t value);
        void setFloat64(float64_t value);
        // void setFix32(const fix32_t &value);
        // void setFix64(const fix64_t &value);
        void setChar(char value);
        void setWChar(wchar_t value);
        void setString(const String &value);
        void setArray(const VariantArray &value);
        void setList(const VariantList &value);
        void setMap(const VariantMap &value);

        /** 获取数值 */
        bool boolValue(bool *ok = nullptr) const;
        int8_t int8Value(bool *ok = nullptr) const;
        uint8_t uint8Value(bool *ok = nullptr) const;
        int16_t int16Value(bool *ok = nullptr) const;
        uint16_t uint16Value(bool *ok = nullptr) const;
        int32_t int32Value(bool *ok = nullptr) const;
        uint32_t uint32Value(bool *ok = nullptr) const;
        int64_t int64Value(bool *ok = nullptr) const;
        uint64_t uint64Value(bool *ok = nullptr) const;
        long_t longValue(bool *ok = nullptr) const;
        ulong_t ulongValue(bool *ok = nullptr) const;
        float32_t float32Value(bool *ok = nullptr) const;
        float64_t float64Value(bool *ok = nullptr) const;
        // fix32_t fix32Value(bool *ok = nullptr) const;
        // fix64_t fix64Value(bool *ok = nullptr) const;
        char charValue(bool *ok = nullptr) const;
        wchar_t wcharValue(bool *ok = nullptr) const;
        String stringValue(bool *ok = nullptr) const;

        const VariantArray &arrayValue(bool *ok = nullptr) const;
        const VariantList &listValue(bool *ok = nullptr) const;
        const VariantMap &mapValue(bool *ok = nullptr) const;

        VariantArray &arrayValue(bool *ok = nullptr);
        VariantList &listValue(bool *ok = nullptr);
        VariantMap &mapValue(bool *ok = nullptr);

        /** 重载操作符 */
        bool operator ==(const Variant &other) const;
        bool operator !=(const Variant &other) const;
        bool operator <(const Variant &other) const;
        bool operator <=(const Variant &other) const;
        bool operator >(const Variant &other) const;
        bool operator >=(const Variant &other) const;

        /** 是否有效类型 */
        bool isValid() const;

        /** 是否数值类型 */
        bool isNumber() const;

        /** 获取类型 */
        Type valueType() const;

        /** 获取类型大小 */
        size_t valueSize() const;

        static VariantArray   INVALID_ARRAY;    /**< 无效数组 */
        static VariantList    INVALID_LIST;     /**< 无效链表 */
        static VariantMap     INVALID_MAP;      /**< 无效map */

    protected:
        /** 赋值数值 */
        void copy(const Variant &other);

        /** 释放内存资源 */
        void releaseMemory();

        /** 比较数值大小 */
        int32_t compare(const Variant &other) const;

    protected:
        Type        mType;      /**< 数值类型 */
        size_t      mValueSize; /**< */

        union
        {
            char        mValue[sizeof(int64_t)];
            bool        mBoolValue;
            int8_t      mInt8Value;
            uint8_t     mUInt8Value;
            int16_t     mInt16Value;
            uint16_t    mUInt16Value;
            int32_t     mInt32Value;
            uint32_t    mUInt32Value;
            int64_t     mInt64Value;
            uint64_t    mUInt64Value;
            long_t      mLongValue;
            ulong_t     mULongValue;
            float32_t   mFloat32Value;
            float64_t   mFloat64Value;
            // fix32_t     mFix32Value;
            // fix64_t     mFix64Value;
            char        mCharValue;
            wchar_t     mWCharValue;
            char        *mStrValue;

            VariantArray    *mArrayValue;
            VariantList     *mListValue;
            VariantMap      *mMapValue;
        };
    };
#endif
}

#include "T3DVariant.inl"

#endif  /*__T3D_DATASTRUCT_H__*/
