

#include "Resource/T3DMaterialManager.h"
#include "Resource/T3DMaterial.h"


namespace Tiny3D
{
    T3D_INIT_SINGLETON(MaterialManager);

    MaterialManager::MaterialManager()
    {

    }

    MaterialManager::~MaterialManager()
    {

    }

    ResourcePtr MaterialManager::create(const String &name)
    {
        return Material::create(name);
    }
}
