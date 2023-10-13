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


#include "Render/T3DConstantBuffer.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    ConstantBufferPtr ConstantBuffer::create(const Buffer &buffer, MemoryType memType, Usage usage, uint32_t accMode)
    {
        return new ConstantBuffer(buffer, memType, usage, accMode);
    }

    //--------------------------------------------------------------------------

    ConstantBuffer::ConstantBuffer(const Buffer &buffer, MemoryType memType, Usage usage, uint32_t accMode)
        : RenderBuffer(buffer, memType, usage, accMode)
    {
        
    }

    //--------------------------------------------------------------------------

    RenderResource::Type ConstantBuffer::getType() const
    {
        return Type::kConstantBuffer;
    }

    //--------------------------------------------------------------------------

    bool ConstantBuffer::onLoad()
    {
        return true;
    }

    //--------------------------------------------------------------------------

    bool ConstantBuffer::onUnload()
    {
        return true;
    }

    //--------------------------------------------------------------------------
}
