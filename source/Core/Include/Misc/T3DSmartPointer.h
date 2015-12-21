
#ifndef __T3D_SMART_POINTER_H__
#define __T3D_SMART_POINTER_H__


#include "T3DPrerequisites.h"


namespace Tiny3D
{
    template <typename T>
    class SmartPtr
    {
    public:
        static const SmartPtr NULL_PTR;

        SmartPtr(T *obj = nullptr);
        SmartPtr(const SmartPtr &rkPointer);
        ~SmartPtr();

        operator T*() const;
        T &operator *() const;
        T *operator ->() const;

        SmartPtr &operator =(T *obj);
        SmartPtr &operator =(const SmartPtr &rkPointer);

        bool operator ==(T *obj) const;
        bool operator !=(T *obj) const;
        bool operator <(T *obj) const;
        bool operator <=(T *obj) const;
        bool operator >(T *obj) const;
        bool operator >=(T *obj) const;

        bool operator ==(const SmartPtr &rkPointer) const;
        bool operator !=(const SmartPtr &rkPointer) const;
        bool operator <(const SmartPtr &rkPointer) const;
        bool operator <=(const SmartPtr &rkPointer) const;
        bool operator >(const SmartPtr &rkPointer) const;
        bool operator >=(const SmartPtr &rkPointer) const;

    private:
        T   *m_pkObject;
    };
}

#include "T3DSmartPointer.inl"


#endif  /*__T3D_SMART_POINTER_H__*/
