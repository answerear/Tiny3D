

#include "Resource/T3DFont.h"
#include "Resource/T3DMaterial.h"
#include "Resource/T3DMaterialManager.h"
#include "Resource/T3DArchive.h"
#include "Resource/T3DArchiveManager.h"
#include "T3DFontFreeType.h"


namespace Tiny3D
{
    FontPtr Font::create(const String &name, int32_t fontSize, FontType fontType)
    {
        return FontFreeType::create(name, fontSize, fontType);
    }

    Font::Font(const String &name, int32_t fontSize, FontType fontType)
        : Resource(name)
        , mFontSize(fontSize)
        , mFontType(fontType)
    {

    }

    Font::~Font()
    {

    }

    Resource::Type Font::getType() const
    {
        return Resource::E_TYPE_FONT;
    }
}
