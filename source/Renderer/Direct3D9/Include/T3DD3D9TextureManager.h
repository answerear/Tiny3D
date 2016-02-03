

#ifndef __T3D_D3D9_TEXTURE_MANAGER_H__
#define __T3D_D3D9_TEXTURE_MANAGER_H__


#include "T3DD3D9Prerequisites.h"


namespace Tiny3D
{
    class T3D_D3D9RENDERER_API D3D9TextureManager : public TextureManager
    {
    public:
        D3D9TextureManager();
        virtual ~D3D9TextureManager();

    protected:
        virtual TexturePtr create(const String &name, Texture::TexType texType) override;
        virtual TexturePtr create(const String &name, Texture::TexType texType, int32_t width, int32_t height, int32_t numMipMaps, PixelFormat format, Texture::TexUsage usage) override;
    };
}


#endif  /*__T3D_D3D9_TEXTURE_MANAGER_H__*/
