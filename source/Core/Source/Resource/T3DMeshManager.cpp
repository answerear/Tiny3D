

#include "Resource/T3DMeshManager.h"


namespace Tiny3D
{
    T3D_INIT_SINGLETON(MeshManager);

    MeshManager::MeshManager()
    {

    }

    MeshManager::~MeshManager()
    {

    }

    MeshPtr MeshManager::loadMesh(const String &name)
    {
        return smart_pointer_cast<Mesh>(load(name, 0));
    }

    ResourcePtr MeshManager::create(const String &name, int32_t argc, va_list args)
    {
        return Mesh::create(name);
    }
}
