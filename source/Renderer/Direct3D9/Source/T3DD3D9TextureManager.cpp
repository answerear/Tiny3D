
#include "T3DD3D9TextureManager.h"
#include "T3DD3D9Texture.h"


namespace Tiny3D
{
    D3D9TextureManager::D3D9TextureManager()
    {

    }

    D3D9TextureManager::~D3D9TextureManager()
    {

    }

    TexturePtr D3D9TextureManager::create(const String &name, Texture::TexType texType)
    {
        return D3D9Texture::create(name, texType, Texture::E_TU_DEFAULT);
    }

    TexturePtr D3D9TextureManager::create(const String &name, Texture::TexType texType, int32_t width, int32_t height, int32_t numMipMaps, PixelFormat format, Texture::TexUsage usage)
    {
        return D3D9Texture::create(name, texType, Texture::E_TU_DEFAULT);
    }
}