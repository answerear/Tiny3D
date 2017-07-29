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

#include "mconv_vertex.h"


namespace mconv
{
    String VertexAttribute::vertexTypeStr() const
    {
        String s;
        switch (mVertexType)
        {
        case E_VT_POSITION:
            {
                s = "POSITION";
            }
            break;
        case E_VT_TEXCOORD:
            {
                s = "TEXCOORD";
            }
            break;
        case E_VT_NORMAL:
            {
                s = "NORMAL";
            }
            break;
        case E_VT_TANGENT:
            {
                s = "TANGENT";
            }
            break;
        case E_VT_BINORMAL:
            {
                s = "BINORMAL";
            }
            break;
        case E_VT_COLOR:
            {
                s = "COLOR";
            }
            break;
        case E_VT_BLEND_WEIGHT:
            {
                s = "BLEND_WEIGHT";
            }
            break;
        case E_VT_BLEND_INDEX:
            {
                s = "BLEND_INDEX";
            }
            break;
        }

        return s;
    }

    String VertexAttribute::dataTypeStr() const
    {
        String s;
        switch (mDataType)
        {
        case E_VT_FLOAT:
            {
                s = "float";
            }
            break;
        case E_VT_DOUBLE:
            {
                s = "double";
            }
            break;
        case E_VT_INT16:
            {
                s = "short";
            }
            break;
        case E_VT_INT32:
            {
                s = "int";
            }
            break;
        case E_VT_INT64:
            {
                s = "long";
            }
            break;
        }

        return s;
    }
}
