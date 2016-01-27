
#ifndef __T3D_SMART_POINTER_H__
#define __T3D_SMART_POINTER_H__


#include "T3DPrerequisites.h"
#include <type_traits>


namespace Tiny3D
{
    template <typename T>
    class SmartPtr
    {
    public:
        static const SmartPtr NULL_PTR;

        SmartPtr(T *obj = nullptr)
        {
            mReferObject = obj;

            if (mReferObject != nullptr)
            {
                mReferObject->acquire();
            }
        }

        SmartPtr(const SmartPtr &rkPointer)
        {
            mReferObject = rkPointer.mReferObject;

            if (mReferObject != nullptr)
            {
                mReferObject->acquire();
            }
        }

        template <typename T2>
        SmartPtr(const SmartPtr<T2> &rkOther, 
            typename std::enable_if<std::is_convertible<T2 *, T *>::value, void>::type ** = 0)
        {
            mReferObject = rkOther;

            if (mReferObject != nullptr)
            {
                mReferObject->acquire();
            }
        }

        template <typename T2>
        SmartPtr(const SmartPtr<T2> &rkOther, const std::_Static_tag &)
        {
            mReferObject = rkOther;

            if (mReferObject != nullptr)
            {
                mReferObject->acquire();
            }
        }

        virtual ~SmartPtr()
        {
            if (mReferObject != nullptr)
            {
                mReferObject->release();
            }
        }

        operator T*() const
        {
            return (T*)mReferObject;
        }

        T &operator *() const
        {
            return (T&)(*mReferObject);
        }

        T *operator ->() const
        {
            return (T*)mReferObject;
        }

        SmartPtr &operator =(T *obj)
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

        SmartPtr &operator =(const SmartPtr &rkPointer)
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

        template <typename T2>
        SmartPtr &operator =(const SmartPtr<T2> &rkOther)
        {
            if (mReferObject != rkOther)
            {
                if (rkOther != nullptr)
                {
                    rkOther->acquire();
                }

                if (mReferObject != nullptr)
                {
                    mReferObject->release();
                }

                mReferObject = rkOther;
            }

            return *this;
        }

    protected:
        Object  *mReferObject;
    };

    template <typename T1, typename T2>
    SmartPtr<T1> smart_pointer_cast(const SmartPtr<T2> &rkOther)
    {
        return SmartPtr<T1>(rkOther, std::_Static_tag());
    }

    template <typename T>
    const SmartPtr<T> SmartPtr<T>::NULL_PTR(nullptr);

    #define T3D_DECLARE_SMART_PTR(classname)    typedef SmartPtr<classname> classname##Ptr
}


#endif  /*__T3D_SMART_POINTER_H__*/
