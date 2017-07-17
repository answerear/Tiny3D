

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

        font = Font::create(name);

        return font;
    }
}
