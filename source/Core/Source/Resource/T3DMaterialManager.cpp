

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

    MaterialPtr MaterialManager::loadMaterial(const String &name)
    {
        return smart_pointer_cast<Material>(ResourceManager::load(name, 0));
    }

    ResourcePtr MaterialManager::create(const String &name, int32_t argc, va_list args)
    {
        return Material::create(name);
    }
}
