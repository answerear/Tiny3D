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

#ifndef __T3D_TYPE_TRAITS_H__
#define __T3D_TYPE_TRAITS_H__


namespace Tiny3D
{
    struct __true_type {};
    struct __false_type {};

    template <typename T>
    struct __type_traits
    {
        typedef __true_type     this_dummy_member_must_be_first;
        typedef __false_type    has_trivial_default_constructor;
        typedef __false_type    has_trivial_copy_constructor;
        typedef __false_type    has_trivial_assignment_operator;
        typedef __false_type    has_trivial_destructor;
        typedef __false_type    is_POD_type;
    };

    template <>
    struct __type_traits<bool>
    {
        typedef __true_type     has_trivial_default_constructor;
        typedef __true_type     has_trivial_copy_constructor;
        typedef __true_type     has_trivial_assignment_operator;
        typedef __true_type     has_trivial_destructor;
        typedef __true_type     is_POD_type;
    };

    template <>
    struct __type_traits<char>
    {
        typedef __true_type     has_trivial_default_constructor;
        typedef __true_type     has_trivial_copy_constructor;
        typedef __true_type     has_trivial_assignment_operator;
        typedef __true_type     has_trivial_destructor;
        typedef __true_type     is_POD_type;
    };

    template <>
    struct __type_traits<signed char>
    {
        typedef __true_type     has_trivial_default_constructor;
        typedef __true_type     has_trivial_copy_constructor;
        typedef __true_type     has_trivial_assignment_operator;
        typedef __true_type     has_trivial_destructor;
        typedef __true_type     is_POD_type;
    };

    template <>
    struct __type_traits<unsigned char>
    {
        typedef __true_type     has_trivial_default_constructor;
        typedef __true_type     has_trivial_copy_constructor;
        typedef __true_type     has_trivial_assignment_operator;
        typedef __true_type     has_trivial_destructor;
        typedef __true_type     is_POD_type;
    };

    template <>
    struct __type_traits<wchar_t>
    {
        typedef __true_type     has_trivial_default_constructor;
        typedef __true_type     has_trivial_copy_constructor;
        typedef __true_type     has_trivial_assignment_operator;
        typedef __true_type     has_trivial_destructor;
        typedef __true_type     is_POD_type;
    };

    template <>
    struct __type_traits<short>
    {
        typedef __true_type     has_trivial_default_constructor;
        typedef __true_type     has_trivial_copy_constructor;
        typedef __true_type     has_trivial_assignment_operator;
        typedef __true_type     has_trivial_destructor;
        typedef __true_type     is_POD_type;
    };

    template <>
    struct __type_traits<unsigned short>
    {
        typedef __true_type     has_trivial_default_constructor;
        typedef __true_type     has_trivial_copy_constructor;
        typedef __true_type     has_trivial_assignment_operator;
        typedef __true_type     has_trivial_destructor;
        typedef __true_type     is_POD_type;
    };

    template <>
    struct __type_traits<int>
    {
        typedef __true_type     has_trivial_default_constructor;
        typedef __true_type     has_trivial_copy_constructor;
        typedef __true_type     has_trivial_assignment_operator;
        typedef __true_type     has_trivial_destructor;
        typedef __true_type     is_POD_type;
    };

    template <>
    struct __type_traits<unsigned int>
    {
        typedef __true_type     has_trivial_default_constructor;
        typedef __true_type     has_trivial_copy_constructor;
        typedef __true_type     has_trivial_assignment_operator;
        typedef __true_type     has_trivial_destructor;
        typedef __true_type     is_POD_type;
    };

    template <>
    struct __type_traits<long>
    {
        typedef __true_type     has_trivial_default_constructor;
        typedef __true_type     has_trivial_copy_constructor;
        typedef __true_type     has_trivial_assignment_operator;
        typedef __true_type     has_trivial_destructor;
        typedef __true_type     is_POD_type;
    };

    template <>
    struct __type_traits<unsigned long>
    {
        typedef __true_type     has_trivial_default_constructor;
        typedef __true_type     has_trivial_copy_constructor;
        typedef __true_type     has_trivial_assignment_operator;
        typedef __true_type     has_trivial_destructor;
        typedef __true_type     is_POD_type;
    };

    template <>
    struct __type_traits<long long>
    {
        typedef __true_type     has_trivial_default_constructor;
        typedef __true_type     has_trivial_copy_constructor;
        typedef __true_type     has_trivial_assignment_operator;
        typedef __true_type     has_trivial_destructor;
        typedef __true_type     is_POD_type;
    };

    template <>
    struct __type_traits<unsigned long long>
    {
        typedef __true_type     has_trivial_default_constructor;
        typedef __true_type     has_trivial_copy_constructor;
        typedef __true_type     has_trivial_assignment_operator;
        typedef __true_type     has_trivial_destructor;
        typedef __true_type     is_POD_type;
    };

    template <>
    struct __type_traits<float>
    {
        typedef __true_type     has_trivial_default_constructor;
        typedef __true_type     has_trivial_copy_constructor;
        typedef __true_type     has_trivial_assignment_operator;
        typedef __true_type     has_trivial_destructor;
        typedef __true_type     is_POD_type;
    };

    template <>
    struct __type_traits<double>
    {
        typedef __true_type     has_trivial_default_constructor;
        typedef __true_type     has_trivial_copy_constructor;
        typedef __true_type     has_trivial_assignment_operator;
        typedef __true_type     has_trivial_destructor;
        typedef __true_type     is_POD_type;
    };

    template <>
    struct __type_traits<long double>
    {
        typedef __true_type     has_trivial_default_constructor;
        typedef __true_type     has_trivial_copy_constructor;
        typedef __true_type     has_trivial_assignment_operator;
        typedef __true_type     has_trivial_destructor;
        typedef __true_type     is_POD_type;
    };

    template <typename T>
    struct __type_traits<T*>
    {
        typedef __true_type     has_trivial_default_constructor;
        typedef __true_type     has_trivial_copy_constructor;
        typedef __true_type     has_trivial_assignment_operator;
        typedef __true_type     has_trivial_destructor;
        typedef __true_type     is_POD_type;
    };

    template <>
    struct __type_traits<char*>
    {
        typedef __true_type     has_trivial_default_constructor;
        typedef __true_type     has_trivial_copy_constructor;
        typedef __true_type     has_trivial_assignment_operator;
        typedef __true_type     has_trivial_destructor;
        typedef __true_type     is_POD_type;
    };

    template <>
    struct __type_traits<signed char*>
    {
        typedef __true_type     has_trivial_default_constructor;
        typedef __true_type     has_trivial_copy_constructor;
        typedef __true_type     has_trivial_assignment_operator;
        typedef __true_type     has_trivial_destructor;
        typedef __true_type     is_POD_type;
    };

    template <>
    struct __type_traits<unsigned char*>
    {
        typedef __true_type     has_trivial_default_constructor;
        typedef __true_type     has_trivial_copy_constructor;
        typedef __true_type     has_trivial_assignment_operator;
        typedef __true_type     has_trivial_destructor;
        typedef __true_type     is_POD_type;
    };

    template <>
    struct __type_traits<const char*>
    {
        typedef __true_type     has_trivial_default_constructor;
        typedef __true_type     has_trivial_copy_constructor;
        typedef __true_type     has_trivial_assignment_operator;
        typedef __true_type     has_trivial_destructor;
        typedef __true_type     is_POD_type;
    };

    template <>
    struct __type_traits<const signed char*>
    {
        typedef __true_type     has_trivial_default_constructor;
        typedef __true_type     has_trivial_copy_constructor;
        typedef __true_type     has_trivial_assignment_operator;
        typedef __true_type     has_trivial_destructor;
        typedef __true_type     is_POD_type;
    };

    template <>
    struct __type_traits<const unsigned char*>
    {
        typedef __true_type     has_trivial_default_constructor;
        typedef __true_type     has_trivial_copy_constructor;
        typedef __true_type     has_trivial_assignment_operator;
        typedef __true_type     has_trivial_destructor;
        typedef __true_type     is_POD_type;
    };

    template <typename T>
    struct _Is_Integer
    {
        typedef __false_type    _Integral;
    };

    template <>
    struct _Is_Integer<bool>
    {
        typedef __true_type     _Integral;
    };

    template <>
    struct _Is_Integer<char>
    {
        typedef __true_type     _Integral;
    };

    template <>
    struct _Is_Integer<signed char>
    {
        typedef __true_type     _Integral;
    };

    template <>
    struct _Is_Integer<unsigned char>
    {
        typedef __true_type     _Integral;
    };

    template <>
    struct _Is_Integer<wchar_t>
    {
        typedef __true_type     _Integral;
    };

    template <>
    struct _Is_Integer<short>
    {
        typedef __true_type     _Integral;
    };

    template <>
    struct _Is_Integer<unsigned short>
    {
        typedef __true_type     _Integral;
    };

    template <>
    struct _Is_Integer<int>
    {
        typedef __true_type     _Integral;
    };

    template <>
    struct _Is_Integer<unsigned int>
    {
        typedef __true_type     _Integral;
    };

    template <>
    struct _Is_Integer<long>
    {
        typedef __true_type     _Integral;
    };

    template <>
    struct _Is_Integer<unsigned long>
    {
        typedef __true_type     _Integral;
    };

    template <>
    struct _Is_Integer<long long>
    {
        typedef __true_type     _Integral;
    };

    template <>
    struct _Is_Integer<unsigned long long>
    {
        typedef __true_type     _Integral;
    };
}

#endif  /*__T3D_TYPE_TRAITS_H__*/
