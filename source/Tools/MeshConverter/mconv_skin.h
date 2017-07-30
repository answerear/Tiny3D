/*******************************************************************************
 * This file is part of Mesh-converter (A mesh converter for Tiny3D Engine)
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

#ifndef __MCONV_SKIN_H__
#define __MCONV_SKIN_H__


#include "mconv_node.h"


namespace mconv
{
    class Skin : public Node
    {
    public:
        Skin(const String &ID)
            : Node(ID)
        {

        }

        virtual ~Skin()
        {

        }

        virtual Type getNodeType() const override
        {
            return E_TYPE_SKIN;
        }
    };

    class SkinInfo : public Node
    {
    public:
        SkinInfo(const String &ID)
            : Node(ID)
        {

        }

        virtual ~SkinInfo()
        {

        }

        virtual Type getNodeType() const override
        {
            return E_TYPE_SKIN_INFO;
        }

        Matrix4     mOffsetMatrix;
    };
}


#endif  /*__MCONV_SKIN_H__*/
