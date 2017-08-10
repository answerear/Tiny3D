/***************************************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Answer Wong
 * For latest info, see https://github.com/asnwerear/Tiny3D
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **************************************************************************************************/

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

    FontPtr FontManager::loadFont(int32_t fontSize, const String &name /* = "" */, 
        Font::FontType fontType /* = Font::E_FT_TRUETYPE */)
    {
        if (name.empty())
        {
            return smart_pointer_cast<Font>(ResourceManager::load(mDefaultFontName, 2, fontSize, fontType));
        }

        return smart_pointer_cast<Font>(ResourceManager::load(name, 2, fontSize, fontType));
    }

    ResourcePtr FontManager::create(const String &name, int32_t argc, va_list args)
    {
        FontPtr font;

        int32_t fontSize = va_arg(args, int32_t);
        Font::FontType fontType = va_arg(args, Font::FontType);
        font = Font::create(name, fontSize, fontType);

        return font;
    }

    void FontManager::unloadFont(FontPtr &font)
    {
        ResourceManager::unload((ResourcePtr &)font);
    }
}
