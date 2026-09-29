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

#ifndef __T3D_WIN32_SHARED_LIBRARY_H__
#define __T3D_WIN32_SHARED_LIBRARY_H__


#include "T3DNoncopyable.h"
#include "Adapter/T3DSharedLibraryInterface.h"
#include "Memory/T3DMemory.h"


namespace Tiny3D
{
    class Win32SharedLibrary : public ISharedLibrary, public Noncopyable
    {
    public:
        Win32SharedLibrary() = default;

        ~Win32SharedLibrary() override;

        TResult open(const String &path, uint32_t flags) override;

        void close() override;

        bool isOpen() const override;

        void *getSymbol(const String &name) const override;

        String getLastError() const override;

        THandle getNativeHandle() const override;

        /**
         * @brief   GetModuleHandleEx(FROM_ADDRESS) + GetModuleFileNameW，返回 UTF-8
         * @remarks 拿到的模块没有增加引用计数，不能对它 FreeLibrary
         */
        static String queryModulePath(const void *address);

        static bool queryAddressMapped(const void *address);

    protected:
        THandle         mModule {nullptr};
        mutable String  mLastError {};
    };
}


#endif  /*__T3D_WIN32_SHARED_LIBRARY_H__*/
