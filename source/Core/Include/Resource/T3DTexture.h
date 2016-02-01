

#ifndef __T3D_TEXTURE_H__
#define __T3D_TEXTURE_H__


#include "T3DResource.h"


namespace Tiny3D
{
    class T3D_ENGINE_API Texture : public Resource
    {
    public:
        enum TexType
        {
            E_TEX_TYPE_1D = 1,
            E_TEX_TYPE_2D,
            E_TEX_TYPE_3D,
            E_TEX_TYPE_CUBE_MAP,
        };

        enum TexUsage
        {
            E_TU_DEFAULT = 0,
            E_TU_RENDERTARGET,
        };

        virtual ~Texture();

        virtual Type getType() const override;

    protected:
        Texture(const String &name);

        virtual bool load() override;
        virtual void unload() override;

        virtual bool loadImage(const Image &src) = 0;
    };
}


#endif  /*__T3D_TEXTURE_H__*/
