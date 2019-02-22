/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2019  Answer Wong
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


#ifndef __T3D_HARDWARE_BUFFER_MANAGER_H__
#define __T3D_HARDWARE_BUFFER_MANAGER_H__


#include "Render/T3DHardwareBufferManagerBase.h"


namespace Tiny3D
{
    /**
     * @brief 硬件缓冲区管理器
     * @remarks 这个类相当于一层渲染系统代理
     */
    class T3D_ENGINE_API HardwareBufferManager
        : public HardwareBufferManagerBase
        , public Singleton<HardwareBufferManager>
    {
    public:
        /**
         * @brief 创建硬件缓冲区管理器对象
         * @param [in] impl : 对应渲染系统的具体实现对象
         */
        static HardwareBufferManagerPtr create(HardwareBufferManagerBase *impl);

        /**
         * @brief 析构函数
         */
        ~HardwareBufferManager();

        /**
         * @brief 创建顶点缓冲区
         * @remarks 继承自 HardwareBufferManagerBase
         * @see HardwareBufferManagerBase::createVertexBuffer()
         */
        virtual HardwareVertexBufferPtr createVertexBuffer(size_t vertexSize, 
            size_t vertexCount, HardwareBuffer::Usage usage, 
            bool useShadowBuffer) override;

        /**
         * @brief 创建索引缓冲区
         * @remarks 继承自 HardwareBufferManagerBase
         * @see HardwareBufferManagerBase::createIndexBuffer()
         */
        virtual HardwareIndexBufferPtr createIndexBuffer(
            HardwareIndexBuffer::Type indexType, size_t indexCount, 
            HardwareBuffer::Usage usage, bool useShadowBuffer) override;

        /**
         * @brief 创建像素缓冲区
         * @remarks 继承自 HardwareBufferManagerBase
         * @see HardwareBufferManagerBase::createPixelBuffer()
         */
        virtual HardwarePixelBufferPtr createPixelBuffer(uint32_t width, 
            uint32_t height, PixelFormat format, HardwareBuffer::Usage usage, 
            bool useShadowBuffer) override;

        /**
         * @brief 创建顶点数组对象
         * @remarks 继承自 HardwareBufferManagerBase
         * @see HardwareBufferManagerBase::createVertexArray()
         */
        virtual VertexArrayPtr createVertexArray() override;

        /**
         * @brief 创建顶点声明
         * @remarks 继承自 HardwareBufferManagerBase
         * @see HardwareBufferManagerBase::createVertexDeclaration()
         */
        virtual VertexDeclarationPtr createVertexDeclaration() override;

    protected:
        /**
         * @brief 构造函数
         * @param [in] impl : 对应渲染系统的具体实现对象
         */
        HardwareBufferManager(HardwareBufferManagerBase *impl);

    protected:
        HardwareBufferManagerBasePtr   mImpl;   /**< 具体渲染体系对应的对象 */
    };

    #define T3D_HARDWARE_BUFFER_MGR     (HardwareBufferManager::getInstance())
}


#endif  /*__T3D_HARDWARE_BUFFER_MANAGER_H__*/
