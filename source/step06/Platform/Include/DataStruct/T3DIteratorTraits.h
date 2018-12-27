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

#ifndef __T3D_ITERATOR_BASE_H__
#define __T3D_ITERATOR_BASE_H__


namespace Tiny3D
{
    struct input_iterator_tag {};
    struct output_iterator_tag {};
    struct forward_iterator_tag : public input_iterator_tag {};
    struct bidirectional_iterator_tag : public forward_iterator_tag {};
    struct random_access_iterator_tag : public bidirectional_iterator_tag {};

    template <typename T, typename Distance>
    struct input_iterator
    {
        typedef input_iterator_tag  iterator_category;
        typedef T                   value_type;
        typedef Distance            difference_type;
        typedef T*                  pointer;
        typedef T&                  reference;
    };

    struct output_iterator
    {
        typedef output_iterator_tag iterator_category;
        typedef void                value_type;
        typedef void                difference_type;
        typedef void                pointer;
        typedef void                reference;
    };

    template <typename T, typename Distance>
    struct forward_iterator
    {
        typedef forward_iterator_tag    iterator_category;
        typedef T                       value_type;
        typedef Distance                difference_type;
        typedef T*                      pointer;
        typedef T&                      reference;
    };

    template <typename T, typename Distance>
    struct bidirectional_iterator
    {
        typedef bidirectional_iterator_tag  iterator_category;
        typedef T                           value_type;
        typedef Distance                    difference_type;
        typedef T*                          pointer;
        typedef T&                          reference;
    };

    template <typename T, typename Distance>
    struct random_access_iterator
    {
        typedef random_access_iterator_tag  iterator_category;
        typedef T                           value_type;
        typedef Distance                    difference_type;
        typedef T*                          pointer;
        typedef T&                          reference;
    };

    template <typename Iterator>
    struct iterator_traits
    {
        typedef typename Iterator::iterator_category    iterator_category;
        typedef typename Iterator::value_type           value_type;
        typedef typename Iterator::difference_type      difference_type;
        typedef typename Iterator::pointer              pointer;
        typedef typename Iterator::reference            reference;
    };

    template <typename T>
    struct iterator_traits<T*>
    {
        typedef random_access_iterator_tag  iterator_category;
        typedef T                           value_type;
        typedef ptrdiff_t                   difference_type;
        typedef T*                          pointer;
        typedef T&                          reference;
    };

    template <typename T>
    struct iterator_traits<const T*>
    {
        typedef random_access_iterator_tag  iterator_category;
        typedef T                           value_type;
        typedef ptrdiff_t                   difference_type;
        typedef T*                          pointer;
        typedef T&                          reference;
    };

    template <typename Iter>
    inline typename iterator_traits<Iter>::iterator_category __iterator_category(const Iter &)
    {
        typedef typename iterator_traits<Iter>::iterator_category _Category;
        return _Category();
    }

    template <typename Iter>
    inline typename iterator_traits<Iter>::difference_type* __distance_type(const Iter &)
    {
        return static_cast<typename iterator_traits<Iter>::difference_type*>(0);
    }

    template <typename Iter>
    inline typename iterator_traits<Iter>::value_type* __value_type(const Iter &)
    {
        return static_cast<typename iterator_traits<Iter>::value_type*>(0);
    }

    template <typename Iter>
    inline typename iterator_traits<Iter>::iterator_category iterator_category(const Iter &i)
    {
        return __iterator_category(i);
    }

    template <typename Iter>
    inline typename iterator_traits<Iter>::difference_type* distance_type(const Iter &i)
    {
        return __distance_type(i);
    }

    template <typename Iter>
    inline typename iterator_traits<Iter>::value_type* value_type(const Iter &i)
    {
        return __value_type(i);
    }
}


#endif  /*__T3D_ITERATOR_BASE_H__*/
