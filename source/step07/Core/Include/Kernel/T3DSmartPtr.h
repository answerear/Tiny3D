/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Answer Wong
 * For latest info, see https://github.com/asnwerear/Tiny3D
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/


#ifndef __T3D_SMART_PTR_H__
#define __T3D_SMART_PTR_H__


#include "T3DPrerequisites.h"
#include <type_traits>


#if _MSC_VER >= 1900
namespace std
{
    struct _Static_tag {};
}
#endif


namespace Tiny3D
{
    /**
     * @brief 智能指针类
     */
    template <typename T>
    class SmartPtr
    {
    public:
        static const SmartPtr NULL_PTR;

        /**
         * @brief 构造函数
         */
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

    #define T3D_DECLARE_SMART_PTR(classname)    \
            typedef SmartPtr<classname> classname##Ptr
}


#endif  /*__T3D_APP_EVENT_LISTENER_H__*/
