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

#include "Resource/T3DDylib.h"

#if defined (T3D_OS_WINDOWS)
    #include <windows.h>
    #define DYLIB_HANDLE           HINSTANCE
    #define DYLIB_LOAD(a)          LoadLibrary(a)
    #define DYLIB_GETSYM(a, b)     GetProcAddress((HMODULE)a, b)
    #define DYLIB_UNLOAD(a)        FreeLibrary((HMODULE)a)
    #define DYLIB_ERROR()          "Unknown Error"

// struct HINSTANCE__;
// typedef struct HINSTANCE__* HINSTANCE;
#elif defined (T3D_OS_LINUX) || defined (T3D_OS_MACOSX) || defined (T3D_OS_ANDROID) || defined (T3D_OS_IOS)
    #define DYLIB_HANDLE           void*
    #define DYLIB_LOAD(a)          dlopen(a, RTLD_NOW)
    #define DYLIB_GETSYM(a, b)     dlsym(a, b)
    #define DYLIB_UNLOAD(a)        dlclose(a)
    #define DYLIB_ERROR()          dlerror()
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
        return DYLIB_GETSYM(mHandle, name.c_str());
    }

    bool Dylib::load()
    {
#if defined (T3D_OS_WINDOWS)
        String name = mName + ".dll";
#elif defined (T3D_OS_LINUX) || defined (T3D_OS_ANDROID)
        String name = mName + ".so";
#elif defined (T3D_OS_MACOSX)
        String name = mName + ".dylib";
#endif
        mHandle = DYLIB_LOAD(name.c_str());
        mIsLoaded = true;
        return true;
    }

    void Dylib::unload()
    {
        if (mIsLoaded)
        {
            DYLIB_UNLOAD(mHandle);
            mIsLoaded = false;
        }
    }

    ResourcePtr Dylib::clone() const
    {
        return Dylib::create(mName);
    }
}
