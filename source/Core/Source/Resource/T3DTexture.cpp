

#include "Resource/T3DTexture.h"
#include "Misc/T3DImage.h"


namespace Tiny3D
{
    Texture::Texture(const String &name, int32_t numMipMaps, 
        int32_t texWidth /* = -1 */, int32_t texHeight /* = -1 */, 
        TexUsage texUsage /* = E_TU_DEFAULT */, TexType texType /* = E_TEX_TYPE_1D */, 
        PixelFormat format /* = E_PF_A8R8G8B8 */)
        : Resource(name)
        , mTexType(texType)
        , mTexUsage(texUsage)
        , mNumMipMaps(numMipMaps)
        , mTexWidth(texWidth)
        , mTexHeight(texHeight)
        , mImgWidth(texWidth)
        , mImgHeight(texHeight)
        , mFormat(format)
        , mHasAlpha(false)
    {

    }

    Texture::~Texture()
    {

    }

    Resource::Type Texture::getType() const
    {
        return E_TYPE_TEXTURE;
    }
}
