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

#include "Adapter/OSX/T3DOSXMemManager.h"
#include <stdlib.h>


namespace Tiny3D
{
    void *OSXMemManager::allocate(size_t size, uint32_t alignment, bool isArray)
    {
        (void)isArray;

        if (size == 0)
        {
            size = 1;
        }

        if (alignment <= 1)
        {
            return malloc(size);
        }

        size_t align = alignment;
        if (align < sizeof(void *))
        {
            align = sizeof(void *);
        }

        void *ptr = nullptr;
        if (posix_memalign(&ptr, align, size) != 0)
        {
            return nullptr;
        }

        return ptr;
    }

    void OSXMemManager::deallocate(uint8_t *addr, uint32_t alignment, bool isArray)
    {
        (void)alignment;
        (void)isArray;
        free(addr);
    }

    void OSXMemManager::exit()
    {
    }
}
