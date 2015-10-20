

#ifndef __T3D_DYLIB_MANAGER_H__
#define __T3D_DYLIB_MANAGER_H__


#include "T3DSingleton.h"
#include "T3DResourceManager.h"


namespace Tiny3D
{
    class Dylib;

    class T3D_ENGINE_API DylibManager 
        : public ResourceManager
        , public Singleton<DylibManager>
    {
    public:
        DylibManager();
        virtual ~DylibManager();

    protected:
        virtual Resource *create(const TString &name);
    };
}


#endif  /*__T3D_DYLIB_MANAGER_H__*/
