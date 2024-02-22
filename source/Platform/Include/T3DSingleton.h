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

#ifndef __T3D_SINGLETON_H__
#define __T3D_SINGLETON_H__


#include "T3DType.h"
#include "T3DMacro.h"
#include "T3DNoncopyable.h"
#include <vector>


#if defined (T3D_OS_WINDOWS)
#pragma warning(disable:4661)
#endif


// #define T3D_INIT_SINGLETON(T) //template<> T::pointer Singleton<T>::m_pInstance = nullptr;


namespace Tiny3D
{
    template <typename T>
    class Singleton : public Noncopyable
    {
    public:
        typedef T  value_type;
        typedef T* pointer;
        typedef T& reference;

        Singleton()
        {
            T3D_ASSERT(m_pInstance == nullptr, "m_pInstance == nullptr");
            m_pInstance = static_cast<pointer>(this);
        }

        virtual ~Singleton()
        {
            m_pInstance = nullptr;
        }

        static reference getInstance()
        {
            return *m_pInstance;
        }

        static pointer getInstancePtr()
        {
            return m_pInstance;
        }

    protected:
        static pointer m_pInstance;
    };

    template<typename T>
    typename Singleton<T>::pointer Singleton<T>::m_pInstance = nullptr;
}


#endif  /*__T3D_SINGLETON_H__*/
