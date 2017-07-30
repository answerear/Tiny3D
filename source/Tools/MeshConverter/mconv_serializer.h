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

#ifndef __MCONV_SERIALIZER_H__
#define __MCONV_SERIALIZER_H__


#include "mconv_prerequisites.h"


namespace mconv
{
    class Serializer
    {
    public:
        virtual ~Serializer()   {}

        virtual bool load(const String &path, void *&pData) = 0;
        virtual bool save(const String &path, void *pData) = 0;
    };
}


#endif  /*__MCONV_SERIALIZER_H__*/
