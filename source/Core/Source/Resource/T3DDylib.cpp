
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
    Dylib::Dylib(const TString &strName)
        : Resource(strName)
    {

    }

    Dylib::~Dylib()
    {

    }

    Resource::EType Dylib::getType() const
    {
        return E_TYPE_DYLIB;
    }

    void *Dylib::getSymbol(const TString &strName) const
    {
        return DYLIB_GETSYM(m_Handle, strName.c_str());
    }

    bool Dylib::load()
    {
        m_Handle = DYLIB_LOAD(m_strName.c_str());
        m_bIsLoaded = true;
        return true;
    }

    void Dylib::unload()
    {
        if (m_bIsLoaded)
        {
            DYLIB_UNLOAD(m_Handle);
            m_bIsLoaded = false;
        }
    }

    Resource *Dylib::clone() const
    {
        return new Dylib(m_strName);
    }
}
