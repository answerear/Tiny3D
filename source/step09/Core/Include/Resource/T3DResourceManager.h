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

#ifndef __T3D_RESOURCE_MANAGER_H__
#define __T3D_RESOURCE_MANAGER_H__


#include "Resource/T3DResource.h"


namespace Tiny3D
{
    class T3D_ENGINE_API ResourceManager : public Object
    {
    public:
        /** 析构函数 */
        virtual ~ResourceManager();

        static ID toID(const String &name);

        /** 从文件加载资源到内存 */
        virtual ResourcePtr load(const String &name, int32_t argc, ...);

        /** 从内存中卸载资源 */
        virtual void unload(ResourcePtr &res);

        /** 把当前资源管理器里所有不使用资源从内存中卸载掉 */
        virtual void unloadUnused();

        /** 
         * @brief 从源资源克隆一份新资源出来
         * @param [in] src : 源资源对象
         * @return 返回一个新的跟源资源对象相同内容的对象
         * @remarks 所有的 Resource 派生类都需要实现 Resource::clone() 接口。
         *      本接口将会调用 Resource::clone() 接口来实现具体的克隆逻辑。
         */
        ResourcePtr clone(const ResourcePtr &src);

        /** 
         * @brief 根据资源名称获取对应资源对象 
         * @param [in] name : 资源名称
         * @param [in] cloneID : 克隆出来的资源需填写该参数，否则用默认值0
         * @return 返回查询的资源对象，如果返回NULL_PTR则没有该资源
         */
        ResourcePtr getResource(const String &name, ID cloneID = 0) const;

        /**
         * @brief 根据资源名称获取会所有同名的资源对象，原资源和克隆资源都返回
         * @param [in] name : 资源名称
         * @param [in][out] resources : 返回的资源对象列表
         * @return 有对应名称的资源则返回true，否则返回false。
         */
        bool getResources(const String &name, TList<ResourcePtr> &resources) const;

    protected:
        /** 构造函数 */
        ResourceManager();

        /** 
         * @brief 创建具体资源对象，具体子类实现该方法
         * @param [in] strName : 资源名称
         * @param [in] argc : 资源创建带的参数列表大小
         * @param [in] args : 参数列表
         */
        virtual ResourcePtr create(const String &strName, int32_t argc, va_list args) = 0;

        /** 根据名称计算资源 hash 值，作为其ID */
        static uint32_t hash(const char *str);

    protected:
        typedef TMap<ID, ResourcePtr>       Resources;
        typedef Resources::iterator         ResourcesItr;
        typedef Resources::const_iterator   ResourcesConstItr;
        typedef Resources::value_type       ResourcesValue;

        typedef TMap<String, Resources>         ResourcesMap;
        typedef ResourcesMap::iterator          ResourcesMapItr;
        typedef ResourcesMap::const_iterator    ResourcesMapConstItr;
        typedef ResourcesMap::value_type        ResourcesMapValue;

        ResourcesMap    mResourcesCache;    /**< 资源对象池 */
        ID              mCloneID;           /**< 克隆ID */
    };
}


#endif  /*__T3D_RESOURCE_MANAGER_H__*/
