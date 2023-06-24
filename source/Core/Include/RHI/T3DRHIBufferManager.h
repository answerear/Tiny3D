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


#ifndef __T3D_RHI_BUFFER_MANAGER_H__
#define __T3D_RHI_BUFFER_MANAGER_H__


#include "RHI/T3DRHIBufferManagerBase.h"


namespace Tiny3D
{
    /**
     * @brief   硬件缓冲区管理器
     * @remarks  这个类相当于一层渲染系统代理.
     */
    class T3D_ENGINE_API RHIBufferManager
        : public RHIBufferManagerBase
        , public Singleton<RHIBufferManager>
    {
    public:
        /**
         * @fn  static RHIBufferManagerPtr create(
         *      RHIBufferManagerBase *impl);
         * @brief   创建硬件缓冲区管理器对象
         * @param [in]  impl    对应渲染系统的具体实现对象.
         * @returns 返回新建的硬件缓冲区管理器对象.
         */
        static RHIBufferManagerPtr create(RHIBufferManagerBase *impl);

        /**
         * @fn  ~RHIBufferManager();
         * @brief   析构函数
         */
        ~RHIBufferManager();

        /**
         * @fn  virtual RHIVertexBufferPtr createVertexBuffer(
         *      size_t vertexSize, size_t vertexCount, const void *vertices, 
         *      RHIBuffer::Usage usage, uint32_t mode) override;
         * @brief   创建顶点缓冲区
         * @param [in]  vertexSize  顶点字节大小.
         * @param [in]  vertexCount 顶点数量.
         * @param [in]  vertices    顶点数据，可以为nullptr.
         * @param [in]  usage       缓冲区用法.
         * @param [in]  mode        缓冲区访问方式.
         * @returns 调用成功返回一个新的硬件顶点缓冲区.
         * @remarks  继承自 RHIBufferManagerBase.
         * @sa  HardwaerVertexBufferPtr 
         *      RHIBufferManagerBase::createVertexBuffer(
         *      size_t vertexSize, size_t vertexCount, const void *vertices,
         *      RHIBuffer::Usage usage, uint32_t mode)
         */
        virtual RHIVertexBufferPtr createVertexBuffer(size_t vertexSize,
            size_t vertexCount, const void *vertices,
            RHIBuffer::Usage usage, uint32_t mode) override;

        /**
         * @fn  virtual RHIIndexBufferPtr createIndexBuffer(
         *      RHIIndexBuffer::Type indexType, size_t indexCount, 
         *      const void *indices, RHIBuffer::Usage usage, 
         *      uint32_t mode) override;
         * @brief   创建索引缓冲区
         * @param [in]  indexType   索引类型.
         * @param [in]  indexCount  索引数量.
         * @param [in]  indices     索引数据，可以为nullptr.
         * @param [in]  usage       缓冲区使用方式.
         * @param [in]  mode        缓冲区访问方式.
         * @returns 调用成功返回一个新的硬件索引缓冲区.
         * @remarks  继承自 RHIBufferManagerBase.
         * @sa  RHIIndexBufferPtr 
         *      RHIBufferManagerBase::createIndexBuffer(
         *      RHIIndexBuffer::Type indexType, size_t indexCount,
         *      const void *indices, RHIBuffer::Usage usage, uint32_t mode)
         */
        virtual RHIIndexBufferPtr createIndexBuffer(
            RHIIndexBuffer::Type indexType, size_t indexCount,
            const void *indices, RHIBuffer::Usage usage,
            uint32_t mode) override;

        /**
         * @fn  virtual RHIPixelBufferPtr createPixelBuffer(size_t width, 
         *      size_t height, PixelFormat format, const void *pixels, 
         *      RHIBuffer::Usage usage, uint32_t mode) override;
         * @brief   创建像素缓冲区
         * @param [in]  width   图像宽度.
         * @param [in]  height  图像高度.
         * @param [in]  format  像素格式.
         * @param [in]  pixels  像素数据，可以为nullptr.
         * @param [in]  usage   缓冲区使用方式.
         * @param [in]  mode    缓冲区访问方式.
         * @returns 调用成功返回一个新的硬件像素缓冲区.
         * @remarks 继承自 RHIBufferManagerBase.
         * @sa  RHIPixelBufferPtr 
         *      RHIBufferManagerBase::createPixelBuffer(
         *      size_t width, size_t height, PixelFormat format, 
         *      const void *pixels, RHIBuffer::Usage usage, uint32_t mode)
         */
        virtual RHIPixelBufferPtr createPixelBuffer(size_t width,
            size_t height, PixelFormat format, const void *pixels,
            RHIBuffer::Usage usage, uint32_t mode, size_t mipmaps) override;

        /**
         * @fn  virtual RHIDataBufferPtr createConstantBuffer(
         *      size_t bufSize, const void *buffer, RHIBuffer::Usage usage, 
         *      uint32_t mode) override;
         * @brief   创建常量缓冲区
         * @param [in]  bufSize 缓冲区大小.
         * @param [in]  buffer  缓冲区数据.
         * @param [in]  usage   缓冲区使用方式.
         * @param [in]  mode    缓冲区访问方式.
         * @returns 调用成功返回一个新的硬件常量缓冲区.
         * @remarks 继承自 RHIBufferManagerBase.
         * @sa  RHIDataBufferPtr 
         *      RHIBufferManagerBase::createConstantBuffer(size_t bufSize, 
         *      const void *buffer, RHIBuffer::Usage usage, uint32_t mode)
         */
        virtual RHIDataBufferPtr createDataBuffer(
            size_t bufSize, const void *buffer, RHIBuffer::Usage usage,
            uint32_t mode) override;

        /**
         * @brief   创建顶点声明
         * @returns 返回一个渲染系统对应的顶点声明对象.
         * @remarks 继承自 RHIBufferManagerBase.
         */
        virtual RHIVertexDeclarationPtr createVertexDeclaration() override;

    protected:
        /**
         * @fn  RHIBufferManager(RHIBufferManagerBase *impl);
         * @brief   构造函数
         * @param [in]  impl    对应渲染系统的具体实现对象.
         */
        RHIBufferManager(RHIBufferManagerBase *impl);

    protected:
        RHIBufferManagerBasePtr   mImpl;   /**< 具体渲染体系对应的对象 */
    };

    #define T3D_RHI_BUFFER_MGR     (RHIBufferManager::getInstance())
}


#endif  /*__T3D_RHI_BUFFER_MANAGER_H__*/
