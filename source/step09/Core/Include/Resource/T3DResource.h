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


#ifndef __T3D_RESOURCE_H__
#define __T3D_RESOURCE_H__


#include "Kernel/T3DObject.h"
#include "T3DTypedef.h"


namespace Tiny3D
{
    class T3D_ENGINE_API Resource : public Object
    {
        friend class ResourceManager;

    public:
        /** 资源类型枚举 */
        enum Type
        {
            E_RT_UNKNOWN = 0,
            E_RT_DYLIB,         /**< 动态库 */
            E_RT_ARCHIVE,       /**< 文件档案系统 */
            E_RT_MATERIAL,      /**< 材质 */
            E_RT_TEXTURE,       /**< 纹理 */
        };

        /** 析构函数 */
        virtual ~Resource();

        /** 获取资源类型 */
        virtual Type getType() const = 0;

        /** 获取资源唯一ID */
        ID getID() const
        {
            return mID;
        }
        
        /** 获取克隆资源唯一ID，当该资源是从其他资源克隆出来时，该ID才有效 */
        ID getCloneID() const
        {
            return mCloneID;
        }

        /** 是否克隆资源 */
        bool isCloned() const
        {
            return (mCloneID != T3D_INVALID_ID);
        }

        /** 获取资源大小 */
        size_t getSize() const
        {
            return mSize;
        }

        /** 获取资源名称 */
        const String &getName() const
        {
            return mName;
        }

        /** 获取资源是否加载 */
        bool isLoaded() const
        {
            return mIsLoaded;
        }

    protected:
        /** 
         * @brief 构造函数
         * @remarks 本类不能直接实例化，所以只能隐藏构造函数 
         */
        Resource(const String &name);

        /** 
         * @brief 加载资源
         * @remarks 每种类型资源需要各自实现其加载逻辑，资源只有加载后才能使用
         */
        virtual TResult load() = 0;

        /** 
         * @brief 卸载资源
         * @remarks 每种类型资源需要各自实现其卸载逻辑，资源卸载后就不能再使用了
         */
        virtual TResult unload();

        /** 
         * @brief 克隆资源
         * @remarks 每种类型资源需要各自实现其克隆逻辑，克隆出一个新资源对象
         */
        virtual ResourcePtr clone() const = 0;

    protected:
        ID      mID;        /**< 资源ID */
        ID      mCloneID;   /**< 如果资源是从其他资源克隆出来的，该ID才有效 */
        size_t  mSize;      /**< 资源大小 */
        bool    mIsLoaded;  /**< 资源是否加载标记 */
        String  mName;      /**< 资源名称 */
    };
}


#endif  /*__T3D_RESOURCE_H__*/
