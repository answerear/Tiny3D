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


#include "Render/T3DIndexBuffer.h"
#include "Kernel/T3DAgent.h"
#include "RHI/T3DRHIContext.h"
#include "RHI/T3DRHIIndexBuffer.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    IndexBufferPtr IndexBuffer::create(IndexType idxType, size_t idxCount, const Buffer &buffer, MemoryType memType, Usage usage, uint32_t accMode)
    {
        return new IndexBuffer(idxType, idxCount, buffer, memType, usage, accMode);
    }

    //--------------------------------------------------------------------------

    IndexBuffer::IndexBuffer(IndexType idxType, size_t idxCount, const Buffer &buffer, MemoryType memType, Usage usage, uint32_t accMode)
        : RenderBuffer(buffer, memType, usage, accMode)
    {
        
    }

    //--------------------------------------------------------------------------

    RenderResource::Type IndexBuffer::getType() const
    {
        return Type::kIndexBuffer;
    }

    //--------------------------------------------------------------------------

    bool IndexBuffer::onLoad()
    {
        mRHIResource = T3D_AGENT.getActiveRHIContext()->createIndexBuffer(this);
        return true;
    }

    //--------------------------------------------------------------------------

    bool IndexBuffer::onUnload()
    {
        mRHIResource = nullptr;
        return true;
    }

    //--------------------------------------------------------------------------
}
