

#include "T3DDylibManager.h"
#include "T3DDylib.h"


namespace Tiny3D
{
    DylibManager::DylibManager()
    {

    }

    DylibManager::~DylibManager()
    {

    }

    Resource *DylibManager::create(const TString &strName)
    {
        return new Dylib(strName);
    }
}
