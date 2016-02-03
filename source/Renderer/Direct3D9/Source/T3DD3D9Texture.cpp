

#include "T3DD3D9Texture.h"


namespace Tiny3D
{
    D3D9TexturePtr D3D9Texture::create(const String &name, TexType texType, TexUsage texUsage)
    {
        D3D9TexturePtr texture = new D3D9Texture(name, texType, texUsage);
        texture->release();
        return texture;
    }

    D3D9Texture::D3D9Texture(const String &name, TexType texType, TexUsage texUsage)
        : Texture(name, texType, texUsage)
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
        TexturePtr texture = create(mName, mTexType, mTexUsage);
        texture->release();
        return texture;
    }
}
