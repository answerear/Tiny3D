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
#include "Serializer/T3DSerializerManager.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    ResourceManagerPtr ResourceManager::create()
    {
        return new ResourceManager();
    }

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

    ResourcePtr ResourceManager::load(const String &archiveName, const String &name)
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
            ArchivePtr archive;
            if (!T3D_ARCHIVE_MGR.getArchive(archiveName, Archive::AccessMode::kRead, archive))
            {
                // 没有对应的档案系统对象
                T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Get archive(%s) [%s] failed !",
                    archiveName.c_str(), name.c_str());
                break;
            }

            res = loadFromArchive(name, archive);
            if (res == nullptr)
            {
                break;
            }

            // 放到缓存中
            if (!insertCache(res))
            {
                res = nullptr;
                break;
            }

            // 回调加载
            if (T3D_FAILED(res->onLoad()))
            {
                removeCache(res);
                break;
            }

            res->mState = Resource::State::kLoaded;
        } while (false);

        return res;
    }

    //--------------------------------------------------------------------------

    ResourcePtr ResourceManager::loadFromArchive(const String &name, const ArchivePtr &archive)
    {
        ResourcePtr res;

        do
        {
            MemoryDataStream stream;
            TResult ret = archive->read(name, stream);
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Read reousrce data [%s] from archive failed !",
                    name.c_str());
                break;
            }

            uint8_t *buffer = nullptr;
            size_t bufSize = 0;
            stream.getBuffer(buffer, bufSize);
            
            // 读取 meta 信息
            uint32_t metaSize = 0;
            if (bufSize < sizeof(metaSize))
            {
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Invalid data size for resource [%s] !", name.c_str());
                break;
            }
            // meta 数据大小
            memcpy(&metaSize, buffer, sizeof(metaSize));
            if (bufSize < metaSize)
            {
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Invalid meta size for resource [%s] !", name.c_str());
                break;
            }
            // meta 数据
            uint8_t *metaData = buffer + sizeof(metaSize);
            MemoryDataStream ms(metaData, metaSize, false);
            // 反序列化出 meta 对象
            MetaPtr meta = T3D_SERIALIZER_MGR.deserialize<Meta>(ms);
            if (meta == nullptr)
            {
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Deserialize meta for resource [%s] failed !", name.c_str());
                break;
            }

            // 读取资源数据
            if (bufSize < metaSize + sizeof(uint32_t) * 2)
            {
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Invalid data size for resource [%s] !", name.c_str());
                break;
            }
            
            uint8_t *data = buffer + metaSize;
            uint32_t dataSize = 0;
            // 资源数据大小
            memcpy(&dataSize, data, sizeof(dataSize));
            // 资源数据
            data += sizeof(dataSize);
            MemoryDataStream ds(data, dataSize, false);
            res = T3D_SERIALIZER_MGR.deserialize<Resource>(ds);
            if (res == nullptr)
            {
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Deserialize resource [%s] failed !", name.c_str());
                break;
            }

            res->mMeta = meta;
        } while (false);
        
        return res;
    }

    //--------------------------------------------------------------------------

    ResourcePtr ResourceManager::load(const String &archiveName, const String &name, CompletedCallback callback)
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
            ret = res->onUnload();
            if (T3D_FAILED(ret))
            {
                break;
            }

            res->mState = Resource::State::kUnloaded;
            
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

