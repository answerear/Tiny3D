

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

    MaterialPtr MaterialManager::loadMaterial(const String &name, Material::MaterialType matType)
    {
        return smart_pointer_cast<Material>(ResourceManager::load(name, 1, matType));
    }

    ResourcePtr MaterialManager::create(const String &name, int32_t argc, va_list args)
    {
        MaterialPtr material;

        if (argc == 1)
        {
            Material::MaterialType matType = va_arg(args, Material::MaterialType);
            material = Material::create(name, matType);
        }

        return material;
    }
}
