

#include "Resource/T3DTexture.h"


namespace Tiny3D
{
    Texture::Texture(const String &name)
        : Resource(name)
    {

    }

    Texture::~Texture()
    {

    }

    Resource::EType Texture::getType() const
    {
        return Resource::E_TYPE_TEXTURE;
    }

    bool Texture::load()
    {
        return true;
    }

    void Texture::unload()
    {
        Resource::unload();
    }

    Resource *Texture::clone() const
    {
        return new Texture(mName);
    }
}
