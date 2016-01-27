

#ifndef __T3D_TEXTURE_H__
#define __T3D_TEXTURE_H__


#include "T3DResource.h"


namespace Tiny3D
{
    class T3D_ENGINE_API Texture : public Resource
    {
    public:
        virtual ~Texture();

        virtual Type getType() const override;

    protected:
        Texture(const String &name);

        virtual bool load() override;
        virtual void unload() override;
    };
}


#endif  /*__T3D_TEXTURE_H__*/
