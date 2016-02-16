

#ifndef __T3D_TEXTURE_MANAGER_H__
#define __T3D_TEXTURE_MANAGER_H__


#include "Resource/T3DResourceManager.h"
#include "Resource/T3DTexture.h"
#include "Misc/T3DCommon.h"


namespace Tiny3D
{
    class T3D_ENGINE_API TextureManager 
        : public Singleton<TextureManager>
        , public ResourceManager
    {
    public:
        TextureManager();
        virtual ~TextureManager();

        virtual TexturePtr loadTexture(const String &name, int32_t numMipMaps = -1, Texture::TexUsage texUsage = Texture::E_TU_DEFAULT, Texture::TexType texType = Texture::E_TEX_TYPE_2D);
        virtual TexturePtr loadTexture(const String &name, int32_t width, int32_t height, int32_t numMipMaps = -1, PixelFormat format = E_PF_A8R8G8B8, Texture::TexUsage texUsage = Texture::E_TU_BLANK, Texture::TexType texType = Texture::E_TEX_TYPE_2D);

    protected:
        virtual ResourcePtr create(const String &name, int32_t argc, va_list args) override;

        virtual TexturePtr createTexture(const String &name, int32_t width, int32_t height, int32_t numMipMaps, PixelFormat format, Texture::TexUsage texUsage, Texture::TexType texType) = 0;

        int32_t mDefaultNumMipMaps;
    };

    #define T3D_TEXTURE_MGR         (TextureManager::getInstance())
}


#endif  /*__T3D_TEXTURE_MANAGER_H__*/

