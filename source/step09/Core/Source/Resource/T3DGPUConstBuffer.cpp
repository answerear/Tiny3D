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
        , mUsage(HardwareBuffer::Usage::STATIC)
        , mAccessMode(0)
        , mHasData(false)
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
        HardwareBuffer::Usage usage, uint32_t mode)
    {
        TResult ret = T3D_OK;

        do 
        {
            if (mHasData)
            {
                ret = T3D_ERR_RES_ALREADY_INIT;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "GPU constant buffer has alreay initialized with data !");
                break;
            }

            if ((bufSize & 0xF) != 0)
            {
                // GPU常量緩衝需要128位(16字節)對齊
                ret = T3D_ERR_RES_INVALID_PARAM;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "GPU constant buffer must be 128 bytes alignment !");
                break;
            }

            if (bufSize != mBufSize * 4)
            {
                // 輸入的大小應該跟數據類型定義的大小匹配
                ret = T3D_ERR_RES_INVALID_PARAM;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Input data size must equal the size of data type declaration !");
                break;
            }

            mBufferImpl = T3D_HARDWARE_BUFFER_MGR.createConstantBuffer(bufSize, 
                buffer, usage, mode);
            if (mBufferImpl == nullptr)
            {
                ret = T3D_ERR_RES_LOAD_FAILED;
                break;
            }

            mBufSize = bufSize;
            mBuffer = buffer;
            mUsage = usage;
            mAccessMode = mode;
            mHasData = true;
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
        mHasData = false;
        mBufSize = 0;
        mBuffer = nullptr;

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
                TResult ret = buffer->initWithData(mBufSize, mBuffer,  mUsage, 
                    mAccessMode);

                if (ret != T3D_OK)
                {
                    buffer = nullptr;
                    break;
                }
            }
        } while (0);

        return buffer;
    }

    //--------------------------------------------------------------------------

    TResult GPUConstBuffer::addDataDeclaration(BuiltinType type, uint8_t count)
    {
        TResult ret = T3D_OK;

        do 
        {
            if (mHasData)
            {
                // 已經有數據了，不能再添加數據類型聲明
                ret = T3D_ERR_RES_ALREADY_INIT;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "GPU constant buffer has already initialized with data !");
                break;

            }

            DataDeclaration decl;
            decl.type = type;
            decl.count = count;
            mDataDeclList.push_back(decl);

            mBufSize += count;
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult GPUConstBuffer::removeDataDeclaration(size_t index)
    {
        TResult ret = T3D_OK;

        do 
        {
            bool found = false;
            size_t i = 0;

            for (auto itr = mDataDeclList.begin(); 
                itr != mDataDeclList.end(); 
                ++itr)
            {
                if (i == index)
                {
                    found = true;
                    mDataDeclList.erase(itr);
                    break;
                }

                ++i;
            }

            if (!found)
            {
                ret = T3D_ERR_NOT_FOUND;
                break;
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    GPUConstBuffer::DataDeclaration GPUConstBuffer::getDataDeclaration(
        size_t index) const
    {
        DataDeclaration decl;

        do 
        {
            bool found = false;
            size_t i = 0;

            for (auto itr = mDataDeclList.begin(); 
                itr != mDataDeclList.end(); 
                ++itr)
            {
                if (i == index)
                {
                    found = true;
                    decl = *itr;
                    break;
                }

                ++i;
            }
        } while (0);
        
        return decl;
    }
}
