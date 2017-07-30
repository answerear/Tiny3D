/*******************************************************************************
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
 ******************************************************************************/

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
