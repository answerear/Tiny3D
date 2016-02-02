

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

        virtual TexturePtr loadTexture(const String &name, Texture::TexType texType = Texture::E_TEX_TYPE_2D, int32_t numMipMaps = -1);
        virtual TexturePtr loadTexture(const String &name, int32_t width, int32_t height, PixelFormat format, Texture::TexUsage usage, Texture::TexType texType = Texture::E_TEX_TYPE_2D, int32_t numMipMaps = -1);

    protected:
        virtual ResourcePtr create(const String &name, int32_t argc, va_list args) override;

        virtual TexturePtr create(const String &name, Texture::TexType texType) = 0;
        virtual TexturePtr create(const String &name, Texture::TexType texType, int32_t width, int32_t height, int32_t numMipMaps, PixelFormat format, Texture::TexUsage usage) = 0;
    };
}


#endif  /*__T3D_TEXTURE_MANAGER_H__*/

