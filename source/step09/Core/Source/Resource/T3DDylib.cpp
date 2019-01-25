/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2019  Answer Wong
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


#include "Resource/T3DDylib.h"
#include "Kernel/T3DEngine.h"
#include "T3DErrorDef.h"


#if defined (T3D_OS_WINDOWS)
    #include <windows.h>

    typedef HINSTANCE   DYLIB_HANDLE;

    #define DYLIB_LOAD(name)            LoadLibrary(name)
    #define DYLIB_GETSYM(handle, name)  GetProcAddress((HMODULE)handle, name)
    #define DYLIB_UNLOAD(handle)        FreeLibrary((HMODULE)handle)
    #define DYLIB_ERROR()               "Unknown Error"
#elif defined (T3D_OS_LINUX) || defined (T3D_OS_OSX) || defined (T3D_OS_ANDROID)
    #include <dlfcn.h>

    typedef void*       DYLIB_HANDLE;

    #define DYLIB_LOAD(name)            dlopen(name, RTLD_NOW)
    #define DYLIB_GETSYM(handle, name)  dlsym(handle, name)
    #define DYLIB_UNLOAD(handle)        dlclose(handle)
    #define DYLIB_ERROR()               dlerror()
#elif defined (T3D_OS_IOS)
    #include <dlfcn.h>

    typedef void*       DYLIB_HANDLE;

    #define DYLIB_LOAD(name)            dlopen(NULL, RTLD_NOW)
    #define DYLIB_GETSYM(handle, name)  dlsym(handle, name)
    #define DYLIB_UNLOAD(handle)        dlclose(handle)
    #define DYLIB_ERROR()               dlerror()
#endif

namespace Tiny3D
{
    DylibPtr Dylib::create(const String &name)
    {
        DylibPtr dylib = new Dylib(name);
        dylib->release();
        return dylib;
    }

    Dylib::Dylib(const String &name)
        : Resource(name)
    {

    }

    Dylib::~Dylib()
    {

    }

    Resource::Type Dylib::getType() const
    {
        return E_TYPE_DYLIB;
    }

    void *Dylib::getSymbol(const String &name) const
    {
        // 为了 iOS 的静态库也能做成类似插件形式，避免重名函数在静态库中出现导致
        // 重复符号编译错误，这里特意加上插件名称
        String symbol = mName + "_" + name;
        return DYLIB_GETSYM(mHandle, symbol.c_str());
    }

    TResult Dylib::load()
    {
        TResult ret = T3D_OK;

        do 
        {
#if defined (T3D_OS_WINDOWS)
            String name = mName + ".dll";
#elif defined (T3D_OS_LINUX) || defined (T3D_OS_ANDROID)
            String name = "lib" + mName + ".so";
#elif defined (T3D_OS_OSX) || defined (T3D_OS_IOS)
            String name = "lib" + mName + ".dylib";
#endif

            String pluginsPath = Engine::getInstance().getPluginsPath();
            String path = Engine::getInstance().getAppPath() + pluginsPath 
                + Dir::NATIVE_SEPARATOR + name;
            mHandle = DYLIB_LOAD(path.c_str());

            if (mHandle == nullptr)
            {
                ret = T3D_ERR_PLUGIN_LOAD_FAILED;
                T3D_LOG_ERROR(LOG_TAG_PLUGIN, "Load plugin failed ! Desc : %s",
                    DYLIB_ERROR());
                break;
            }

            mIsLoaded = true;
        } while (0);

        return ret;
    }

    TResult Dylib::unload()
    {
        if (mIsLoaded)
        {
            DYLIB_UNLOAD(mHandle);
            mIsLoaded = false;
        }

        return T3D_OK;
    }

    ResourcePtr Dylib::clone() const
    {
        return Dylib::create(getName());
    }
}

