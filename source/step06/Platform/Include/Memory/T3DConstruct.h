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

#ifndef __T3D_CONSTRUCT_H__
#define __T3D_CONSTRUCT_H__


#include <new.h>


namespace Tiny3D
{
    template <typename T1, typename T2>
    inline void Construct(T1 *p, const T2 &value)
    {
        new ((void*)p) T1(value);
    }

    template <typename T>
    inline void Construct(T *p)
    {
        new ((void*)p) T();
    }

    template <typename T>
    inline void Destroy(T *pointer)
    {
        pointer->~T();
    }

    template <typename ForwardIterator>
    void DestroyAux(ForwardIterator first, ForwardIterator last, __false_type)
    {
        while (first != last)
        {
            Destroy(&*first);
            ++first;
        }
    }

    template <typename ForwardIterator>
    inline void DestroyAux(ForwardIterator first, ForwardIterator last, __true_type)
    {

    }

    template <typename ForwardIterator, typename T>
    inline void Destroy(ForwardIterator first, ForwardIterator last, T *)
    {
        typedef typename __type_traits<T>::has_trivial_destructor _Trivial_destructor;
        DestroyAux(first, last, _Trivial_destructor());
    }

    template <typename ForwardIterator>
    inline void Destroy(ForwardIterator first, ForwardIterator last)
    {
        Destroy(first, last, __value_type(first));
    }

    inline void Destroy(char*, char*) {}
    inline void Destroy(int*, int*) {}
    inline void Destroy(long*, long*) {}
    inline void Destroy(float*, float*) {}
    inline void Destroy(double*, double*) {}
    inline void Destroy(wchar_t*, wchar_t*) {}
}


#endif  /*__T3D_CONSTRUCT_H__*/
