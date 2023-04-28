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
#include <sstream>


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    ResourceManager::ResourceManager()
    {

    }

    //--------------------------------------------------------------------------

    ResourceManager::~ResourceManager()
    {
        // unloadAllResources();
    }

    //--------------------------------------------------------------------------

    ResourcePtr ResourceManager::lookup(const String &name)
    {
        ResourcePtr res;
        auto it = mResourcesLookup.find(name);
        
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
            const String &name = resource->getName();
            auto rval = mResourcesLookup.insert(ResourcesLookupValue(name, Resources()));
            if (!rval.second)
            {
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Insert resource [%s] to cache failed !", name.c_str());
                break;
            }

            rval.first->second.push_back(resource.get());
            
            auto rt = mResourcesCache.insert(ResourcesCacheValue(resource->getResourceID(), resource.get()));
            if (!rt.second)
            {
                mResourcesLookup.erase(rval.first);
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Insert resource [%s] to cache failed !", name.c_str());
                break;
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    ResourcePtr ResourceManager::load(const String &name, int32_t argc, ...)
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
            va_list args;
            va_start(args, argc);
            res = create(name, argc, args);
            va_end(args);
            if (res == nullptr)
            {
                // 創建失敗
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Create resource [%s] object failed !", name.c_str());
                break;
            }

            // 加载资源
            TResult ret = res->load();
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
                res = nullptr;
                break;
            }
        } while (false);

        return res;
    }

    //--------------------------------------------------------------------------

    ResourcePtr ResourceManager::load(const String &name, CompletedCallback callback, int32_t argc, ...)
    {
        ResourcePtr res = nullptr;

        do 
        {
            res = lookup(name);
            if (res != nullptr)
            {
                // 已经存在了该资源
                break;
            }

            // lookup 中没有，创建对象
            va_list args;
            va_start(args, argc);
            res = create(name, argc, args);
            va_end(args);
            if (res == nullptr)
            {
                // 創建失敗
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Create resource [%s] object failed !", name.c_str());
                break;
            }
        } while (false);

        return res;
    }

    //--------------------------------------------------------------------------

    TResult ResourceManager::unload(ResourcePtr res)
    {
        TResult ret = T3D_OK;

        do 
        {
            // if (res == nullptr)
            // {
            //     ret = T3D_ERR_RES_INVALID_OBJECT;
            //     T3D_LOG_ERROR(LOG_TAG_RESOURCE,
            //         "Invalid resource object !");
            //     break;
            // }
            //
            // if (res->mResReferCount == 0)
            // {
            //     // 沒有地方引用了，已經卸載了
            //     break;
            // }
            //
            // // 引用計數遞減，減少一次引用
            // res->mResReferCount--;
            //
            // if (res->resReferCount() > 0)
            // {
            //     // 資源還有其他地方引用，不卸載
            //     break;
            // }
            //
            // // 卸載資源
            // UUID resID = (res->isCloned() ? res->getCloneID() : res->getID());
            // auto itr = mResourcesCache.find(resID);
            // if (itr == mResourcesCache.end())
            // {
            //     ret = T3D_ERR_NOT_FOUND;
            //     T3D_LOG_ERROR(LOG_TAG_RESOURCE,
            //         "Couldn't find the resource [%s] in the cache !", 
            //         res->getName().c_str());
            //     break;
            // }
            //
            // // 讓資源自己處理卸載事情
            // ret = res->unload();
            // if (T3D_FAILED(ret))
            // {
            //     break;
            // }
            //
            // // 最後從緩存中清除掉
            // mResourcesCache.erase(itr);
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ResourceManager::unloadAllResources()
    {
        TResult ret = T3D_OK;

        // // 清理所有资源缓存
        // auto itr = mResourcesCache.begin();
        // while (itr != mResourcesCache.end())
        // {
        //     auto res = itr->second;
        //     itr++;
        //     if (res->isLoaded())
        //         unload(res);
        // }
        //
        // mResourcesCache.clear();
        //
        // // 清理所有资源元信息缓存
        // mMetaCache.clear();

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
            // if (src == nullptr)
            // {
            //     T3D_LOG_ERROR(LOG_TAG_RESOURCE,
            //         "Invalid source object to clone !");
            //     break;
            // }
            //
            // auto itr = mMetaCache.find(src->getName());
            // if (itr == mMetaCache.end())
            // {
            //     T3D_LOG_ERROR(LOG_TAG_RESOURCE,
            //         "Invalid source object ! Because it did not be loaded yet. ");
            //     break;
            // }
            //
            // UUID cloneID = UUID::generate();
            // MetaPtr meta = itr->second->clone();
            // meta->uuid = cloneID;
            // res = src->clone(meta);
            // if (res == nullptr)
            // {
            //     T3D_LOG_ERROR(LOG_TAG_RESOURCE,
            //         "Clone resource [%s] object failed !", 
            //         src->getName().c_str());
            //     break;
            // }
            //
            // res->mCloneID = cloneID;
            //
            // auto rval = mResourcesCache.insert(ResourcesValue(cloneID, res));
            // if (!rval.second)
            // {
            //     T3D_LOG_ERROR(LOG_TAG_RESOURCE,
            //         "Add resource [%s] cloned to cache failed !",
            //         res->getName().c_str());
            //     res = nullptr;
            // }
        } while (false);

        return res;
    }

    //--------------------------------------------------------------------------

    ResourcePtr ResourceManager::getResource(
        const String &name, UUID cloneID /* = UUID::INVALID */) const
    {
        ResourcePtr res;

        do 
        {
            // UUID resID = UUID::INVALID;
            // if (cloneID != UUID::INVALID)
            // {
            //     // 克隆對象
            //     resID = cloneID;
            // }
            // else
            // {
            //     auto it = mMetaCache.find(name);
            //     if (it == mMetaCache.end())
            //     {
            //         break;
            //     }
            //
            //     resID = it->second->uuid;
            // }
            //
            // auto itr = mResourcesCache.find(resID);
            // if (itr == mResourcesCache.end())
            // {
            //     break;
            // }
            //
            // res = itr->second;
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

