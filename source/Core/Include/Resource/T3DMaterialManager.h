

#ifndef __T3D_MATERIAL_MANAGER_H__
#define __T3D_MATERIAL_MANAGER_H__


#include "Resource/T3DResourceManager.h"
#include "T3DPlatform.h"
#include "Resource/T3DMaterial.h"


namespace Tiny3D
{
    class T3D_ENGINE_API MaterialManager 
        : public Singleton<MaterialManager>
        , public ResourceManager
    {
    public:
        MaterialManager();
        virtual ~MaterialManager();

        virtual MaterialPtr loadMaterial(const String &name, Material::MaterialType matType);

    protected:
        virtual ResourcePtr create(const String &name, int32_t argc, va_list args) override;
    };

    #define T3D_MATERIAL_MGR        (MaterialManager::getInstance())
}

#endif  /*__T3D_MATERIAL_MANAGER_H__*/
