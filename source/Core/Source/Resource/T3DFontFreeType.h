
#ifndef __T3D_FONT_FREE_TYPE_H__
#define __T3D_FONT_FREE_TYPE_H__


#include "Resource/T3DFont.h"
#include "T3DPrerequisitesInternal.h"
#include "T3DTypedefInternal.h"
#include "Misc/T3DObject.h"
#include "Misc/T3DSmartPtr.h"

#define generic _generic    // keyword for C++/CX
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#undef generic


namespace Tiny3D
{
    class FontFreeType : public Font
    {
    public:
        static FontFreeTypePtr create(const String &name, int32_t fontSize, FontType fontType);

        virtual ~FontFreeType();

        virtual bool updateContent(const String &text, MaterialPtr &material, CharSet &set) override;

        FT_Face getFontFace() const { return mFTFace; }

    protected:
        FontFreeType(const String &name, int32_t fontSize, FontType fontType);

        virtual bool load() override;
        virtual void unload() override;
        virtual ResourcePtr clone() const override;

        bool loadFreeType(DataStream &stream);

    protected:
        FT_Library      mFTLibrary;
        FT_Face         mFTFace;
    };
}


#endif  /*__T3D_FONT_FREE_TYPE_H__*/
