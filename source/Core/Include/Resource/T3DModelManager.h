

#ifndef __T3D_MODEL_MANAGER_H__
#define __T3D_MODEL_MANAGER_H__


#include "Resource/T3DResourceManager.h"
#include "Resource/T3DModel.h"


namespace Tiny3D
{
    class T3D_ENGINE_API ModelManager 
        : public Singleton<ModelManager>
        , public ResourceManager
    {
    public:
        ModelManager();
        virtual ~ModelManager();

        virtual ModelPtr loadModel(const String &name);

    protected:
        virtual ResourcePtr create(const String &name, int32_t argc, va_list args) override;
    };

    #define T3D_MESH_MGR        (MeshManager::getInstance())
}


#endif  /*__T3D_MESH_MANAGER_H__*/
