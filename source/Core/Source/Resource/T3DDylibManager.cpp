

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

    DylibPtr DylibManager::loadDylib(const String &name)
    {
        return smart_pointer_cast<Dylib>(ResourceManager::load(name, 0));
    }

    ResourcePtr DylibManager::create(const String &name, int32_t argc, va_list args)
    {
        return Dylib::create(name);
    }
}
