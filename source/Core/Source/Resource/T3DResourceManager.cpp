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


#include "Resource/T3DResourceManager.h"
#include "T3DErrorDef.h"
#include "Kernel/T3DArchive.h"
#include "Kernel/T3DArchiveManager.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    ResourcePtr ResourceManager::lookup(const String &name)
    {
        ResourcePtr res;
        const auto it = mResourcesLookup.find(name);
        
        if (it != mResourcesLookup.end())
        {
            if (!it->second.empty())
            {
                // 已经加载过了
                res = it->second.front();
            }
        }
        
        return res;
    }

    //--------------------------------------------------------------------------

    bool ResourceManager::insertCache(const ResourcePtr &resource)
    {
        bool ret = true;

        do
        {
            // 插入根据名称查找表
            const String &name = resource->getName();
            auto rval = mResourcesLookup.insert(ResourcesLookupValue(name, Resources()));
            if (!rval.second)
            {
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Insert resource [%s] to cache failed !", name.c_str());
                ret = false;
                break;
            }

            rval.first->second.push_back(resource.get());

            // 插入资源缓存池
            auto rt = mResourcesCache.insert(ResourcesCacheValue(resource->getResourceID(), resource.get()));
            if (!rt.second)
            {
                mResourcesLookup.erase(rval.first);
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Insert resource [%s] to cache failed !", name.c_str());
                ret = false;
                break;
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    void ResourceManager::removeCache(const ResourcePtr &resource)
    {
        do
        {
            mResourcesLookup.erase(resource->getName());
            mResourcesCache.erase(resource->getResourceID());
        } while (false);
    }

    //--------------------------------------------------------------------------

    ResourcePtr ResourceManager::load(ArchivePtr archive, const String &name, int32_t argc, ...)
    {
        ResourcePtr res;

        do
        {
            res = lookup(name);
            if (res != nullptr)
            {
                // 已经存在了该资源
                break;
            }

            // lookup 中没有，创建对象
            va_list params;
            va_start(params, argc);
            res = create(name, argc, params);
            va_end(params);
            
            if (res == nullptr)
            {
                // 创建失败
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Create resource [%s] object failed !", name.c_str());
                break;
            }

            // 加载资源
            TResult ret = res->load(archive);
            if (T3D_FAILED(ret))
            {
                // 加載失敗
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Load resource [%s] failed !", name.c_str());
                res = nullptr;
                break;
            }

            // 放到缓存中
            if (!insertCache(res))
            {
                // 失败了，先卸载资源
                res->unload();
                res = nullptr;
                break;
            }
        } while (false);

        return res;
    }

    //--------------------------------------------------------------------------

    ResourcePtr ResourceManager::load(const String &name, CompletedCallback callback, int32_t argc, ...)
    {
        ResourcePtr res;

        do 
        {
            // res = lookup(name);
            // if (res != nullptr)
            // {
            //     // 已经存在了该资源
            //     break;
            // }
            //
            // // lookup 中没有，创建对象
            // va_list args;
            // va_start(args, argc);
            // res = create(name, argc, args);
            // va_end(args);
            // if (res == nullptr)
            // {
            //     // 創建失敗
            //     T3D_LOG_ERROR(LOG_TAG_RESOURCE,
            //         "Create resource [%s] object failed !", name.c_str());
            //     break;
            // }
        } while (false);

        return res;
    }

    //--------------------------------------------------------------------------

    TResult ResourceManager::unload(ResourcePtr res)
    {
        TResult ret = T3D_OK;

        do 
        {
            if (res == nullptr)
            {
                ret = T3D_ERR_RES_INVALID_OBJECT;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Invalid resource object !");
                break;
            }
            
            ret = res->unload();
            if (T3D_FAILED(ret))
            {
                break;
            }
            
            removeCache(res);
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ResourceManager::unloadAllResources()
    {
        TResult ret = T3D_OK;

        // 清理所有资源缓存
        while (!mResourcesCache.empty())
        {
            auto itr = mResourcesCache.begin();
            auto res = itr->second;
            if (res->isLoaded())
                unload(res);
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ResourceManager::unloadUnused()
    {
        TResult ret = T3D_OK;

        do 
        {
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    ResourcePtr ResourceManager::clone(ResourcePtr src)
    {
        ResourcePtr res;

        do 
        {
            if (src == nullptr)
            {
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Invalid source resource to clone !");
                break;
            }

            // 克隆对象
            ResourcePtr dst = src->clone();
            // 重新生成 UUID
            dst->mMeta->uuid = UUID::generate();
            if (!insertCache(dst))
            {
                break;
            }

            res = dst;
        } while (false);

        return res;
    }

    //--------------------------------------------------------------------------

    ResourcePtr ResourceManager::getResource(
        const String &name, UUID uuid /* = UUID::INVALID */) const
    {
        ResourcePtr res;

        do 
        {
            if (uuid == UUID::INVALID)
            {
                // 没有指明 UUID，只能到 lookup 去找
                auto itr = mResourcesLookup.find(name);
                if (itr == mResourcesLookup.end())
                {
                    // 没有找到
                    break;
                }

                if (itr->second.empty())
                {
                    // 空的链表，也是没找到
                    break;
                }
                
                res = getResource(itr->second.front()->getResourceID());
            }
            else
            {
                // 指明 UUID，直接到 cache 去找
                auto itr = mResourcesCache.find(uuid);
                if (itr == mResourcesCache.end())
                {
                    // 没找到
                    break;
                }

                res = itr->second;
            }
        } while (0);

        return res;
    }

    //--------------------------------------------------------------------------

    ResourcePtr ResourceManager::getResource(UUID uuid) const
    {
        ResourcePtr res;

        do
        {
            auto itr = mResourcesCache.find(uuid);
            if (itr == mResourcesCache.end())
            {
                break;
            }

            res = itr->second;
        } while (false);

        return res;
    }
}

