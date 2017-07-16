

#include "T3DD3D9Texture.h"
#include "T3DD3D9Renderer.h"
#include "T3DD3D9Mappings.h"


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
        : Texture(name, numMipMaps, width, height, texUsage, texType, format)
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

    bool D3D9Texture::loadImage(const Image &image)
    {
        LPDIRECT3DDEVICE9 D3DDevice = D3D9_RENDERER.getD3DDevice();
        bool ret = false;
        HRESULT hr;

        mImgWidth = image.getWidth();
        mImgHeight = image.getHeight();

        D3DLOCKED_RECT rect;

        hr = mD3DTexture->LockRect(0, &rect, nullptr, 0);
        if (SUCCEEDED(hr))
        {
            ret = image.copyToScaling(rect.pBits, mTexWidth, mTexHeight, mFormat, rect.Pitch, false);
            hr = mD3DTexture->UnlockRect(0);
        }

        return ret;
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
        bool ret = false;

        HRESULT hr;
        
        LPDIRECT3DDEVICE9 D3DDevice = D3D9_RENDERER.getD3DDevice();

        if (mTexUsage == E_TU_DEFAULT)
        {
            Image image;
            ret = image.load(mName);

            mFormat = image.getFormat();
            D3DFORMAT format = D3D9Mappings::get(mFormat);
            mImgWidth = image.getWidth();
            mImgHeight = image.getHeight();

            if (SUCCEEDED(hr = D3DXCreateTexture(D3DDevice, mImgWidth, mImgHeight, mNumMipMaps, 0, format, D3DPOOL_MANAGED, &mD3DTexture)))
            {
                ret = true;

                D3DSURFACE_DESC desc;
                hr = mD3DTexture->GetLevelDesc(0, &desc);

                if (SUCCEEDED(hr))
                {
                    mTexWidth = desc.Width;
                    mTexHeight = desc.Height;
                    mFormat = D3D9Mappings::get(desc.Format);
                }
            }
            else
            {

            }

            if (ret)
            {
                ret = loadImage(image);
            }
        }
        else
        {
            D3DFORMAT format = D3D9Mappings::get(mFormat);

            if (SUCCEEDED(hr = D3DXCreateTexture(D3DDevice, mTexWidth, mTexHeight, mNumMipMaps, 0, format, D3DPOOL_DEFAULT, &mD3DTexture)))
            {
                ret = true;
            }
            else
            {

            }
        }

        return ret;
    }
}
