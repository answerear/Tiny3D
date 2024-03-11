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


#ifndef __T3D_SMART_PTR_H__
#define __T3D_SMART_PTR_H__


#include "T3DSystemPrerequisites.h"
#include <type_traits>
#include "Object/T3DObject.h"


#if defined (T3D_OS_WINDOWS)
#if _MSC_VER >= 1900
namespace std
{
    struct _Static_tag {};
}
#endif
#else
namespace std
{
    struct _Static_tag {};
}
#endif


namespace Tiny3D
{
    /**
     * @class   SmartPtr
     * @brief   智能指针类
     * @tparam  T   Generic type parameter.
     */
    template <typename T>
    class SmartPtr
    {
    public:
        typedef T element_type;
        
        static const SmartPtr NULL_PTR; /**< The null pointer */

        /**
         * @fn  SmartPtr::SmartPtr(T *obj = nullptr)
         * @brief   构造函数
         * @param [in,out]  obj (Optional) If non-null, the object.
         */
        SmartPtr(Object *obj = nullptr)
        {
            mReferObject = obj;

            if (mReferObject != nullptr)
            {
                mReferObject->acquire();
            }
        }

        /**
         * @fn  SmartPtr::SmartPtr(const SmartPtr &rkPointer)
         * @brief   Copy constructor
         * @param   rkPointer   The rk pointer.
         */
        SmartPtr(const SmartPtr &rkPointer)
        {
            mReferObject = rkPointer.mReferObject;

            if (mReferObject != nullptr)
            {
                mReferObject->acquire();
            }
        }

        /**
         * @fn  template <typename T2> SmartPtr::SmartPtr(
         *      const SmartPtr<T2> &rkOther, 
         *      typename std::enable_if<std::is_convertible<T2 *, 
         *      T *>::value, void>::type ** = 0)
         * @brief   Constructor
         * @tparam  T2  Generic type parameter.
         * @param           rkOther     The rk other.
         * @param [in,out]  parameter2  (Optional) If non-null, the second parameter.
         */
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

        /**
         * @fn  template <typename T2> SmartPtr::SmartPtr(
         *      const SmartPtr<T2> &rkOther, const std::_Static_tag &)
         * @brief   Constructor
         * @tparam  T2  Generic type parameter.
         * @param   rkOther     The rk other.
         * @param   parameter2  The second parameter.
         */
        template <typename T2>
        SmartPtr(const SmartPtr<T2> &rkOther, const std::_Static_tag &)
        {
            mReferObject = rkOther;

            if (mReferObject != nullptr)
            {
                mReferObject->acquire();
            }
        }

        /**
         * @fn  virtual SmartPtr::~SmartPtr()
         * @brief   Destructor
         */
        virtual ~SmartPtr()
        {
            if (mReferObject != nullptr)
            {
                mReferObject->release();
            }
        }

        /**
         * @fn  operator SmartPtr::T*() const
         * @brief   T* casting operator
         * @return  The result of the operation.
         */
        operator T*() const
        {
            return (T*)mReferObject;
        }

        /**
         * @fn  T SmartPtr::&operator*() const
         * @brief   Indirection operator
         * @return  The result of the operation.
         */
        T &operator *() const
        {
            return (T&)(*mReferObject);
        }

        /**
         * @fn  T SmartPtr::*operator->() const
         * @brief   Member dereference operator
         * @return  The dereferenced object.
         */
        T *operator ->() const
        {
            return (T*)mReferObject;
        }

        /**
         * @fn  SmartPtr SmartPtr::&operator=(T *obj)
         * @brief   Assignment operator
         * @param [in,out]  obj If non-null, the object.
         * @return  A shallow copy of this object.
         */
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

        /**
         * @fn  SmartPtr SmartPtr::&operator=(const SmartPtr &rkPointer)
         * @brief   Assignment operator
         * @param   rkPointer   The rk pointer.
         * @return  A shallow copy of this object.
         */
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

        /**
         * @fn  template <typename T2> SmartPtr 
         *      SmartPtr::&operator=(const SmartPtr<T2> &rkOther)
         * @brief   Assignment operator
         * @tparam  T2  Generic type parameter.
         * @param   rkOther The rk other.
         * @return  The result of the operation.
         */
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

        T *get() const
        {
            return (T*)mReferObject;
        }
    protected:
        /// The refer object
        Object  *mReferObject {nullptr};
    };

    template <typename T1, typename T2>
    SmartPtr<T1> smart_pointer_cast(const SmartPtr<T2> &rkOther)
    {
        return SmartPtr<T1>(rkOther, std::_Static_tag());
    }

    template <typename T>
    const SmartPtr<T> SmartPtr<T>::NULL_PTR(nullptr);

    template <typename T>
    class ThreadSafePtr : public SmartPtr<T>
    {
    public:
        ThreadSafePtr(Object *obj = nullptr) : SmartPtr<T>(obj)
        {
            if (mReferObject != nullptr)
            {
                mReferObject->enableThreadSafe(true);
            }
        }

        ThreadSafePtr(const ThreadSafePtr &other) : SmartPtr<T>(other)
        {
            if (mReferObject != nullptr)
            {
                mReferObject->enableThreadSafe(true);
            }
        }

        ThreadSafePtr(const SmartPtr<T> &other) : SmartPtr<T>(other)
        {
            if (mReferObject != nullptr)
            {
                mReferObject->enableThreadSafe(true);
            }
        }

        ThreadSafePtr &operator=(const ThreadSafePtr &other)
        {
            SmartPtr<T>::operator=(other);
            if (mReferObject != nullptr)
            {
                mReferObject->enableThreadSafe(true);
            }
            return *this;
        }

        // 从 SmartPtr 赋值
        ThreadSafePtr &operator=(const SmartPtr<T> &other)
        {
            SmartPtr<T>::operator=(other);
            if (mReferObject != nullptr)
            {
                mReferObject->enableThreadSafe(true);
            }
            return *this;
        }

        ~ThreadSafePtr() override
        {
            if (mReferObject != nullptr)
            {
                mReferObject->enableThreadSafe(false);
            }
        }
    };
    
    #define T3D_DECLARE_SMART_PTR(classname)    \
        using classname##Ptr = SmartPtr<classname>; \
        using classname##SafePtr = ThreadSafePtr<classname>;
}

namespace rttr
{
    class type;

    template<typename T>
    struct wrapper_mapper<Tiny3D::SmartPtr<T>>
    {
        using wrapped_type = decltype(Tiny3D::SmartPtr<T>().get());
        using type = Tiny3D::SmartPtr<T>;

        static RTTR_INLINE wrapped_type get(const type& obj)
        {
            return obj.get();
        }

        static RTTR_INLINE type create(const wrapped_type& t)
        {
            return type(t);
        }

        template<typename U>
        static Tiny3D::SmartPtr<U> convert(const type& source, bool& ok)
        {
            if (auto p = rttr_cast<typename Tiny3D::SmartPtr<U>::element_type*>(source.get()))
            {
                ok = true;
                return Tiny3D::SmartPtr<U>(p);
            }
            else
            {
                ok = false;
                return Tiny3D::SmartPtr<U>();
            }
        }
    };
}

#endif  /*__T3D_SMART_PTR_H__*/
