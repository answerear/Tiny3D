

#include "T3DD3D9Texture.h"
#include "T3DD3D9Renderer.h"

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

        bool ret = false;

        if (mTexUsage == E_TU_RENDERTARGET)
        {
            ret = createRenderTexture();
        }
        else
        {
            ret = createTexture();
        }

        return ret;
    }

    void D3D9Texture::unload()
    {
        Texture::unload();
    }

    bool D3D9Texture::loadImage(const Image &src)
    {
        return true;
    }

    bool  D3D9Texture::copyToTexture(const TexturePtr &texture, Rect *src /* = nullptr */, Rect *dst /* = nullptr */) const
    {
        return true;
    }

    ResourcePtr D3D9Texture::clone() const
    {
        TexturePtr texture = create(mName, mTexWidth, mTexHeight, mNumMipMaps, mFormat, mTexUsage, mTexType);
        texture->release();
        return texture;
    }

    bool D3D9Texture::createRenderTexture()
    {
        return true;
    }

    bool D3D9Texture::createTexture()
    {
        LPDIRECT3DDEVICE9 D3DDevice = D3D9_RENDERER.getD3DDevice();



        return true;
    }
}
