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


#include "SharedLibrary/T3DSharedLibrary.h"
#include "T3DPlatformErrorDef.h"
#include "T3DPlatform.h"
#include "IO/T3DDir.h"
#include "Adapter/T3DFactoryInterface.h"
#include "Adapter/T3DPlatformInterface.h"
#include "Adapter/T3DSharedLibraryInterface.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    SharedLibrary::SharedLibrary()
    {
        mLib = T3D_PLATFORM_FACTORY.createPlatformSharedLibrary();
    }

    //--------------------------------------------------------------------------

    SharedLibrary::~SharedLibrary()
    {
        close();
        T3D_SAFE_DELETE(mLib);
    }

    //--------------------------------------------------------------------------

    TResult SharedLibrary::open(const String &path, uint32_t flags)
    {
        mLastError.clear();

        if (mLib == nullptr)
        {
            mLastError = "Shared library implementation is not created";
            return T3D_ERR_IMPLEMENT_NOT_CREATED;
        }

        const uint32_t binding = kSharedLibraryNow | kSharedLibraryLazy;
        const uint32_t visibility = kSharedLibraryLocal | kSharedLibraryGlobal;
        const uint32_t known = binding | visibility;

        if ((flags & ~known) != 0
            || (flags & binding) == binding
            || (flags & visibility) == visibility)
        {
            mLastError = "Invalid shared library flags : " + std::to_string(flags);
            return T3D_ERR_SHAREDLIB_FLAGS;
        }

        return mLib->open(path, flags);
    }

    //--------------------------------------------------------------------------

    void SharedLibrary::close()
    {
        if (mLib != nullptr)
        {
            mLib->close();
        }
    }

    //--------------------------------------------------------------------------

    bool SharedLibrary::isOpen() const
    {
        return mLib != nullptr && mLib->isOpen();
    }

    //--------------------------------------------------------------------------

    void *SharedLibrary::getSymbol(const String &name) const
    {
        if (mLib != nullptr)
        {
            return mLib->getSymbol(name);
        }

        return nullptr;
    }

    //--------------------------------------------------------------------------

    String SharedLibrary::getLastError() const
    {
        if (!mLastError.empty() || mLib == nullptr)
        {
            return mLastError;
        }

        return mLib->getLastError();
    }

    //--------------------------------------------------------------------------

    THandle SharedLibrary::getNativeHandle() const
    {
        if (mLib != nullptr)
        {
            return mLib->getNativeHandle();
        }

        return nullptr;
    }

    //--------------------------------------------------------------------------

    String SharedLibrary::makeFileName(const String &logicalName)
    {
#if defined (T3D_OS_WINDOWS)
        return logicalName + ".dll";
#elif defined (T3D_OS_OSX) || defined (T3D_OS_IOS)
        return "lib" + logicalName + ".dylib";
#else
        return "lib" + logicalName + ".so";
#endif
    }

    //--------------------------------------------------------------------------

    String SharedLibrary::getModulePath(const void *address)
    {
        Platform *platform = Platform::getInstancePtr();
        if (address == nullptr || platform == nullptr
            || platform->getPlatformImpl() == nullptr)
        {
            return String();
        }

        return platform->getPlatformImpl()->getModulePath(address);
    }

    //--------------------------------------------------------------------------

    String SharedLibrary::getModuleDir(const void *address)
    {
        const String path = getModulePath(address);
        if (path.empty())
        {
            return String();
        }

        String dir, name;
        Dir::parsePath(path, dir, name);
        return dir;
    }

    //--------------------------------------------------------------------------

    bool SharedLibrary::isAddressMapped(const void *address)
    {
        Platform *platform = Platform::getInstancePtr();
        if (address == nullptr || platform == nullptr
            || platform->getPlatformImpl() == nullptr)
        {
            return false;
        }

        return platform->getPlatformImpl()->isAddressMapped(address);
    }

    //--------------------------------------------------------------------------
}
