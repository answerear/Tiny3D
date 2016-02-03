

#ifndef __T3D_D3D9_TEXTURE_H__
#define __T3D_D3D9_TEXTURE_H__


#include "T3DD3D9Prerequisites.h"


namespace Tiny3D
{
    class T3D_D3D9RENDERER_API D3D9Texture : public Texture
    {
    public:
        static D3D9TexturePtr create(const String &name, TexType texType, TexUsage texUsage);

        virtual ~D3D9Texture();

    protected:
        D3D9Texture(const String &name, TexType texType, TexUsage texUsage);

        virtual bool load() override;
        virtual void unload() override;
        virtual bool loadImage(const Image &src) override;
        virtual ResourcePtr clone() const override;

        LPDIRECT3DTEXTURE9  mD3DTexture;
    };
}


#endif  /*__T3D_D3D9_TEXTURE_H__*/
