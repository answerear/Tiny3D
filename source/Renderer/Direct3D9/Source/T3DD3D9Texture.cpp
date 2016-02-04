

#include "T3DD3D9Texture.h"


namespace Tiny3D
{
    TexturePtr D3D9Texture::create(const String &name, int32_t width, int32_t height,
        int32_t numMipMaps, PixelFormat format, Texture::TexUsage texUsage, Texture::TexType texType)
    {
        TexturePtr texture = new D3D9Texture(name, width, height, numMipMaps, format, texUsage, texType);
        texture->release();
        return texture;
    }

    D3D9Texture::D3D9Texture(const String &name, int32_t width, int32_t height,
        int32_t numMipMaps, PixelFormat format, Texture::TexUsage texUsage, Texture::TexType texType)
        : Texture(name, width, height, numMipMaps, texUsage, texType, format)
        , mD3DTexture(nullptr)
    {

    }

    D3D9Texture::~D3D9Texture()
    {
        D3D_SAFE_RELEASE(mD3DTexture);
    }

    bool D3D9Texture::load()
    {
        if (isLoaded())
            return true;

        if (mTexUsage == E_TU_RENDERTARGET)
        {

        }
        else
        {
            Image image;
            if (image.load(mName))
            {

            }
        }

        return true;
    }

    void D3D9Texture::unload()
    {
        Texture::unload();
    }

    bool D3D9Texture::loadImage(const Image &src)
    {
        return true;
    }

    ResourcePtr D3D9Texture::clone() const
    {
        TexturePtr texture = create(mName, mTexWidth, mTexHeight, mNumMipMaps, mFormat, mTexUsage, mTexType);
        texture->release();
        return texture;
    }
}
