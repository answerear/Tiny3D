/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2020  Answer Wong
 * For latest info, see https://github.com/answerear/Tiny3D
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

#include "Resource/T3DTexture.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------
    
    TexturePtr Texture::create(const String &name)
    {
        return new Texture(name);
    }

    //--------------------------------------------------------------------------

    Texture::Texture(const String &name)
        : Resource(name)
    {
        
    }

    //--------------------------------------------------------------------------

    Resource::Type Texture::getType() const
    {
        return Type::kTexture;
    }

    //--------------------------------------------------------------------------
    
    ResourcePtr Texture::clone() const
    {
        TexturePtr texture = create(getName());
        texture->cloneProperties(this);
        return texture;
    }
    
    //--------------------------------------------------------------------------

    void Texture::cloneProperties(const Resource *const src)
    {
        Resource::cloneProperties(src);
        const Texture *material = static_cast<const Texture*>(src);
    }

    //--------------------------------------------------------------------------
}
