

#ifndef __T3D_VARIANT_H__
#define __T3D_VARIANT_H__


#include "T3DPrerequisites.h"


namespace Tiny3D
{
    class T3D_ENGINE_API Variant
    {
    public:
        enum EType
        {
            E_NONE = 0,
            E_BOOL,
            E_INT32,
            E_UINT32,
            E_INT64,
            E_UINT64,
            E_LONG,
            E_REAL,
            E_DOUBLE,
            E_FLOAT,
            E_CHAR,
            E_STRING,
            E_ARRAY,
            E_LIST,
            E_MAP,
        };

        Variant();
        Variant(bool value);
        Variant(int32_t value);
        Variant(uint32_t value);
        Variant(int64_t value);
        Variant(uint64_t value);
        Variant(long_t value);
        Variant(Real value);
        Variant(char value);
        Variant(const char *value);
        Variant(const String &value);
        Variant(const VariantArray &value);
        Variant(const VariantList &value);
        Variant(const VariantMap &value);

        Variant(const Variant &other);
        Variant &operator =(const Variant &other);

        virtual ~Variant();

        void setBool(bool value);
        void setInt32(int32_t value);
        void setUInt32(uint32_t value);
        void setInt64(int64_t value);
        void setUInt64(uint64_t value);
        void setLong(long_t value);
        void setReal(Real value);
        void setDouble(double value);
        void setFloat(float value);
        void setChar(char value);
        void setString(const String &value);
        void setArray(const VariantArray &value);
        void setList(const VariantList &value);
        void setMap(const VariantMap &value);

        bool boolValue(bool *ok = nullptr) const;
        int32_t int32Value(bool *ok = nullptr) const;
        uint32_t uint32Value(bool *ok = nullptr) const;
        int64_t int64Value(bool *ok = nullptr) const;
        uint64_t uint64Value(bool *ok = nullptr) const;
        long_t longValue(bool *ok = nullptr) const;
        Real realValue(bool *ok = nullptr) const;
        double doubleValue(bool *ok = nullptr) const;
        float floatValue(bool *ok = nullptr) const;
        char charValue(bool *ok = nullptr) const;
        String stringValue(bool *ok = nullptr) const;
        const VariantArray &arrayValue(bool *ok = nullptr) const;
        const VariantList &listValue(bool *ok = nullptr) const;
        const VariantMap &mapValue(bool *ok = nullptr) const;
        VariantArray &arrayValue(bool *ok = nullptr);
        VariantList &listValue(bool *ok = nullptr);
        VariantMap &mapValue(bool *ok = nullptr);

        bool operator ==(const Variant &other) const;
        bool operator !=(const Variant &other) const;
        bool operator <(const Variant &other) const;
        bool operator <=(const Variant &other) const;
        bool operator >(const Variant &other) const;
        bool operator >=(const Variant &other) const;

        bool isValid() const;
        bool isNumber() const;

        EType valueType() const;

        static VariantArray   INVALID_ARRAY;
        static VariantList    INVALID_LIST;
        static VariantMap     INVALID_MAP;

    protected:
        void copy(const Variant &other);
        void releaseMemory();
        int32_t compare(const Variant &other) const;

    private:
        EType   mType;
        int32_t mValueSize;

        union
        {
            char        mValue[sizeof(int64_t)];
            bool        mBoolValue;
            int32_t     mInt32Value;
            uint32_t    mUInt32Value;
            int64_t     mInt64Value;
            uint64_t    mUInt64Value;
            long_t      mLongValue;
            double      mDoubleValue;
            float       mFloatValue;
            Real        mRealValue;
            char        mCharValue;
            char        *mStrValue;

            VariantArray    *mArrayValue;
            VariantList     *mListValue;
            VariantMap      *mMapValue;
        };
    };
}


#include "T3DVariant.inl"


#endif  /*__T3D_VARIANT_H__*/
