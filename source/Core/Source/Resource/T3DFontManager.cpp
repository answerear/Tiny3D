

#include "Resource/T3DFontManager.h"
#include "Resource/T3DFreeTypeAtlas.h"


namespace Tiny3D
{
    T3D_INIT_SINGLETON(FontManager);

    FontManager::FontManager(const FontConfig &config)
        : mFontConfig(config)
    {
        FreeTypeAtlas *atlas = new FreeTypeAtlas();
    }

    FontManager::~FontManager()
    {
        FreeTypeAtlas *atlas = FreeTypeAtlas::getInstancePtr();
        delete atlas;
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
