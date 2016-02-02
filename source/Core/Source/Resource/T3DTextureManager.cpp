

#include "Resource/T3DTextureManager.h"


namespace Tiny3D
{
    T3D_INIT_SINGLETON(TextureManager);

    TextureManager::TextureManager()
    {

    }

    TextureManager::~TextureManager()
    {

    }

    TexturePtr TextureManager::loadTexture(const String &name, Texture::TexType texType /* = Texture::E_TEX_TYPE_2D */, int32_t numMipMaps /* = -1 */)
    {
        return smart_pointer_cast<Texture>(ResourceManager::load(name, 2, texType, numMipMaps));
    }

    TexturePtr TextureManager::loadTexture(const String &name, int32_t width, int32_t height, PixelFormat format, Texture::TexUsage usage, Texture::TexType texType /* = Texture::E_TEX_TYPE_2D */, int32_t numMipMaps /* = -1 */)
    {
        return smart_pointer_cast<Texture>(ResourceManager::load(name, 6, width, height, format, usage, texType, numMipMaps));
    }

    ResourcePtr TextureManager::create(const String &name, int32_t argc, va_list args)
    {
        ResourcePtr res;

        if (argc == 2)
        {
            Texture::TexType texType = va_arg(args, Texture::TexType);
            int32_t numMipMaps = va_arg(args, int32_t);
            res = create(name, texType);
        }
        else if (argc == 6)
        {
            int32_t width = va_arg(args, int32_t);
            int32_t height = va_arg(args, int32_t);
            PixelFormat format = va_arg(args, PixelFormat);
            Texture::TexUsage usage = va_arg(args, Texture::TexUsage);
            Texture::TexType texType = va_arg(args, Texture::TexType);
            int32_t numMipMaps = va_arg(args, int32_t);
            res = create(name, texType, width, height, numMipMaps, format, usage);
        }

        return res;
    }
}
