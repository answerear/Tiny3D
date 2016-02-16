

#ifndef __T3D_D3D9_TEXTURE_H__
#define __T3D_D3D9_TEXTURE_H__


#include "T3DD3D9Prerequisites.h"


namespace Tiny3D
{
    class T3D_D3D9RENDERER_API D3D9Texture : public Texture
    {
    public:
        static TexturePtr create(const String &name, int32_t width, int32_t height,
            int32_t numMipMaps, PixelFormat format, Texture::TexUsage texUsage, Texture::TexType texType);

        virtual ~D3D9Texture();

        LPDIRECT3DTEXTURE9 getD3DTexture()  { return mD3DTexture; }

    protected:
        D3D9Texture(const String &name, int32_t width, int32_t height,
            int32_t numMipMaps, PixelFormat format, Texture::TexUsage texUsage, Texture::TexType texType);

        virtual bool load() override;
        virtual void unload() override;
        virtual bool loadImage(const Image &src) override;
        virtual bool copyToTexture(const TexturePtr &texture, Rect *src /* = nullptr */, Rect *dst /* = nullptr */) const override;
        virtual ResourcePtr clone() const override;

        bool createTexture();
        bool createRenderTexture();

        LPDIRECT3DTEXTURE9  mD3DTexture;
    };
}


#endif  /*__T3D_D3D9_TEXTURE_H__*/
