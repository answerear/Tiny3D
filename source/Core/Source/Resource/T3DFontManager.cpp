

#include "Resource/T3DFontManager.h"


namespace Tiny3D
{
    T3D_INIT_SINGLETON(FontManager);

    FontManager::FontManager()
    {

    }

    FontManager::~FontManager()
    {

    }

    FontPtr FontManager::loadFont(const String &name)
    {
        return smart_pointer_cast<Font>(ResourceManager::load(name, 0));
    }

    ResourcePtr FontManager::create(const String &name, int32_t argc, va_list args)
    {
        FontPtr font;

        int32_t fontSize = va_arg(args, int32_t);
        Font::FontType fontType = va_arg(args, Font::FontType);
        font = Font::create(name, fontSize, fontType);

        return font;
    }
}
