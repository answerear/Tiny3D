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


#include "Memory/T3DMemoryManager.h"

#if defined (T3D_OS_WINDOWS)
#include "Adapter/Windows/T3DWin32MemManager.h"
#elif defined (T3D_OS_LINUX)
#elif defined (T3D_OS_IOS)
#elif defined (T3D_OS_OSX)
#elif defined (T3D_OS_IOS)
#endif

namespace Tiny3D
{
    //--------------------------------------------------------------------------

    MemoryManager &MemoryManager::getInstance()
    {
#if defined (T3D_OS_WINDOWS)
    #if defined (T3D_DEBUG)
        static DebugMemManager instance;
    #else
        static Win32MemManager instance;
    #endif
#else
#endif
        
        return instance;
    }
    
    //--------------------------------------------------------------------------

    void *Allocator::operator new(size_t size)
    {
        return Tiny3D::MemoryManager::getInstance().allocate(size, 0, false);
    }
    
    // void *operator new(size_t size, void *ptr, const char *file, int32_t line)
    // {
    //     return ptr;
    // }
    
    void *Allocator::operator new(size_t size, const std::nothrow_t&) noexcept
    {
        return Tiny3D::MemoryManager::getInstance().allocate(size, 0, false);
    }
    
    void *Allocator::operator new[](size_t size)
    {
        return Tiny3D::MemoryManager::getInstance().allocate(size, 0, true);
    }
    
    void *Allocator::operator new[](size_t size, const std::nothrow_t&) noexcept
    {
        return Tiny3D::MemoryManager::getInstance().allocate(size, 0, true);
    }
    
    void Allocator::operator delete(void *addr)
    {
        Tiny3D::MemoryManager::getInstance().deallocate((uint8_t*)addr, 0, false);
    }
    
    void Allocator::operator delete(void *addr, const std::nothrow_t&) noexcept
    {
        Tiny3D::MemoryManager::getInstance().deallocate((uint8_t*)addr, 0, false);
    }
    
    void Allocator::operator delete(void *addr, std::size_t size)
    {
        Tiny3D::MemoryManager::getInstance().deallocate((uint8_t*)addr, 0, false);
    }
    
    void Allocator::operator delete(void *addr, size_t size, const std::nothrow_t&) noexcept
    {
        Tiny3D::MemoryManager::getInstance().deallocate((uint8_t*)addr, 0, false);
    }
    
    // void operator delete(void *addr, void *, const char *file, int32_t line) noexcept 
    // { 
    //     return;
    // }
    
    void Allocator::operator delete[](void *addr)
    {
        Tiny3D::MemoryManager::getInstance().deallocate((uint8_t*)addr, 0, true);
    }
    
    void Allocator::operator delete[](void *addr, const std::nothrow_t&) noexcept
    {
        Tiny3D::MemoryManager::getInstance().deallocate((uint8_t*)addr, 0, true);
    }
    
    void Allocator::operator delete[](void *addr, size_t size)
    {
        Tiny3D::MemoryManager::getInstance().deallocate((uint8_t*)addr, 0, true);
    }
    
    void Allocator::operator delete[](void *addr, size_t size, const std::nothrow_t&) noexcept
    {
        Tiny3D::MemoryManager::getInstance().deallocate((uint8_t*)addr, 0, true);
    }
    
    //--------------------------------------------------------------------------
}


//------------------------------------------------------------------------------

extern "C++"
{
#if 0
    void *operator new(size_t size, const char *file, int32_t line)
    {
        return Tiny3D::MemoryManager::getInstance().allocate(size, 0, false);
    }

    // void *operator new(size_t size, void *ptr, const char *file, int32_t line)
    // {
    //     return ptr;
    // }

    void *operator new(size_t size, const std::nothrow_t&, const char *file, int32_t line) noexcept
    {
        return Tiny3D::MemoryManager::getInstance().allocate(size, 0, false);
    }

    void *operator new[](size_t size, const char *file, int32_t line)
    {
        return Tiny3D::MemoryManager::getInstance().allocate(size, 0, true);
    }

    void *operator new[](size_t size, const std::nothrow_t&, const char *file, int32_t line) noexcept
    {
        return Tiny3D::MemoryManager::getInstance().allocate(size, 0, true);
    }

    void operator delete(void *addr, const char *file, int32_t line)
    {
        Tiny3D::MemoryManager::getInstance().deallocate((uint8_t*)addr, 0, false);
    }

    void operator delete(void *addr, const std::nothrow_t&, const char *file, int32_t line) noexcept
    {
        Tiny3D::MemoryManager::getInstance().deallocate((uint8_t*)addr, 0, false);
    }

    void operator delete(void *addr, std::size_t size, const char *file, int32_t line)
    {
        Tiny3D::MemoryManager::getInstance().deallocate((uint8_t*)addr, 0, false);
    }

    void operator delete(void *addr, size_t size, const std::nothrow_t&, const char *file, int32_t line) noexcept
    {
        Tiny3D::MemoryManager::getInstance().deallocate((uint8_t*)addr, 0, false);
    }

    // void operator delete(void *addr, void *, const char *file, int32_t line) noexcept 
    // { 
    //     return;
    // }

    void operator delete[](void *addr, const char *file, int32_t line)
    {
        Tiny3D::MemoryManager::getInstance().deallocate((uint8_t*)addr, 0, true);
    }

    void operator delete[](void *addr, const std::nothrow_t&, const char *file, int32_t line) noexcept
    {
        Tiny3D::MemoryManager::getInstance().deallocate((uint8_t*)addr, 0, true);
    }

    void operator delete[](void *addr, size_t size, const char *file, int32_t line)
    {
        Tiny3D::MemoryManager::getInstance().deallocate((uint8_t*)addr, 0, true);
    }

    void operator delete[](void *addr, size_t size, const std::nothrow_t&, const char *file, int32_t line) noexcept
    {
        Tiny3D::MemoryManager::getInstance().deallocate((uint8_t*)addr, 0, true);
    }
#endif
}

//------------------------------------------------------------------------------
