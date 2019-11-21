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


#include "Resource/T3DGPUConstBuffer.h"
#include "Render/T3DHardwareBufferManager.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    GPUConstBufferPtr GPUConstBuffer::create(const String& name, size_t bufSize, 
        HardwareBuffer::Usage usage, bool useSystemMemory, bool useShadowBuffer)
    {
        GPUConstBufferPtr mgr = new GPUConstBuffer(name, bufSize, usage,
            useSystemMemory, useShadowBuffer);
        mgr->release();
        return mgr;
    }

    //--------------------------------------------------------------------------

    GPUConstBuffer::GPUConstBuffer(const String& name, size_t bufSize, 
        HardwareBuffer::Usage usage, bool useSystemMemory, bool useShadowBuffer)
        : Resource(name)
        , mBufSize(bufSize)
        , mUsage(usage)
        , mUseSystemMemory(useSystemMemory)
        , mUseShadowBuffer(useShadowBuffer)
    {

    }

    //--------------------------------------------------------------------------

    GPUConstBuffer::~GPUConstBuffer()
    {

    }

    //--------------------------------------------------------------------------

    Resource::Type GPUConstBuffer::getType() const
    {
        return E_RT_GPUCBUFFER;
    }

    //--------------------------------------------------------------------------

    TResult GPUConstBuffer::load()
    {
        TResult ret = T3D_OK;

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult GPUConstBuffer::unload()
    {
        TResult ret = T3D_OK;

        return ret;
    }

    //--------------------------------------------------------------------------

    ResourcePtr GPUConstBuffer::clone() const
    {
        GPUConstBufferPtr buffer = GPUConstBuffer::create(mName, mBufSize,
            mUsage, mUseSystemMemory, mUseShadowBuffer);

        if (buffer != nullptr)
        {
            buffer->load();
        }

        return buffer;
    }
}
