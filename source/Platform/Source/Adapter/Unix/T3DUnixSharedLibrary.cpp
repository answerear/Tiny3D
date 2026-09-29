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


#include "Adapter/Unix/T3DUnixSharedLibrary.h"
#include "SharedLibrary/T3DSharedLibrary.h"
#include "Console/T3DConsole.h"
#include "T3DPlatformErrorDef.h"
#include <dlfcn.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>


namespace Tiny3D
{
    namespace
    {
        // dlerror 读一次就清空，必须紧跟在失败的 dlopen / dlsym 之后调用
        String takeDlError(const char *fallback)
        {
            const char *err = ::dlerror();
            return (err != nullptr) ? String(err) : String(fallback);
        }
    }

    //--------------------------------------------------------------------------

    UnixSharedLibrary::~UnixSharedLibrary()
    {
        close();
    }

    //--------------------------------------------------------------------------

    TResult UnixSharedLibrary::open(const String &path, uint32_t flags)
    {
        close();

        int mode = (flags & kSharedLibraryLazy) ? RTLD_LAZY : RTLD_NOW;
        if (flags & kSharedLibraryLocal)
        {
            mode |= RTLD_LOCAL;
        }
        else if (flags & kSharedLibraryGlobal)
        {
            mode |= RTLD_GLOBAL;
        }

        ::dlerror();
        void *handle = ::dlopen(path.c_str(), mode);
        if (handle == nullptr)
        {
            mLastError = takeDlError("dlopen failed");
            return T3D_ERR_SHAREDLIB_OPEN;
        }

        mHandle = handle;
        mLastError.clear();
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    void UnixSharedLibrary::close()
    {
        if (mHandle != nullptr)
        {
            ::dlclose(mHandle);
            mHandle = nullptr;
        }
    }

    //--------------------------------------------------------------------------

    bool UnixSharedLibrary::isOpen() const
    {
        return mHandle != nullptr;
    }

    //--------------------------------------------------------------------------

    void *UnixSharedLibrary::getSymbol(const String &name) const
    {
        if (mHandle == nullptr)
        {
            mLastError = "Shared library is not open";
            return nullptr;
        }

        ::dlerror();
        void *sym = ::dlsym(mHandle, name.c_str());
        if (sym == nullptr)
        {
            mLastError = takeDlError("dlsym failed");
        }

        return sym;
    }

    //--------------------------------------------------------------------------

    String UnixSharedLibrary::getLastError() const
    {
        return mLastError;
    }

    //--------------------------------------------------------------------------

    THandle UnixSharedLibrary::getNativeHandle() const
    {
        return mHandle;
    }

    //--------------------------------------------------------------------------

    String UnixSharedLibrary::queryModulePath(const void *address)
    {
        Dl_info info {};
        if (::dladdr(const_cast<void *>(address), &info) == 0
            || info.dli_fname == nullptr)
        {
            return String();
        }

        char resolved[PATH_MAX];
        if (::realpath(info.dli_fname, resolved) != nullptr)
        {
            return String(resolved);
        }

        // 某些嵌入式 / 旧 bionic 对 realpath 较严；退回 dli_fname，不要直接空串
        if (Console *console = Console::getInstancePtr())
        {
            console->print(
                "SharedLibrary: realpath(\"%s\") failed (%s), using dli_fname\n",
                info.dli_fname, ::strerror(errno));
        }

        return String(info.dli_fname);
    }

    //--------------------------------------------------------------------------

    bool UnixSharedLibrary::queryAddressMapped(const void *address)
    {
        Dl_info info {};
        return ::dladdr(const_cast<void *>(address), &info) != 0
            && info.dli_fname != nullptr;
    }

    //--------------------------------------------------------------------------
}
