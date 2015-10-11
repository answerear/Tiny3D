
#ifndef __T3D_DYLIB_H__
#define __T3D_DYLIB_H__


#include "T3DResource.h"


#if defined (T3D_OS_WINDOWS)
    #define DYLIB_HANDLE           HINSTANCE
    #define DYLIB_LOAD(a)          LoadLibrary(a)
    #define DYLIB_GETSYM(a, b)     GetProcAddress(a, b)
    #define DYLIB_UNLOAD(a)        !FreeLibrary(a)
    #define DYLIB_ERROR()          "Unknown Error"

    struct HINSTANCE__;
    typedef struct HINSTANCE__* hInstance;
#elif defined (T3D_OS_LINUX) || defined (T3D_OS_MACOSX) || defined (T3D_OS_ANDROID) || defined (T3D_OS_IOS)
    #define DYLIB_HANDLE           void*
    #define DYLIB_LOAD(a)          dlopen(a, RTLD_NOW)
    #define DYLIB_GETSYM(a, b)     dlsym(a, b)
    #define DYLIB_UNLOAD(a)        dlclose(a)
    #define DYLIB_ERROR()          dlerror()
#endif


namespace Tiny3D
{
    class T3D_ENGINE_API Dylib : public Resource
    {
    public:
        Dylib(const TString &strName);
        virtual ~Dylib();

        virtual EType getType() const;

        virtual bool load();
        virtual void unload();

        virtual void *getSymbol(const TString &strName) const;

    protected:
        DYLIB_HANDLE   m_Handle;
    };
}


#endif  /*__T3D_DYLIB_H__*/
