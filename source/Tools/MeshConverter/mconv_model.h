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

#ifndef __MCONV_MODEL_H__
#define __MCONV_MODEL_H__


#include "mconv_node.h"


namespace mconv
{
    class Model : public Node
    {
    public:
        Model(const String &ID)
            : Node(ID)
        {

        }

        virtual ~Model()
        {

        }

        virtual Type getNodeType() const override
        {
            return E_TYPE_MODEL;
        }

        size_t  mMeshCount;
        bool    mSharedVertex;      /// ÊÇ·ñ¹²Ïí¶¥µã»º´æ
    };
}


#endif  /*__MCONV_MODEL_H__*/
