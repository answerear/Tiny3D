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

#include "T3DSubMeshData.h"

namespace Tiny3D
{
    SubMeshDataPtr SubMeshData::create(const String &name, const String &materialName, Renderer::PrimitiveType priType, bool is16Bits, size_t indexCount)
    {
        SubMeshDataPtr data = new SubMeshData(name, materialName, priType, is16Bits, indexCount);

        if (data != nullptr)
        {
            data->release();
        }

        return data;
    }

    SubMeshData::SubMeshData(const String &name, const String &materialName, Renderer::PrimitiveType priType, bool is16Bits, size_t indexCount)
        : mName(name)
        , mMaterialName(materialName)
        , mPrimitiveType(priType)
        , mIs16Bits(is16Bits)
        , mIndices(is16Bits ? indexCount * sizeof(uint16_t) : indexCount * sizeof(uint32_t))
    {

    }
}
