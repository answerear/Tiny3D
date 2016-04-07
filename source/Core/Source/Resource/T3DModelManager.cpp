

#include "Resource/T3DModelManager.h"


namespace Tiny3D
{
    T3D_INIT_SINGLETON(ModelManager);

    ModelManager::ModelManager()
    {

    }

    ModelManager::~ModelManager()
    {

    }

    ModelPtr ModelManager::loadModel(const String &name)
    {
        return smart_pointer_cast<Model>(load(name, 0));
    }

    ResourcePtr ModelManager::create(const String &name, int32_t argc, va_list args)
    {
        return Model::create(name);
    }
}
