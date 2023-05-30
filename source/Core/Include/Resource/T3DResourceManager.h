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

#ifndef __T3D_RESOURCE_MANAGER_H__
#define __T3D_RESOURCE_MANAGER_H__


#include "Resource/T3DResource.h"


namespace Tiny3D
{
    class T3D_ENGINE_API ResourceManager : public Object
    {
    public:        
        ~ResourceManager() override = default;
        
        virtual TResult unloadAllResources();

        virtual TResult unloadUnused();

        ResourcePtr clone(ResourcePtr src);

        ResourcePtr getResource(const String &name, UUID uuid = UUID::INVALID) const;

        ResourcePtr getResource(UUID uuid) const;

    protected:
        /**
         * @brief   構造函數
         */
        ResourceManager() = default;

        virtual ResourcePtr load(ArchivePtr archive, const String &name, int32_t argc, ...);

        virtual ResourcePtr load(const String &name, CompletedCallback callback, int32_t argc, ...);

        virtual TResult unload(ResourcePtr res);

        virtual ResourcePtr create(const String &name, int32_t argc, va_list args) = 0;

        ResourcePtr lookup(const String &name);

        bool insertCache(const ResourcePtr &resource);

        void removeCache(const ResourcePtr &resource);

    protected:
        typedef TList<Resource*> Resources;
        typedef Resources::iterator ResourcesItr;
        typedef Resources::const_iterator ResourcesConstItr;
        typedef Resources::value_type ResourcesValue;

        typedef TMap<String, Resources> ResourcesLookup;
        typedef ResourcesLookup::iterator ResourcesLookupItr;
        typedef ResourcesLookup::const_iterator ResourcesLookupConstItr;
        typedef ResourcesLookup::value_type ResourcesLookupValue;

        typedef TMap<UUID, ResourcePtr> ResourcesCache;
        typedef ResourcesCache::iterator ResourcesCacheItr;
        typedef ResourcesCache::const_iterator ResourcesCacheConstItr;
        typedef ResourcesCache::value_type ResourcesCacheValue;

        String mArchiveType;
        ResourcesLookup mResourcesLookup; /**< 资源查找表 */
        ResourcesCache mResourcesCache; /**< 资源缓存池 */
    };
}


#endif  /*__T3D_RESOURCE_MANAGER_H__*/
