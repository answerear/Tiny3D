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

    GPUConstBufferPtr GPUConstBuffer::create(const String& name)
    {
        GPUConstBufferPtr mgr = new GPUConstBuffer(name);
        mgr->release();
        return mgr;
    }

    //--------------------------------------------------------------------------

    GPUConstBuffer::GPUConstBuffer(const String& name)
        : Resource(name)
        , mBufSize(0)
        , mBuffer(nullptr)
        , mUsage(HardwareBuffer::E_HBU_DYNAMIC)
        , mUseSystemMemory(false)
        , mUseShadowBuffer(false)
        , mBufferImpl(nullptr)
    {

    }

    //--------------------------------------------------------------------------

    GPUConstBuffer::~GPUConstBuffer()
    {

    }

    //--------------------------------------------------------------------------

    Resource::Type GPUConstBuffer::getType() const
    {
        return Type::E_RT_GPUCBUFFER;
    }

    //--------------------------------------------------------------------------

    TResult GPUConstBuffer::initWithData(size_t bufSize, const void* buffer,
        HardwareBuffer::Usage usage, bool useSystemMemory, bool useShadowBuffer)
    {
        TResult ret = T3D_OK;

        do 
        {
            mBufferImpl = T3D_HARDWARE_BUFFER_MGR.createConstantBuffer(
                bufSize, buffer, usage, useSystemMemory, useShadowBuffer);
            if (mBufferImpl == nullptr)
            {
                ret = T3D_ERR_RES_LOAD_FAILED;
                break;
            }

            mBufSize = bufSize;
            mBuffer = buffer;
            mUsage = usage;
            mUseSystemMemory = useSystemMemory;
            mUseShadowBuffer = useShadowBuffer;
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult GPUConstBuffer::load()
    {
        // Do nothing here.
        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult GPUConstBuffer::unload()
    {
        TResult ret = T3D_OK;

        mBufferImpl = nullptr;

        return ret;
    }

    //--------------------------------------------------------------------------

    ResourcePtr GPUConstBuffer::clone() const
    {
        GPUConstBufferPtr buffer;

        do 
        {
            buffer = GPUConstBuffer::create(mName);

            if (buffer != nullptr)
            {
                TResult ret = buffer->initWithData(mBufSize, mBuffer, 
                    mUsage, mUseSystemMemory, mUseShadowBuffer);

                if (ret != T3D_OK)
                {
                    buffer = nullptr;
                    break;
                }
            }
        } while (0);

        return buffer;
    }
}
