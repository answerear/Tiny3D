/*******************************************************************************
 * This file is part of Mesh-converter (A mesh converter for Tiny3D Engine)
 * Copyright (C) 2015-2017  Aaron Wong
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

#ifndef __MCONV_TEXTURE_H__
#define __MCONV_TEXTURE_H__


#include "mconv_node.h"


namespace mconv
{
    class Texture : public Node
    {
    public:
        Texture(const String &ID)
            : Node(ID)
            , mFilename("")
            , mType("diffuse")
            , mWrapModeU("repeat")
            , mWrapModeV("repeat")
        {

        }

        virtual ~Texture()
        {

        }

        virtual Type getNodeType() const override
        {
            return E_TYPE_TEXTURE;
        }

        String  mFilename;
        String  mType;
        String  mWrapModeU;
        String  mWrapModeV;
    };

    class Textures : public Node
    {
    public:
        Textures(const String &ID)
            : Node(ID)
        {

        }

        virtual ~Textures()
        {

        }

        virtual Type getNodeType() const override
        {
            return E_TYPE_TEXTURES;
        }
    };
}


#endif  /*__MCONV_TEXTURE_H__*/
