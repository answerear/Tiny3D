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
#include "Serializer/T3DSerializerManager.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    TResult ResourceManager::addResource(ResourcePtr res)
    {
        TResult ret = T3D_OK;

        do
        {
            const auto it = mResourcesCache.find(res->getResourceID());

            if (it != mResourcesCache.end())
            {
                // 已经加过了
                ret = T3D_ERR_DUPLICATED_ITEM;
                break;
            }

            // 存到缓存池
            mResourcesCache.insert(ResourcesCacheValue(res->getResourceID(), res));
            
            const auto itr = mResourcesLookup.find(res->getName());
            if (itr != mResourcesLookup.end())
            {
                // 已经有相同名字的资源，加入到链表
                itr->second.push_back(res);
            }
            else
            {
                // 没有对应名字资源，创建个链表存放第一个资源
                auto i = mResourcesLookup.insert(ResourcesLookupValue(res->getName(), Resources()));
                if (i.second)
                {
                    i.first->second.push_back(res);
                }
            }
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    Resource *ResourceManager::lookup(const String &name)
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
            // 从 lookup 移除
            const auto itr = mResourcesLookup.find(resource->getName());
            if (itr != mResourcesLookup.end())
            {
                for (auto it = itr->second.begin(); it != itr->second.end(); ++it)
                {
                    if (resource->getResourceID() == (*it)->getResourceID())
                    {
                        itr->second.erase(it);
                        break;
                    }
                }
            }

            // 从 cache 移除
            mResourcesCache.erase(resource->getResourceID());
        } while (false);
    }

    //--------------------------------------------------------------------------

    ResourcePtr ResourceManager::load(Archive *metaArchive, Archive *resArchive, const String &name, int32_t argc, ...)
    {
        Meta *meta = nullptr;
        Resource *res = nullptr;
        
        do
        {
            // 查找是否有缓存
            res = lookup(name);
            if (res != nullptr)
            {
                // 已经存在了该资源
                break;
            }

            // 从 meta 档案系统获取数据
            MemoryDataStream ms;
            String metaName = name + ".meta";
            TResult ret = metaArchive->read(metaName, ms);
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Read reousrce meta [%s] from archive failed !",
                    metaName.c_str());
                break;
            }
            
            // 加载 meta 对象
            meta = loadMeta(ms);
            if (meta == nullptr)
            {
                break;
            }

            // 从资源档案系统获取数据
            MemoryDataStream rs;
            ret = resArchive->read(name, rs);
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Read resource content [%s] from archive failed !",
                    name.c_str());
                break;
            }

            // 加载 resource 对象
            va_list params;
            va_start(params, argc);
            res = loadResource(rs, argc, params);
            va_end(params);
            if (res == nullptr)
            {
                break;
            }

            // 放到缓存中
            if (!insertCache(res))
            {
                // 失败了，先卸载资源
                res = nullptr;
                break;
            }
            
            res->mMeta = meta;
            res->onLoad();
        } while (false);

        if (res == nullptr)
        {
            T3D_SAFE_DELETE(meta);
        }

        return res;
    }

    //--------------------------------------------------------------------------

    Meta *ResourceManager::loadMeta(DataStream &stream)
    {
        return T3D_SERIALIZER_MGR.deserialize<Meta>(stream);
    }

    //--------------------------------------------------------------------------

    Resource *ResourceManager::loadResource(DataStream &stream, int32_t argc, va_list args)
    {
        return T3D_SERIALIZER_MGR.deserialize<Resource>(stream);
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
            
            ret = res->onUnload();
            if (T3D_FAILED(ret))
            {
                break;
            }
            
            removeCache(res);
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ResourceManager::save(Resource *res, Archive *metaArchive, Archive *resArchive)
    {
        TResult ret = T3D_OK;

        do
        {
            // 保存 meta 对象
            MemoryDataStream ms;
            ret = saveMeta(ms, res->getMeta());
            if (T3D_FAILED(ret))
            {
                break;
            }

            // 写到 meta 档案系统里 
            String metaName = res->getName() + ".meta";
            ret = metaArchive->write(metaName, ms);
            if (T3D_FAILED(ret))
            {
                break;
            }

            // 保存资源对象
            MemoryDataStream rs;
            ret = saveResource(rs, res);
            if (T3D_FAILED(ret))
            {
                break;
            }

            // 写到 资源 档案系统里
            ret = resArchive->write(res->getName(), rs);
            if (T3D_FAILED(ret))
            {
                break;
            }
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
            if (res->getState() == Resource::State::kLoaded)
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

