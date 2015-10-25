

namespace Tiny3D
{
    inline bool Variant::isValid() const
    {
        return (mType != E_NONE);
    }

    inline Variant::Variant()
        : mType(E_NONE)
        , mValueSize(0)
    {
        memset(mValue, 0, sizeof(mValue));
    }

    inline Variant::Variant(bool value)
        : mType(E_BOOL)
        , mValueSize(sizeof(bool))
    {
        memset(mValue, 0, sizeof(mValue));
        mBoolValue = value;
    }

    inline Variant::Variant(int32_t value)
        : mType(E_INT32)
        , mValueSize(sizeof(int32_t))
    {
        memset(mValue, 0, sizeof(mValue));
        mInt32Value = value;
    }

    inline Variant::Variant(uint32_t value)
        : mType(E_UINT32)
        , mValueSize(sizeof(uint32_t))
    {
        memset(mValue, 0, sizeof(mValue));
        mUInt32Value = value;
    }

    inline Variant::Variant(int64_t value)
        : mType(E_INT64)
        , mValueSize(sizeof(int64_t))
    {
        memset(mValue, 0, sizeof(mValue));
        mInt64Value = value;
    }

    inline Variant::Variant(uint64_t value)
        : mType(E_UINT64)
        , mValueSize(sizeof(uint64_t))
    {
        memset(mValue, 0, sizeof(mValue));
        mUInt64Value = value;
    }

    inline Variant::Variant(long_t value)
        : mType(E_LONG)
        , mValueSize(sizeof(long_t))
    {
        memset(mValue, 0, sizeof(mValue));
        mLongValue = value;
    }

    inline Variant::Variant(Real value)
        : mType(E_REAL)
        , mValueSize(sizeof(Real))
    {
        memset(mValue, 0, sizeof(mValue));
        mRealValue = value;
    }

    inline Variant::Variant(char value)
        : mType(E_CHAR)
        , mValueSize(sizeof(char))
    {
        memset(mValue, 0, sizeof(mValue));
        mCharValue = value;
    }

    inline Variant::Variant(const String &value)
        : mType(E_STRING)
        , mValueSize(0)
    {
        memset(mValue, 0, sizeof(mValue));
        mValueSize = (value.length() > 0 ? value.length() + 1 : 1);
        mStrValue = new char[mValueSize];
        memcpy(mStrValue, value.c_str(), mValueSize-1);
        mStrValue[mValueSize-1] = 0;
    }

    inline Variant::Variant(const VariantArray &value)
        : mType(E_ARRAY)
        , mValueSize(sizeof(VariantArray))
    {
        memset(mValue, 0, sizeof(mValue));
        mArrayValue = new VariantArray(value);
    }

    inline Variant::Variant(const VariantList &value)
        : mType(E_LIST)
        , mValueSize(sizeof(VariantList))
    {
        memset(mValue, 0, sizeof(mValue));
        mListValue = new VariantList(value);
    }

    inline Variant::Variant(const VariantMap &value)
        : mType(E_MAP)
        , mValueSize(sizeof(VariantMap))
    {
        memset(mValue, 0, sizeof(mValue));
        mMapValue = new VariantMap(value);
    }

    inline Variant::Variant(const Variant &other)
    {
        copy(other);
    }

    inline Variant &Variant::operator =(const Variant &other)
    {
        copy(other);
        return *this;
    }

    inline Variant::~Variant()
    {
        releaseMemory();
    }

    inline void Variant::setBool(bool value)
    {
        releaseMemory();
        mType = E_BOOL;
        mValueSize = sizeof(bool);
        mBoolValue = value;
    }

    inline void Variant::setInt32(int32_t value)
    {
        releaseMemory();
        mType = E_INT32;
        mValueSize = sizeof(int32_t);
        mInt32Value = value;
    }

    inline void Variant::setUInt32(uint32_t value)
    {
        releaseMemory();
        mType = E_UINT32;
        mValueSize = sizeof(uint32_t);
        mUInt32Value = value;
    }

    inline void Variant::setInt64(int64_t value)
    {
        releaseMemory();
        mType = E_INT64;
        mValueSize = sizeof(int64_t);
        mInt64Value = value;
    }

    inline void Variant::setUInt64(uint64_t value)
    {
        releaseMemory();
        mType = E_UINT64;
        mValueSize = sizeof(uint64_t);
        mUInt64Value = value;
    }

    inline void Variant::setLong(long_t value)
    {
        releaseMemory();
        mType = E_LONG;
        mValueSize = sizeof(long_t);
        mLongValue = value;
    }

    inline void Variant::setReal(Real value)
    {
        releaseMemory();
        mType = E_REAL;
        mValueSize = sizeof(Real);
        mRealValue = value;
    }

    inline void Variant::setDouble(double value)
    {
        releaseMemory();
        mType = E_DOUBLE;
        mValueSize = sizeof(double);
        mDoubleValue = value;
    }

    inline void Variant::setFloat(float value)
    {
        releaseMemory();
        mType = E_FLOAT;
        mValueSize = sizeof(float);
        mFloatValue = value;
    }

    inline void Variant::setChar(char value)
    {
        releaseMemory();
        mType = E_CHAR;
        mValueSize = sizeof(char);
        mCharValue = value;
    }

    inline void Variant::setString(const String &value)
    {
        releaseMemory();
        mType = E_STRING;
        mValueSize = (value.length() > 0 ? value.length() + 1 : 1);
        mStrValue = new char[mValueSize];
        memcpy(mStrValue, value.c_str(), mValueSize-1);
        mStrValue[mValueSize-1] = 0;
    }

    inline void Variant::setArray(const VariantArray &value)
    {
        releaseMemory();
        mType = E_ARRAY;
        mValueSize = sizeof(VariantArray);
        mArrayValue = new VariantArray(value);
    }

    inline void Variant::setList(const VariantList &value)
    {
        releaseMemory();
        mType = E_LIST;
        mValueSize = sizeof(VariantList);
        mListValue = new VariantList(value);
    }

    inline void Variant::setMap(const VariantMap &value)
    {
        releaseMemory();
        mType = E_MAP;
        mValueSize = sizeof(VariantMap);
        mMapValue = new VariantMap(value);
    }

    inline bool Variant::isNumber() const
    {
        return (E_INT32 == mType || E_UINT32 == mType 
            || E_INT64 == mType || E_UINT64 == mType
            || E_LONG == mType || E_REAL == mType 
            || E_DOUBLE == mType || E_FLOAT == mType
            || E_CHAR == mType || E_BOOL == mType);
    }

    inline bool Variant::boolValue(bool *ok /* = nullptr */) const
    {
        bool val = false;
        bool ret = false;

        if (isNumber())
        {
            val = mBoolValue;
            ret = true;
        }

        if (ok != nullptr)
            *ok = ret;

        return val;
    }

    inline int32_t Variant::int32Value(bool *ok /* = nullptr */) const
    {
        int32_t val = 0;
        bool ret = false;

        if (isNumber())
        {
            val = mInt32Value;
            ret = true;
        }

        if (ok != nullptr)
            *ok = ret;

        return val;
    }

    inline uint32_t Variant::uint32Value(bool *ok /* = nullptr */) const
    {
        uint32_t val = 0;
        bool ret = false;

        if (isNumber())
        {
            val = mUInt32Value;
            ret = true;
        }

        if (ok != nullptr)
            *ok = ret;

        return val;
    }

    inline int64_t Variant::int64Value(bool *ok /* = nullptr */) const
    {
        int64_t val = 0;
        bool ret = false;

        if (isNumber())
        {
            val = mInt64Value;
            ret = true;
        }

        if (ok != nullptr)
            *ok = ret;

        return val;
    }

    inline uint64_t Variant::uint64Value(bool *ok /* = nullptr */) const
    {
        uint64_t val = 0;
        bool ret = false;

        if (isNumber())
        {
            val = mUInt64Value;
            ret = true;
        }

        if (ok != nullptr)
            *ok = ret;

        return val;
    }

    inline long_t Variant::longValue(bool *ok /* = nullptr */) const
    {
        long_t val = 0;
        bool ret = false;

        if (isNumber())
        {
            val = mLongValue;
            ret = true;
        }

        if (ok != nullptr)
            *ok = ret;

        return val;
    }

    inline Real Variant::realValue(bool *ok /* = nullptr */) const
    {
        Real val = 0;
        bool ret = false;

        if (isNumber())
        {
            val = mRealValue;
            ret = true;
        }

        if (ok != nullptr)
            *ok = ret;

        return val;
    }

    inline double Variant::doubleValue(bool *ok /* = nullptr */) const
    {
        double val = 0;
        bool ret = false;

        if (isNumber())
        {
            val = mDoubleValue;
            ret = true;
        }

        if (ok != nullptr)
            *ok = ret;

        return val;
    }

    inline float Variant::floatValue(bool *ok /* = nullptr */) const
    {
        float val = 0;
        bool ret = false;

        if (isNumber())
        {
            val = mFloatValue;
            ret = true;
        }

        if (ok != nullptr)
            *ok = ret;

        return val;
    }

    inline char Variant::charValue(bool *ok /* = nullptr */) const
    {
        char val = 0;
        bool ret = false;

        if (isNumber())
        {
            val = mCharValue;
            ret = true;
        }

        if (ok != nullptr)
            *ok = ret;

        return val;
    }

    inline String Variant::stringValue(bool *ok /* = nullptr */) const
    {
        String val;
        bool ret = false;

        if (E_STRING == mType)
        {
            val.assign(mStrValue);
            ret = true;
        }

        if (ok != nullptr)
            *ok = ret;

        return val;
    }

    inline const VariantArray &Variant::arrayValue(bool *ok /* = nullptr */) const
    {
        if (E_ARRAY == mType)
        {
            if (ok != nullptr)
                *ok = true;

            return *mArrayValue;
        }

        if (ok != nullptr)
            *ok = false;

        return INVALID_ARRAY;
    }

    inline const VariantList &Variant::listValue(bool *ok /* = nullptr */) const
    {
        if (E_LIST == mType)
        {
            if (ok != nullptr)
                *ok = true;

            return *mListValue;
        }

        if (ok != nullptr)
            *ok = false;

        return INVALID_LIST;
    }

    inline const VariantMap &Variant::mapValue(bool *ok /* = nullptr */) const
    {
        if (E_MAP == mType)
        {
            if (ok != nullptr)
                *ok = true;

            return *mMapValue;
        }

        if (ok != nullptr)
            *ok = false;

        return INVALID_MAP;
    }

    inline VariantArray &Variant::arrayValue(bool *ok /* = nullptr */)
    {
        if (E_ARRAY == mType)
        {
            if (ok != nullptr)
                *ok = true;

            return *mArrayValue;
        }

        if (ok != nullptr)
            *ok = false;

        return INVALID_ARRAY;
    }

    inline VariantList &Variant::listValue(bool *ok /* = nullptr */)
    {
        if (E_LIST == mType)
        {
            if (ok != nullptr)
                *ok = true;

            return *mListValue;
        }

        if (ok != nullptr)
            *ok = false;

        return INVALID_LIST;
    }

    inline VariantMap &Variant::mapValue(bool *ok /* = nullptr */)
    {
        if (E_MAP == mType)
        {
            if (ok != nullptr)
                *ok = true;

            return *mMapValue;
        }

        if (ok != nullptr)
            *ok = false;

        return INVALID_MAP;
    }

    inline Variant::EType Variant::valueType() const
    {
        return mType;
    }

    inline bool Variant::operator ==(const Variant &other) const
    {
        return compare(other) == 0;
    }

    inline bool Variant::operator !=(const Variant &other) const
    {
        return compare(other) != 0;
    }

    inline bool Variant::operator <(const Variant &other) const
    {
        return compare(other) < 0;
    }

    inline bool Variant::operator <=(const Variant &other) const
    {
        return compare(other) <= 0;
    }

    inline bool Variant::operator >(const Variant &other) const
    {
        return compare(other) > 0;
    }

    inline bool Variant::operator >=(const Variant &other) const
    {
        return compare(other) >= 0;
    }
}

