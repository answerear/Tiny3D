

namespace Tiny3D
{
    template <typename T>
    SmartPtr<T>::SmartPtr(T *obj)
        : m_pkObject(obj)
    {
        if (m_pkObject != nullptr)
        {
            m_pkObject->acquire();
        }
    }

    template <typename T>
    SmartPtr<T>::SmartPtr(const SmartPtr &rkPointer)
        : m_pkObject(rkPointer.m_pkObject)
    {
        if (m_pkObject != nullptr)
        {
            m_pkObject->acquire();
        }
    }

    template <typename T>
    SmartPtr<T>::~SmartPtr()
    {
        if (m_pkObject != nullptr)
        {
            m_pkObject->release();
        }
    }

    template <typename T>
    SmartPtr<T>::operator T*() const
    {
        return m_pkObject;
    }

    template <typename T>
    T &SmartPtr<T>::operator *() const
    {
        return *m_pkObject;
    }

    template <typename T>
    T *SmartPtr<T>::operator ->() const
    {
        return m_pkObject;
    }

    template <typename T>
    SmartPtr<T> &SmartPtr<T>::operator =(T *obj)
    {
        if (m_pkObject != obj)
        {
            if (obj != nullptr)
            {
                obj->acquire();
            }

            if (m_pkObject != nullptr)
            {
                m_pkObject->release();
            }

            m_pkObject = obj;
        }

        return *this;
    }

    template <typename T>
    SmartPtr<T> &SmartPtr<T>::operator =(const SmartPtr<T> &rkPointer)
    {
        if (m_pkObject != rkPointer.m_pkObject)
        {
            if (rkPointer.m_pkObject != nullptr)
            {
                rkPointer.m_pkObject->acquire();
            }

            if (m_pkObject != nullptr)
            {
                m_pkObject->release();
            }

            m_pkObject = rkPointer.m_pkObject;
        }

        return *this;
    }

    template <typename T>
    bool SmartPtr<T>::operator ==(T *obj) const
    {
        return m_pkObject == obj;
    }

    template <typename T>
    bool SmartPtr<T>::operator !=(T *obj) const
    {
        return m_pkObject != obj;
    }

    template <typename T>
    bool SmartPtr<T>::operator <(T *obj) const
    {
        return m_pkObject < obj;
    }

    template <typename T>
    bool SmartPtr<T>::operator <=(T *obj) const
    {
        return m_pkObject <= obj;
    }

    template <typename T>
    bool SmartPtr<T>::operator >(T *obj) const
    {
        return m_pkObject > obj;
    }

    template <typename T>
    bool SmartPtr<T>::operator >=(T *obj) const
    {
        return m_pkObject >= obj;
    }

    template <typename T>
    bool SmartPtr<T>::operator ==(const SmartPtr<T> &rkPointer) const
    {
        return m_pkObject == rkPointer.m_pkObject;
    }

    template <typename T>
    bool SmartPtr<T>::operator !=(const SmartPtr<T> &rkPointer) const
    {
        return m_pkObject != rkPointer.m_pkObject;
    }

    template <typename T>
    bool SmartPtr<T>::operator <(const SmartPtr<T> &rkPointer) const
    {
        return m_pkObject < rkPointer.m_pkObject;
    }

    template <typename T>
    bool SmartPtr<T>::operator <=(const SmartPtr<T> &rkPointer) const
    {
        return m_pkObject <= rkPointer.m_pkObject;
    }

    template <typename T>
    bool SmartPtr<T>::operator >(const SmartPtr<T> &rkPointer) const
    {
        return m_pkObject > rkPointer.m_pkObject;
    }

    template <typename T>
    bool SmartPtr<T>::operator >=(const SmartPtr<T> &rkPointer) const
    {
        return m_pkObject >= rkPointer.m_pkObject;
    }

    template <typename T>
    const SmartPtr<T> SmartPtr<T>::NULL_PTR(nullptr);
}
