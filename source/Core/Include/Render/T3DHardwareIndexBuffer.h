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

#ifndef __T3D_HARDWARE_INDEX_BUFFER_H__
#define __T3D_HARDWARE_INDEX_BUFFER_H__


#include "Render/T3DHardwareBuffer.h"


namespace Tiny3D
{
    class T3D_ENGINE_API HardwareIndexBuffer : public HardwareBuffer
    {
    public:
        enum Type
        {
            E_IT_16BITS = 0,
            E_IT_32BITS,
        };

        virtual ~HardwareIndexBuffer();

        Type getIndexType() const       { return mType; }
        size_t getIndexSize() const     { return mIndexSize; }
        size_t getIndexCount() const    { return mIndexCount; }

    protected:
        HardwareIndexBuffer(Type indexType, size_t indexCount, HardwareBuffer::Usage usage, bool useSystemMemory, bool useShadowBuffer);

        Type    mType;
        size_t  mIndexSize;
        size_t  mIndexCount;
    };
}


#endif  /*__T3D_HARDWARE_INDEX_BUFFER_H__*/
