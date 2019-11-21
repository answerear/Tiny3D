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


#ifndef __T3D_GPU_CONST_BUFFER_H__
#define __T3D_GPU_CONST_BUFFER_H__


#include "Resource/T3DResource.h"
#include "Render/T3DHardwareConstantBuffer.h"


namespace Tiny3D
{
    /**
     * @class   GPUConstBuffer
     * @brief   GPU 常量緩衝區代理類，讓常量緩衝區有資源行為，用於資源管理.
     */
    class T3D_ENGINE_API GPUConstBuffer : public Resource
    {
    public:
        /**
         * @fn  static GPUConstBufferPtr GPUConstBuffer::create(
         *      const String& name, size_t bufSize, HardwareBuffer::Usage usage,
         *      bool useSystemMemory, bool useShadowBuffer);
         * @brief   創建一個新的常量緩衝區資源對象
         * @param   name    The name.
         * @param   bufSize Size of the buffer.
         * @param   usage   The usage.
         * @param   useSystemMemory True to use system memory.
         * @param   useShadowBuffer True to use shadow buffer.
         * @returns A GPUConstBufferPtr.
         */
        static GPUConstBufferPtr create(const String& name, size_t bufSize,
            HardwareBuffer::Usage usage, bool useSystemMemory, 
            bool useShadowBuffer);

        /**
         * @fn  virtual GPUConstBuffer::~GPUConstBuffer();
         * @brief   析構函數
         */
        virtual ~GPUConstBuffer();

        /**
         * @fn  virtual Type GPUConstBuffer::getType() const override;
         * @brief   重寫 Resource::getType()
         * @returns 返回資源類型.
         */
        virtual Type getType() const override;

        /**
         * @fn  HardwareConstantBufferPtr GPUConstBuffer::getBufferImpl() const;
         * @brief   獲取具體跟平台有關的對象
         * @returns 返回具體跟平台有關的對象.
         */
        HardwareConstantBufferPtr getBufferImpl() const { return mBufferImpl; }

    protected:
        /**
         * @fn  GPUConstBuffer::GPUConstBuffer(const String& name, 
         *      size_t bufSize, HardwareBuffer::Usage usage, 
         *      bool useSystemMemory, bool useShadowBuffer);
         * @brief   構造函數
         * @param   name    資源名稱.
         * @param   bufSize 緩衝區大小.
         * @param   usage   緩衝區用法.
         * @param   useSystemMemory 是否使用系統內存.
         * @param   useShadowBuffer 是否使用影子緩衝區加速訪問.
         */
        GPUConstBuffer(const String& name, size_t bufSize,
            HardwareBuffer::Usage usage, bool useSystemMemory, 
            bool useShadowBuffer);

        /**
         * @fn  virtual TResult GPUConstBuffer::load() override;
         * @brief   重寫 Resource::load()
         * @returns 調用成功返回T3D_OK.
         */
        virtual TResult load() override;

        /**
         * @fn  virtual TResult GPUConstBuffer::unload() override;
         * @brief   重寫 Resource::unlaod()
         * @returns 調用成功返回T3D_OK.
         */
        virtual TResult unload() override;

        /**
         * @fn  virtual ResourcePtr GPUConstBuffer::clone() const override;
         * @brief   重寫 Resource::clone()
         * @returns 調用成功返回一個新資源對象.
         */
        virtual ResourcePtr clone() const override;

    protected:
        size_t                      mBufSize;
        HardwareBuffer::Usage       mUsage;
        bool                        mUseSystemMemory;
        bool                        mUseShadowBuffer;
        HardwareConstantBufferPtr   mBufferImpl;    /**< 具體實現類 */
    };
}


#endif  /*__T3D_GPU_CONST_BUFFER_H__*/
