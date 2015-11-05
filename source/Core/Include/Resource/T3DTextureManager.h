

#ifndef __T3D_TEXTURE_MANAGER_H__
#define __T3D_TEXTURE_MANAGER_H__


#include "Resource/T3DResourceManager.h"
#include "T3DPlatform.h"


namespace Tiny3D
{
    class T3D_ENGINE_API TextureManager 
        : public Singleton<TextureManager>
        , public ResourceManager
    {
    public:
        TextureManager();
        virtual ~TextureManager();
    };
}


#endif  /*__T3D_TEXTURE_MANAGER_H__*/

