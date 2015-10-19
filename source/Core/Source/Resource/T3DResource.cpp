

#include "Resource/T3DResource.h"


namespace Tiny3D
{
    Resource::Resource(const TString &strName)
        : m_unSize(0)
        , m_bIsLoaded(false)
        , m_strName(strName)
    {

    }

    Resource::~Resource()
    {
        if (m_bIsLoaded)
            unload();
    }

    void Resource::unload()
    {

    }
}
