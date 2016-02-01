

#include "Resource/T3DTexture.h"
#include "Misc/T3DImage.h"


namespace Tiny3D
{
    Texture::Texture(const String &name)
        : Resource(name)
    {

    }

    Texture::~Texture()
    {

    }

    Resource::Type Texture::getType() const
    {
        return E_TYPE_TEXTURE;
    }

    bool Texture::load()
    {
        Image image;
        bool ret = image.load(mName);

        if (ret)
        {
            ret = loadImage(image);
        }
        
        return ret;
    }

    void Texture::unload()
    {
        Resource::unload();
    }
}
