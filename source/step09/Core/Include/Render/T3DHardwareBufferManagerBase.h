/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Agent)
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


#ifndef __T3D_HARDWARE_BUFFER_MANAGER_BASE_H__
#define __T3D_HARDWARE_BUFFER_MANAGER_BASE_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"
#include "Render/T3DHardwareVertexBuffer.h"
#include "Render/T3DHardwareIndexBuffer.h"
#include "Kernel/T3DPixelFormat.h"


namespace Tiny3D
{
    /**
     * @brief 硬件缓冲区管理器基类
     */
    class T3D_ENGINE_API HardwareBufferManagerBase : public Object
    {
    public:
        /**
         * @brief 析构函数
         */
        virtual ~HardwareBufferManagerBase();

        /**
         * @brief 创建顶点缓冲区
         * @param [in] vertexSize : 顶点字节代销
         * @param [in] vertexCount : 顶点数量
         * @param [in] usage : 缓冲区用法
         * @param [in] useShadowBuffer : 是否使用影子缓存
         * @return 调用成功返回一个新的硬件顶点缓冲区
         * @remarks 具体子类实现该接口创建对应的具体顶点缓冲区实例
         */
        virtual HardwareVertexBufferPtr createVertexBuffer(size_t vertexSize, 
            size_t vertexCount, HardwareBuffer::Usage usage, 
            bool useShadowBuffer) = 0;

        /**
         * @brief 创建索引缓冲区
         * @param [in] indexType : 索引类型
         * @param [in] indexCount : 索引数量
         * @param [in] usage : 缓冲区使用方式
         * @param [in] useShadowBuffer : 是否使用影子缓存
         * @return 调用成功返回一个新的硬件索引缓冲区
         * @remarks 具体子类实现该接口创建对应的具体顶点缓冲区实例
         */
        virtual HardwareIndexBufferPtr createIndexBuffer(
            HardwareIndexBuffer::Type indexType, size_t indexCount, 
            HardwareBuffer::Usage usage, bool useShadowBuffer) = 0;

        /**
         * @brief 创建像素缓冲区
         * @param [in] width : 图像宽度
         * @param [in] height : 图像高度
         * @param [in] format : 像素格式
         * @param [in] usage : 缓冲区使用方式
         * @param [in] useShadowBuffer : 是否使用影子缓存
         * @return 调用成功返回一个新的硬件像素缓冲区
         * @remarks 具体子类实现该接口创建对应的具体顶点缓冲区实例
         */
        virtual HardwarePixelBufferPtr createPixelBuffer(uint32_t width, 
            uint32_t height, PixelFormat format, HardwareBuffer::Usage usage, 
            bool useShadowBuffer) = 0;

        /**
         * @brief 创建顶点数组对象
         * @return 调用成功返回一个新的顶点数组对象
         * @remarks 具体子类实现该接口创建对应的具体顶点缓冲区实例
         */
        virtual VertexArrayPtr createVertexArray() = 0;

        /**
         * @brief 创建顶点声明对象
         * @return 返回一个渲染系统对应的顶点声明对象
         */
        virtual VertexDeclarationPtr createVertexDeclaration();

    protected:
        /**
         * @brief 构造函数
         */
        HardwareBufferManagerBase();

    protected:
        typedef std::set<HardwareVertexBufferPtr>   VertexBufferList;
        typedef VertexBufferList::iterator          VertexBufferListItr;
        typedef VertexBufferList::const_iterator    VertexBufferListConstItr;

        typedef std::set<HardwareIndexBufferPtr>    IndexBufferList;
        typedef IndexBufferList::iterator           IndexBufferListItr;
        typedef IndexBufferList::const_iterator     IndexBufferListConstItr;

        typedef std::set<HardwarePixelBufferPtr>    PixelBufferList;
        typedef PixelBufferList::iterator           PixelBufferListItr;
        typedef PixelBufferList::const_iterator     PixelBufferListConstItr;

        VertexBufferList    mVertexBuffers;     /**< 顶点缓冲区列表 */
        IndexBufferList     mIndexBuffers;      /**< 索引缓冲区列表 */
        PixelBufferList     mPixelBuffers;      /**< 像素缓冲区列表 */
    };
}


#endif  /*__T3D_HARDWARE_BUFFER_MANAGER_BASE_H__*/
