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

#ifndef __T3D_MEMORY_MANAGER_H__
#define __T3D_MEMORY_MANAGER_H__


#include "T3DPlatformPrerequisites.h"
#include "T3DMacro.h"
#include <new.h>

#define T3D_USE_CUSTOM_NEW

namespace Tiny3D
{
    class T3D_PLATFORM_API MemoryManager
    {
    public:
        static MemoryManager &getInstance();
        
        virtual ~MemoryManager() = default;
        
        virtual void *allocate(size_t size, uint32_t alignment, bool isArray) = 0;

        virtual void deallocate(uint8_t *addr, uint32_t alignment, bool isArray) = 0;

        virtual void exit() = 0;

    protected:
        bool mEnable {false};
    };

    struct T3D_PLATFORM_API Allocator
    {
        Allocator() = default;
    
        void *operator new(size_t size);
    
        // T3D_PLATFORM_API void *operator new(size_t size, void *ptr, const char *file, int32_t line) noexcept;
    
        void *operator new(size_t size, const std::nothrow_t &) noexcept;
    
        void *operator new[](size_t size);
    
        void *operator new[](size_t size, const std::nothrow_t &) noexcept;
    
        void operator delete(void *addr);
    
        void operator delete(void *addr, const std::nothrow_t &) noexcept;
    
        // T3D_PLATFORM_API void operator delete(void *addr, void *, const char *file, int32_t line) noexcept;
    
        void operator delete(void *addr, std::size_t size) noexcept;
    
        void operator delete(void *addr, size_t size, const std::nothrow_t &) noexcept;
    
        void operator delete[](void *addr);
    
        void operator delete[](void *addr, const std::nothrow_t &) noexcept;
    
        void operator delete[](void *addr, size_t size);
    
        void operator delete[](void *addr, size_t size, const std::nothrow_t &) noexcept;
    };

    template <typename T>
    T *New()
    {
        return static_cast<T*>(MemoryManager::getInstance().allocate(sizeof(T), 0, false));
    }

    template <typename T>
    T *NewArray(size_t n)
    {
        return static_cast<T*>(MemoryManager::getInstance().allocate(sizeof(T) * n, 0, true));
    }

    template <typename T>
    void Delete(T *ptr)
    {
        MemoryManager::getInstance().deallocate((uint8_t*)ptr, 0, false);
    }

    template <typename T>
    void DeleteArray(T *ptr)
    {
        MemoryManager::getInstance().deallocate((uint8_t*)ptr, 0, true);
    }
}

#if defined (T3D_USE_CUSTOM_NEW)

// extern "C++"
// {
    // #undef new

#if 1
    // FORCEINLINE void *operator new(size_t size)
    // {
    //     return Tiny3D::MemoryManager::getInstance().allocate(size, 0, false);
    // }
    //
    // // void *operator new(size_t size, void *ptr, const char *file, int32_t line)
    // // {
    // //     return ptr;
    // // }
    //
    // FORCEINLINE void *operator new(size_t size, const std::nothrow_t&) noexcept
    // {
    //     return Tiny3D::MemoryManager::getInstance().allocate(size, 0, false);
    // }
    //
    // FORCEINLINE void *operator new[](size_t size)
    // {
    //     return Tiny3D::MemoryManager::getInstance().allocate(size, 0, true);
    // }
    //
    // FORCEINLINE void *operator new[](size_t size, const std::nothrow_t&) noexcept
    // {
    //     return Tiny3D::MemoryManager::getInstance().allocate(size, 0, true);
    // }
    //
    // FORCEINLINE void operator delete(void *addr)
    // {
    //     Tiny3D::MemoryManager::getInstance().deallocate((uint8_t*)addr, 0, false);
    // }
    //
    // FORCEINLINE void operator delete(void *addr, const std::nothrow_t&) noexcept
    // {
    //     Tiny3D::MemoryManager::getInstance().deallocate((uint8_t*)addr, 0, false);
    // }
    //
    // FORCEINLINE void operator delete(void *addr, std::size_t size)
    // {
    //     Tiny3D::MemoryManager::getInstance().deallocate((uint8_t*)addr, 0, false);
    // }
    //
    // FORCEINLINE void operator delete(void *addr, size_t size, const std::nothrow_t&) noexcept
    // {
    //     Tiny3D::MemoryManager::getInstance().deallocate((uint8_t*)addr, 0, false);
    // }
    //
    // // void operator delete(void *addr, void *, const char *file, int32_t line) noexcept 
    // // { 
    // //     return;
    // // }
    //
    // FORCEINLINE void operator delete[](void *addr)
    // {
    //     Tiny3D::MemoryManager::getInstance().deallocate((uint8_t*)addr, 0, true);
    // }
    //
    // FORCEINLINE void operator delete[](void *addr, const std::nothrow_t&) noexcept
    // {
    //     Tiny3D::MemoryManager::getInstance().deallocate((uint8_t*)addr, 0, true);
    // }
    //
    // FORCEINLINE void operator delete[](void *addr, size_t size)
    // {
    //     Tiny3D::MemoryManager::getInstance().deallocate((uint8_t*)addr, 0, true);
    // }
    //
    // FORCEINLINE void operator delete[](void *addr, size_t size, const std::nothrow_t&) noexcept
    // {
    //     Tiny3D::MemoryManager::getInstance().deallocate((uint8_t*)addr, 0, true);
    // }
#else
    T3D_PLATFORM_API void *operator new(size_t size, const char *file, int32_t line);
    
    // T3D_PLATFORM_API void *operator new(size_t size, void *ptr, const char *file, int32_t line) noexcept;
    
    T3D_PLATFORM_API void *operator new(size_t size, const std::nothrow_t &, const char *file, int32_t line) noexcept;
    
    T3D_PLATFORM_API void *operator new[](size_t size, const char *file, int32_t line);
    
    T3D_PLATFORM_API void *operator new[](size_t size, const std::nothrow_t &, const char *file, int32_t line) noexcept;
    
    T3D_PLATFORM_API void operator delete(void *addr, const char *file, int32_t line);
    
    T3D_PLATFORM_API void operator delete(void *addr, const std::nothrow_t &, const char *file, int32_t line) noexcept;
    
    // T3D_PLATFORM_API void operator delete(void *addr, void *, const char *file, int32_t line) noexcept;
    
    T3D_PLATFORM_API void operator delete(void *addr, std::size_t size, const char *file, int32_t line) noexcept;
    
    T3D_PLATFORM_API void operator delete(void *addr, size_t size, const std::nothrow_t &, const char *file, int32_t line) noexcept;
    
    T3D_PLATFORM_API void operator delete[](void *addr, const char *file, int32_t line);
    
    T3D_PLATFORM_API void operator delete[](void *addr, const std::nothrow_t &, const char *file, int32_t line) noexcept;
    
    T3D_PLATFORM_API void operator delete[](void *addr, size_t size, const char *file, int32_t line);
    
    T3D_PLATFORM_API void operator delete[](void *addr, size_t size, const std::nothrow_t &, const char *file, int32_t line) noexcept;
#endif
    
    // #define CUSTOM_NEW new
    // #define CUSTOM_DELETE delete
// }
#else
    #define CUSTOM_NEW new
    #define CUSTOM_DELETE delete
#endif

#endif  /*__T3D_MEMORY_MANAGER_H__*/
