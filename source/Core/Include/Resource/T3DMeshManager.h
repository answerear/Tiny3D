

#ifndef __T3D_MESH_MANAGER_H__
#define __T3D_MESH_MANAGER_H__


#include "Resource/T3DResourceManager.h"
#include "Resource/T3DMesh.h"


namespace Tiny3D
{
    class T3D_ENGINE_API MeshManager 
        : public Singleton<MeshManager>
        , public ResourceManager
    {
    public:
        MeshManager();
        virtual ~MeshManager();

        virtual MeshPtr loadMesh(const String &name);

    protected:
        virtual ResourcePtr create(const String &name, int32_t argc, va_list args) override;
    };

    #define T3D_MESH_MGR        (MeshManager::getInstance())
}


#endif  /*__T3D_MESH_MANAGER_H__*/
