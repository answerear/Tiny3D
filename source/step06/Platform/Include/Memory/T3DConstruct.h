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


#include "T3DPlatformPrerequisites.h"
#include <new.h>

namespace Tiny3D
{
    template <typename T1, typename T2>
    inline void _Construct(T1 *p, const T2 &value)
    {
        new ((void *)p) T1(value);
    }

    template <typename T>
    inline void _Construct(T *p)
    {
        new ((void *)p) T();
    }

    template <typename T>
    inline void _Destroy(T *pointer)
    {
        pointer->~T();
    }

    template <typename ForwardIterator>
    inline void __Destroy_aux(ForwardIterator first, ForwardIterator last, __false_type)
    {
        for (; first != last; ++first)
            _Destroy(&*first);
    }

    template <typename ForwardIterator>
    inline void __Destroy_aux(ForwardIterator, ForwardIterator last, __true_type)
    {

    }

    template <typename ForwardIterator, typename T>
    inline void __Destroy(ForwardIterator first, ForwardIterator last, T *)
    {
        typedef typename __type_traits<T>::has_trivial_destructor _Trivial_destructor;
        __Destroy_aux(first, last, _Trivial_destructor(first));
    }

    template <typename ForwardIterator>
    inline void __Destroy(ForwardIterator first, ForwardIterator last)
    {

    }
}


#endif  /*__T3D_CONSTRUCT_H__*/
