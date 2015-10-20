

#include "Resource/T3DDylibManager.h"
#include "Resource/T3DDylib.h"


namespace Tiny3D
{
    T3D_INIT_SINGLETON(DylibManager);

    DylibManager::DylibManager()
    {

    }

    DylibManager::~DylibManager()
    {

    }

    Resource *DylibManager::create(const TString &name)
    {
        return new Dylib(name);
    }
}
