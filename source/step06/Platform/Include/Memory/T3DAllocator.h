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

#ifndef __T3D_ALLOCATOR_H__
#define __T3D_ALLOCATOR_H__


#include "T3DPlatformPrerequisites.h"
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>


namespace Tiny3D
{
    extern void (*__malloc_alloc_oom_handler)();

    template <typename T, typename Alloc>
    class allocator
    {
    public:
        static T *allocate(size_t n);
        static T *allocate();
        static void deallocate(T *p, size_t n);
        static void deallocate(T *p)
    };

    class __malloc_alloc
    {
    public:
        static void *allocate(size_t n)
        {
            void *result = malloc(n);
            if (0 == result)
                _oom_malloc(n);
            return result;
        }

        static void deallocate(void *p, size_t/* n*/)
        {
            free(p);
        }

        static void *reallocate(void *p, size_t/* old_size*/, size_t new_size)
        {
            void *result = realloc(p, new_size);
            if (0 == result)
                _oom_realloc(p, new_size);
            return result;
        }

        static void(*_set_malloc_handler(void (*_f)()))()
        {
            void (*__old)() = __malloc_alloc_oom_handler;
            __malloc_alloc_oom_handler = _f;
            return (__old);
        }

    private:
        static void *_oom_malloc(size_t n)
        {
            void (*my_malloc_handler)();
            void *result;
            while (1)
            {
                my_malloc_handler = __malloc_alloc_oom_handler;
                if (0 == my_malloc_handler)
                {
                    fprintf(stderr, "out of memory !\n");
                    exit(1);
                }

                (*my_malloc_handler)();
                result = malloc(n);
                if (result)
                    break;
            }

            return result;
        }

        static void *_oom_realloc(void *p, size_t n)
        {
            void(*my_malloc_handler)();
            void *result;
            while (1)
            {
                my_malloc_handler = __malloc_alloc_oom_handler;
                if (0 == my_malloc_handler)
                {
                    fprintf(stderr, "out of memory !\n");
                    exit(1);
                }

                (*my_malloc_handler)();
                result = realloc(p, n);
                if (result)
                    break;
            }

            return result;
        }
    };
}


#endif  /*__T3D_ALLOCATOR_H__*/
