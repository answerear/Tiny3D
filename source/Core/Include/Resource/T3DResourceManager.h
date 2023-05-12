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
    /**
     * @class   ResourceManager
     * @brief   A 3D engine api.
     */
    class T3D_ENGINE_API ResourceManager : public Object
    {
    public:
        /**
         * @brief   析構函數
         */
        ~ResourceManager() override;

        virtual ResourcePtr load(const String &name);

        /**
         * @brief   從文件加載資源到內存並生成資源對象
         * @param   name    The name.
         * @return  A ResourcePtr.
         */
        virtual ResourcePtr load(const String &name, CompletedCallback callback);
        
        /**
         * @brief   從內存中卸載資源
         * @param   res The resource.
         * @return  调用成功返回 T3D_OK.
         */
        virtual TResult unload(ResourcePtr res);

        /**
         * @brief   卸載所有資源，慎用 ！
         * @return  调用成功返回 T3D_OK.
         */
        virtual TResult unloadAllResources();

        /**
         * @brief   把當前資源管理裡所有不適用資源從內存中卸載掉
         * @return  调用成功返回 T3D_OK.
         */
        virtual TResult unloadUnused();

        /**
         * @brief   從源資源克隆 一份新資源出來
         * @param [in]  src : 源資源對象.
         * @return  返回一個新的與源資源對象相同內容的對象.
         * @remarks  所有的 Resource 派生類都需要實現 Resource::clone() 接口。 
         *           本接口將會調用 Resource::clone() 接口來實現具體的克隆邏輯.
         */
        ResourcePtr clone(ResourcePtr src);

        /**
         * @brief   根據資源名稱獲取對應資源對象
         * @param [in]  name    : 資源名稱.
         * @param [in]  cloneID (Optional) : 傳入該參數直接用該ID查找，默認自動找非克隆對象.
         * @return  返回查詢的資源對象，如果返回 NULL_PTR 則表示沒有該資源.
         */
        ResourcePtr getResource(const String &name, UUID cloneID = UUID::INVALID) const;

        /**
         * @brief   根據資源 UUID 獲取對應資源對象
         * @param [in]  uuid    : 資源 UUID .
         * @return  返回查詢的資源對象，如果返回 NULL_PTR 則表示沒有該資源.
         */
        ResourcePtr getResource(UUID uuid) const;

    protected:
        /**
         * @brief   構造函數
         */
        ResourceManager();

        /**
         * @brief   創建具體資源對象，具體子類實現該方法
         * @param [in] name : 資源名稱.
         * @return  A ResourcePtr.
         */
        virtual ResourcePtr create(const String &name) = 0;

        ResourcePtr lookup(const String &name);

        bool insertCache(const ResourcePtr &resource);
     
    protected:
        typedef TList<Resource*>                Resources;
        typedef Resources::iterator             ResourcesItr;
        typedef Resources::const_iterator       ResourcesConstItr;
        typedef Resources::value_type           ResourcesValue;
     
        typedef TMap<String, Resources>         ResourcesLookup;
        typedef ResourcesLookup::iterator       ResourcesLookupItr;
        typedef ResourcesLookup::const_iterator ResourcesLookupConstItr;
        typedef ResourcesLookup::value_type     ResourcesLookupValue;

        typedef TMap<UUID, ResourcePtr>         ResourcesCache;
        typedef ResourcesCache::iterator        ResourcesCacheItr;
        typedef ResourcesCache::const_iterator  ResourcesCacheConstItr;
        typedef ResourcesCache::value_type      ResourcesCacheValue;

        ResourcesLookup  mResourcesLookup;      /**< 资源查找表 */
        ResourcesCache   mResourcesCache;       /**< 资源缓存池 */
    };
}


#endif  /*__T3D_RESOURCE_MANAGER_H__*/
