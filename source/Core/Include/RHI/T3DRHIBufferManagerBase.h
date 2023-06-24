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


#ifndef __T3D_RHI_BUFFER_MANAGER_BASE_H__
#define __T3D_RHI_BUFFER_MANAGER_BASE_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"
#include "RHI/T3DRHIVertexBuffer.h"
#include "RHI/T3DRHIIndexBuffer.h"
#include "Kernel/T3DConstant.h"


namespace Tiny3D
{
    /**
     * @brief   硬件缓冲区管理器基类
     */
    class T3D_ENGINE_API RHIBufferManagerBase : public Object
    {
    public:
        /**
         * @brief   析构函数
         */
        virtual ~RHIBufferManagerBase();

        /**
         * @brief   创建顶点缓冲区
         * @param [in]  vertexSize  顶点字节大小.
         * @param [in]  vertexCount 顶点数量.
         * @param [in]  vertices    顶点数据，可以为nullptr.
         * @param [in]  usage       缓冲区用法.
         * @param [in]  mode        缓冲区访问方式.
         * @returns 调用成功返回一个新的硬件顶点缓冲区.
         * @remarks 具体子类实现该接口创建对应的具体顶点缓冲区实例.
         */
        virtual RHIVertexBufferPtr createVertexBuffer(size_t vertexSize, 
            size_t vertexCount, const void *vertices,
            RHIBuffer::Usage usage, uint32_t mode) = 0;

        /**
         * @fn  virtual RHIIndexBufferPtr 
         *      HardwareBufferManagerBase::createIndexBuffer(
         *      RHIIndexBuffer::Type indexType, size_t indexCount, 
         *      const void *indices, RHIBuffer::Usage usage, 
         *      uint32_t mode) = 0;
         * @brief   创建索引缓冲区
         * @param [in]  indexType   索引类型.
         * @param [in]  indexCount  索引数量.
         * @param [in]  indices     索引数据，可以为nullptr.
         * @param [in]  usage       缓冲区使用方式.
         * @param [in]  mode        缓冲区访问方式.
         * @returns 调用成功返回一个新的硬件索引缓冲区.
         * @remarks 具体子类实现该接口创建对应的具体顶点缓冲区实例.
         */
        virtual RHIIndexBufferPtr createIndexBuffer(
            RHIIndexBuffer::Type indexType, size_t indexCount, 
            const void *indices, RHIBuffer::Usage usage, 
            uint32_t mode) = 0;

        /**
         * @fn  virtual RHIPixelBufferPtr 
         *      HardwareBufferManagerBase::createPixelBuffer(size_t width, 
         *      size_t height, PixelFormat format, const void *pixels, 
         *      RHIBuffer::Usage usage, uint32_t mode) = 0;
         * @brief   创建像素缓冲区
         * @param [in]  width   图像宽度.
         * @param [in]  height  图像高度.
         * @param [in]  format  像素格式.
         * @param [in]  pixels  像素数据，可以为nullptr.
         * @param [in]  usage   缓冲区使用方式.
         * @param [in]  mode    缓冲区访问方式.
         * @returns 调用成功返回一个新的硬件像素缓冲区.
         * @remarks 具体子类实现该接口创建对应的具体顶点缓冲区实例.
         */
        virtual RHIPixelBufferPtr createPixelBuffer(size_t width, 
            size_t height, PixelFormat format, const void *pixels,
            RHIBuffer::Usage usage, uint32_t mode, size_t mipmaps) = 0;

        /**
         * @fn  virtual HardwareConstantBufferPtr 
         *      HardwareBufferManagerBase::createConstantBuffer(size_t bufSize, 
         *      const void *buffer, RHIBuffer::Usage usage, 
         *      uint32_t mode) = 0;
         * @brief   创建常量缓冲区
         * @param [in]  bufSize 缓冲区大小.
         * @param [in]  buffer  缓冲区数据.
         * @param [in]  usage   缓冲区使用方式.
         * @param [in]  mode    缓冲区访问方式.
         * @returns 调用成功返回一个新的硬件常量缓冲区.
         * @remarks 具体子类实现该接口创建对应的具体常量缓冲区实例.
         */
        virtual RHIDataBufferPtr createDataBuffer(
            size_t bufSize, const void *buffer, RHIBuffer::Usage usage, uint32_t mode) = 0;

        /**
         * @brief   创建顶点声明对象
         * @returns 返回一个渲染系统对应的顶点声明对象.
         */
        virtual RHIVertexDeclarationPtr createVertexDeclaration();

    protected:
        /**
         * @fn  HardwareBufferManagerBase::HardwareBufferManagerBase();
         * @brief   构造函数
         */
        RHIBufferManagerBase();

    protected:
        typedef std::set<RHIVertexBufferPtr>        VertexBufferList;
        typedef VertexBufferList::iterator          VertexBufferListItr;
        typedef VertexBufferList::const_iterator    VertexBufferListConstItr;

        typedef std::set<RHIIndexBufferPtr>         IndexBufferList;
        typedef IndexBufferList::iterator           IndexBufferListItr;
        typedef IndexBufferList::const_iterator     IndexBufferListConstItr;

        typedef std::set<RHIPixelBufferPtr>         PixelBufferList;
        typedef PixelBufferList::iterator           PixelBufferListItr;
        typedef PixelBufferList::const_iterator     PixelBufferListConstItr;

        VertexBufferList    mVertexBuffers;     /**< 顶点缓冲区列表 */
        IndexBufferList     mIndexBuffers;      /**< 索引缓冲区列表 */
        PixelBufferList     mPixelBuffers;      /**< 像素缓冲区列表 */
    };
}


#endif  /*__T3D_RHI_BUFFER_MANAGER_BASE_H__*/
