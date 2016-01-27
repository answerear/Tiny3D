
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
