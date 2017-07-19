

#ifndef __T3D_FONT_H__
#define __T3D_FONT_H__


#include "T3DResource.h"


namespace Tiny3D
{
    class T3D_ENGINE_API Font : public Resource
    {
    public:
        enum FontType
        {
            E_FT_TRUETYPE = 1,
            E_FT_IMAGE = 2,
        };

        static FontPtr create(const String &name, FontType eFontType);

        virtual ~Font();

        virtual Type getType() const override;

    protected:
        Font(const String &name);

        virtual bool load() override;
        virtual void unload() override;
        virtual ResourcePtr clone() const override;

        bool loadFreeType(DataStream &stream);
    };
}


#endif  /*__T3D_FONT_H__*/
