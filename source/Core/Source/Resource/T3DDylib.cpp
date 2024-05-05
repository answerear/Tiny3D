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


#include "Resource/T3DDylib.h"
#include "Kernel/T3DAgent.h"
#include "T3DErrorDef.h"


#if defined (T3D_OS_WINDOWS)
    #include <windows.h>

    typedef HINSTANCE   DYLIB_HANDLE;

    #define DYLIB_LOAD(name)            LoadLibrary(name)
    #define DYLIB_GETSYM(handle, name)  GetProcAddress((HMODULE)handle, name)
    #define DYLIB_UNLOAD(handle)        FreeLibrary((HMODULE)handle)
    #define DYLIB_ERROR()               "Unknown Error"
#elif defined (T3D_OS_LINUX) || defined (T3D_OS_OSX) || defined (T3D_OS_ANDROID) || defined (T3D_OS_IOS)
    #include <dlfcn.h>

    typedef void*       DYLIB_HANDLE;

    #define DYLIB_LOAD(name)            dlopen(name, RTLD_NOW)
    #define DYLIB_GETSYM(handle, name)  dlsym(handle, name)
    #define DYLIB_UNLOAD(handle)        dlclose(handle)
    #define DYLIB_ERROR()               dlerror()
#endif

namespace Tiny3D
{
    //--------------------------------------------------------------------------

    DylibPtr Dylib::create(const String &name)
    {
        DylibPtr dylib = new Dylib(name);
        // dylib->release();
        return dylib;
    }

    //--------------------------------------------------------------------------

    Dylib::Dylib(const String &name)
        : Resource(name)
        , mHandle(nullptr)
    {
        mUUID = UUID::generate();
    }

    //--------------------------------------------------------------------------

    Dylib::~Dylib()
    {

    }

    //--------------------------------------------------------------------------

    Resource::Type Dylib::getType() const
    {
        return Type::kDylib;
    }

    //--------------------------------------------------------------------------

    void *Dylib::getSymbol(const String &name) const
    {
        return DYLIB_GETSYM(mHandle, name.c_str());
    }

    //--------------------------------------------------------------------------

    TResult Dylib::onLoad(Archive *archive)
    {
        TResult ret = T3D_OK;

        do 
        {
            mState = State::kLoading;
            
#if defined (T3D_OS_WINDOWS)
            String name = mName + ".dll";
#elif defined (T3D_OS_LINUX) || defined (T3D_OS_ANDROID)
            String name = "lib" + mName + ".so";
#elif defined (T3D_OS_OSX) || defined (T3D_OS_IOS)
            String name = "lib" + mName + ".dylib";
#endif

            const String &pluginsPath = Agent::getInstance().getPluginsPath();
            String path = pluginsPath + Dir::getNativeSeparator() + name;
            mHandle = DYLIB_LOAD(path.c_str());

            if (mHandle == nullptr)
            {
                ret = T3D_ERR_PLG_LOAD_FAILED;
                T3D_LOG_ERROR(LOG_TAG_PLUGIN, "Load plugin failed ! Desc : %s", DYLIB_ERROR());
                mState = State::kUnloaded;
                break;
            }

            ret = Resource::onLoad(archive);
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Dylib::onUnload()
    {
        if (getState() == State::kLoaded)
        {
            DYLIB_UNLOAD(mHandle);
        }

        return Resource::onUnload();
    }

    //--------------------------------------------------------------------------

    ResourcePtr Dylib::clone() const
    {
        DylibPtr dylib = create(getName());
        dylib->cloneProperties(this);
        return dylib;
    }
}

