

#include "DataStruct/T3DVariant.h"


namespace Tiny3D
{
    VariantArray    Variant::INVALID_ARRAY;
    VariantList     Variant::INVALID_LIST;
    VariantMap      Variant::INVALID_MAP;


    void Variant::copy(const Variant &other)
    {
        mType = other.mType;
        mValueSize = other.mValueSize;

        switch (mType)
        {
        case E_BOOL:
        case E_INT32:
        case E_UINT32:
        case E_INT64:
        case E_UINT64:
        case E_LONG:
        case E_REAL:
        case E_DOUBLE:
        case E_FLOAT:
        case E_CHAR:
            {
                memcpy(mValue, other.mValue, mValueSize);
            }
            break;
        case E_STRING:
            {
                mStrValue = new char[mValueSize];
                memcpy(mStrValue, other.mStrValue, mValueSize);
            }
            break;
        case E_ARRAY:
            {
                mArrayValue = new VariantArray(*other.mArrayValue);
            }
            break;
        case E_LIST:
            {
                mListValue = new VariantList(*other.mListValue);
            }
            break;
        case E_MAP:
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
        case E_STRING:
            {
                delete []mStrValue;
                mStrValue = nullptr;
            }
            break;
        case E_ARRAY:
            {
                delete mArrayValue;
                mArrayValue = nullptr;
            }
            break;
        case E_LIST:
            {
                delete mListValue;
                mListValue = nullptr;
            }
            break;
        case E_MAP:
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
        int32_t ret = false;

        if (isNumber())
        {
            ret = memcmp(mValue, other.mValue, mValueSize);
        }
        else if (E_STRING == mType)
        {
            strncmp(mStrValue, other.mStrValue, mValueSize);
        }
        else if (E_ARRAY == mType)
        {
            if (*mArrayValue == *other.mArrayValue)
                ret = 0;
            else if (*mArrayValue < *other.mArrayValue)
                ret = -1;
            else if (*mArrayValue > *other.mArrayValue)
                ret = 1;
        }
        else if (E_LIST == mType)
        {
            if (*mListValue == *other.mListValue)
                ret = 0;
            else if (*mListValue < *other.mListValue)
                ret = -1;
            else if (*mListValue > *other.mListValue)
                ret = 1;
        }
        else if (E_MAP == mType)
        {
            if (*mMapValue == *other.mMapValue)
                ret = 0;
            else if (*mMapValue < *other.mMapValue)
                ret = -1;
            else if (*mMapValue > *other.mMapValue)
                ret = 1;
        }

        return ret;
    }
}
