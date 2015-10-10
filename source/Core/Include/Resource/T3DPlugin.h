
#ifndef __T3D_PLUGIN_H__
#define __T3D_PLUGIN_H__


#include "T3DResource.h"


#if defined (T3D_OS_WINDOWS)
    #define PLUGIN_HANDLE           HINSTANCE
    #define PLUGIN_LOAD(a)          LoadLibrary(a)
    #define PLUGIN_GETSYM(a, b)     GetProcAddress(a, #b)
    #define PLUGIN_UNLOAD(a)        !FreeLibrary(a)
    #define PLUGIN_ERROR()          "Unknown Error"

    struct HINSTANCE__;
    typedef struct HINSTANCE__* hInstance;
#elif defined (T3D_OS_LINUX) || defined (T3D_OS_MACOSX) || defined (T3D_OS_ANDROID)
    #define PLUGIN_HANDLE           void*
    #define PLUGIN_LOAD(a)          dlopen(a, RTLD_NOW)
    #define PLUGIN_GETSYM(a, b)     dlsym(a, #b)
    #define PLUGIN_UNLOAD(a)        dlclose(a)
    #define PLUGIN_ERROR()          dlerror()
#elif defined (T3D_OS_IOS)
    #define PLUGIN_HANDLE           (void)0
    #define PLUGIN_LOAD(a)          (void)a
    #define PLUGIN_GETSYM(a, b)     b
    #define PLUGIN_UNLOAD(a)        (void)a
    #define PLUGIN_ERROR()          "Unknown Error"
#endif


namespace Tiny3D
{
    class T3D_ENGINE_API Plugin : public Resource
    {
    public:
        Plugin(const TString &strName);
        virtual ~Plugin();

        virtual bool load();
        virtual void unload();

        virtual void *getSymbol(const TString &strName) const;

    protected:
        PLUGIN_HANDLE   m_Handle;
    };
}


#endif  /*__T3D_PLUGIN_H__*/
