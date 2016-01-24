
#ifndef __T3D_SMART_POINTER_H__
#define __T3D_SMART_POINTER_H__


#include "T3DPrerequisites.h"


namespace Tiny3D
{
    class T3D_ENGINE_API SmartBasePtr
    {
    protected:
        virtual ~SmartBasePtr();

        Object   *mReferObject;
    };

    template <typename T, typename Base = SmartBasePtr>
    class SmartPtr : public Base
    {
    public:
        static const SmartPtr NULL_PTR;

        SmartPtr(T *obj = nullptr);
        SmartPtr(const SmartPtr &rkPointer);

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
    };

    #define T3D_DECLARE_SMART_PTR(classname, baseclass) typedef SmartPtr<classname, baseclass##Ptr> classname##Ptr
}

#include "T3DSmartPtr.inl"


#endif  /*__T3D_SMART_POINTER_H__*/
