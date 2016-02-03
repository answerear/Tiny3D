

#include "Resource/T3DTexture.h"
#include "Misc/T3DImage.h"


namespace Tiny3D
{
    Texture::Texture(const String &name, TexType texType, TexUsage texUsage)
        : Resource(name)
        , mTexType(texType)
        , mTexUsage(texUsage)
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
