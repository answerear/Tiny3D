

#ifndef __T3D_TEXTURE_H__
#define __T3D_TEXTURE_H__


#include "T3DResource.h"


namespace Tiny3D
{
    class T3D_ENGINE_API Texture : public Resource
    {
    public:
        Texture(const String &name);
        virtual ~Texture();

        virtual Type getType() const;

    protected:
        virtual bool load();
        virtual void unload();
        virtual Resource *clone() const;
    };
}


#endif  /*__T3D_TEXTURE_H__*/
