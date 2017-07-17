

#include "Resource/T3DFont.h"
#include "Resource/T3DMaterial.h"
#include "Resource/T3DMaterialManager.h"

#define generic _generic    // keyword for C++/CX
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#undef generic


namespace Tiny3D
{
    FontPtr Font::create(const String &name)
    {
        FontPtr font = new Font(name);
        font->release();
        return font;
    }

    Font::Font(const String &name)
        : Resource(name)
    {

    }

    Font::~Font()
    {

    }

    Resource::Type Font::getType() const
    {
        return Resource::E_TYPE_FONT;
    }

    bool Font::load()
    {
        return true;
    }

    void Font::unload()
    {

    }

    ResourcePtr Font::clone() const
    {
        return Font::create(mName);
    }
}
