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


#ifndef __T3D_GPU_CONST_BUFFER_MANAGER_H__
#define __T3D_GPU_CONST_BUFFER_MANAGER_H__


#include "Resource/T3DResourceManager.h"
#include "Resource/T3DGPUConstBuffer.h"


namespace Tiny3D
{
    /**
     * @class   GPUConstBufferManager
     * @brief   GPU 常量緩衝區管理器
     */
    class T3D_ENGINE_API GPUConstBufferManager
        : public Singleton<GPUConstBufferManager>
        , public ResourceManager
    {
    public:
        /**
         * @fn  static HardwareConstantBufferPtr create();
         * @brief   Creates a new HardwareConstantBufferPtr
         * @returns A HardwareConstantBufferPtr.
         */
        static GPUConstBufferManagerPtr create();

        /**
         * @fn  virtual ~GPUConstBufferManager();
         * @brief   Destructor
         */
        virtual ~GPUConstBufferManager();

        /**
         * @fn  virtual HardwareConstantBufferPtr loadBuffer(
         *      const String &name, size_t bufSize, 
         *      HardwareBuffer::Usage usage, bool useSystemMemory, 
         *      bool useShadowBuffer);
         * @brief   Loads a buffer
         * @param   name            The name.
         * @param   bufSize         Size of the buffer.
         * @param   usage           The usage.
         * @param   useSystemMemory True to use system memory.
         * @param   useShadowBuffer True to use shadow buffer.
         * @returns The buffer.
         */
        virtual GPUConstBufferPtr loadBuffer(const String &name,
            size_t bufSize, HardwareBuffer::Usage usage,
            bool useSystemMemory, bool useShadowBuffer);

        /**
         * @fn  virtual TResult unloadBuffer(MaterialPtr material);
         * @brief   Unload buffer
         * @param   material    The material.
         * @returns A TResult.
         */
        virtual TResult unloadBuffer(GPUConstBufferPtr material);

    protected:
        /**
         * @brief 构造函数
         */
        GPUConstBufferManager();

        /**
         * @brief 重写基类接口，实现创建材质对象
         * @see ResourcePtr Resource::create(const String &name, int32_t argc,
         *      va_list args)
         */
        virtual ResourcePtr create(const String &name, int32_t argc, 
            va_list args) override;
    };

    #define T3D_CONST_BUFFER_MGR        (GPUConstBufferManager::getInstance())
}


#endif  /*__T3D_GPU_CONST_BUFFER_MANAGER_H__*/
