/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
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

#ifndef __T3D_MODEL_SERIALIZER_H__
#define __T3D_MODEL_SERIALIZER_H__


#include "T3DPrerequisitesInternal.h"
#include "T3DTypedefInternal.h"
#include "T3DPrerequisites.h"
#include "T3DTypedef.h"


namespace Tiny3D
{
    class ModelSerializer
    {
    public:
        virtual ~ModelSerializer();

        virtual bool load(MemoryDataStream &stream, ModelDataPtr model) = 0;
        virtual bool save(MemoryDataStream &stream, ModelDataPtr model) = 0;
    };
}

#endif  /*__T3D_MODEL_SERIALIZER_H__*/