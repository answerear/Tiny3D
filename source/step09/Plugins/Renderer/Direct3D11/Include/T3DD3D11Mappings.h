/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2019  Answer Wong
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


#ifndef __T3D_D3D11_MAPPINGS_H__
#define __T3D_D3D11_MAPPINGS_H__


#include "T3DD3D11Prerequisites.h"


namespace Tiny3D
{
    /**
     * @brief Tiny3D 和 DirectX 11 之间常量的映射转换
     */
    class D3D11Mappings
    {
    public:
        static const char * const POSITION;
        static const char * const BLENDWEIGHT;
        static const char * const BLENDINDICES;
        static const char * const NORMAL;
        static const char * const COLOR;
        static const char * const TEXCOORD;
        static const char * const TANGENT;
        static const char * const BINORMAL;

        static const char *get(VertexAttribute::Semantic semantic);
        static DXGI_FORMAT get(VertexAttribute::Type type);
    };
}


#endif  /*__T3D_D3D11_MAPPINGS_H__*/