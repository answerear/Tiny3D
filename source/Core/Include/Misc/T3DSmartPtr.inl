

namespace Tiny3D
{
    template <typename T, typename Base>
    SmartPtr<T, Base>::SmartPtr(T *obj)
    {
        mReferObject = obj;

        if (mReferObject != nullptr)
        {
            mReferObject->acquire();
        }
    }

    template <typename T, typename Base>
    SmartPtr<T, Base>::SmartPtr(const SmartPtr<T, Base> &rkPointer)
    {
        mReferObject = rkPointer.mReferObject;

        if (mReferObject != nullptr)
        {
            mReferObject->acquire();
        }
    }

    template <typename T, typename Base>
    SmartPtr<T, Base>::operator T*() const
    {
        return (T*)mReferObject;
    }

    template <typename T, typename Base>
    T &SmartPtr<T, Base>::operator *() const
    {
        return (T&)(*mReferObject);
    }

    template <typename T, typename Base>
    T *SmartPtr<T, Base>::operator ->() const
    {
        return (T*)mReferObject;
    }

    template <typename T, typename Base>
    SmartPtr<T, Base> &SmartPtr<T, Base>::operator =(T *obj)
    {
        if (mReferObject != obj)
        {
            if (obj != nullptr)
            {
                obj->acquire();
            }

            if (mReferObject != nullptr)
            {
                mReferObject->release();
            }

            mReferObject = obj;
        }

        return *this;
    }

    template <typename T, typename Base>
    SmartPtr<T, Base> &SmartPtr<T, Base>::operator =(const SmartPtr<T, Base> &rkPointer)
    {
        if (mReferObject != rkPointer.mReferObject)
        {
            if (rkPointer.mReferObject != nullptr)
            {
                rkPointer.mReferObject->acquire();
            }

            if (mReferObject != nullptr)
            {
                mReferObject->release();
            }

            mReferObject = rkPointer.mReferObject;
        }

        return *this;
    }

    template <typename T, typename Base>
    bool SmartPtr<T, Base>::operator ==(T *obj) const
    {
        return mReferObject == obj;
    }

    template <typename T, typename Base>
    bool SmartPtr<T, Base>::operator !=(T *obj) const
    {
        return mReferObject != obj;
    }

    template <typename T, typename Base>
    bool SmartPtr<T, Base>::operator <(T *obj) const
    {
        return mReferObject < obj;
    }

    template <typename T, typename Base>
    bool SmartPtr<T, Base>::operator <=(T *obj) const
    {
        return mReferObject <= obj;
    }

    template <typename T, typename Base>
    bool SmartPtr<T, Base>::operator >(T *obj) const
    {
        return mReferObject > obj;
    }

    template <typename T, typename Base>
    bool SmartPtr<T, Base>::operator >=(T *obj) const
    {
        return mReferObject >= obj;
    }

    template <typename T, typename Base>
    bool SmartPtr<T, Base>::operator ==(const SmartPtr<T, Base> &rkPointer) const
    {
        return mReferObject == rkPointer.mReferObject;
    }

    template <typename T, typename Base>
    bool SmartPtr<T, Base>::operator !=(const SmartPtr<T, Base> &rkPointer) const
    {
        return mReferObject != rkPointer.mReferObject;
    }

    template <typename T, typename Base>
    bool SmartPtr<T, Base>::operator <(const SmartPtr<T, Base> &rkPointer) const
    {
        return mReferObject < rkPointer.mReferObject;
    }

    template <typename T, typename Base>
    bool SmartPtr<T, Base>::operator <=(const SmartPtr<T, Base> &rkPointer) const
    {
        return mReferObject <= rkPointer.mReferObject;
    }

    template <typename T, typename Base>
    bool SmartPtr<T, Base>::operator >(const SmartPtr<T, Base> &rkPointer) const
    {
        return mReferObject > rkPointer.mReferObject;
    }

    template <typename T, typename Base>
    bool SmartPtr<T, Base>::operator >=(const SmartPtr<T, Base> &rkPointer) const
    {
        return mReferObject >= rkPointer.mReferObject;
    }

    template <typename T, typename Base>
    const SmartPtr<T, Base> SmartPtr<T, Base>::NULL_PTR(nullptr);
}
