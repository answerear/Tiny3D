/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/


#include "Resource/T3DResourceManager.h"
#include "Resource/T3DResource.h"
#include "T3DErrorDef.h"
#include "Kernel/T3DArchive.h"
// #include "Kernel/T3DArchiveManager.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    // TResult ResourceManager::addResource(ResourcePtr res)
    // {
    //     TResult ret = T3D_OK;
    //
    //     do
    //     {
    //         const auto it = mResourcesCache.find(res->getUUID());
    //
    //         if (it != mResourcesCache.end())
    //         {
    //             // 已经加过了
    //             ret = T3D_ERR_DUPLICATED_ITEM;
    //             break;
    //         }
    //
    //         // 存到缓存池
    //         mResourcesCache.emplace(res->getUUID(), res);
    //         
    //         const auto itr = mResourcesLookup.find(res->getName());
    //         if (itr != mResourcesLookup.end())
    //         {
    //             // 已经有相同名字的资源，加入到链表
    //             itr->second.push_back(res);
    //         }
    //         else
    //         {
    //             // 没有对应名字资源，创建个链表存放第一个资源
    //             auto i = mResourcesLookup.emplace(res->getName(), Resources());
    //             if (i.second)
    //             {
    //                 i.first->second.push_back(res);
    //             }
    //         }
    //     } while (false);
    //     
    //     return ret;
    // }

    //--------------------------------------------------------------------------

    Resource *ResourceManager::lookup(const String &filename) const
    {
        Resource *res = nullptr;
        const auto &it = mResourcesLookup.find(filename);
        if (it != mResourcesLookup.end())
        {
            res = it->second;
        }
        return res;
    }

    //--------------------------------------------------------------------------

    Resource *ResourceManager::lookup(const UUID &uuid) const
    {
        Resource *res = nullptr;
        const auto &it = mResourcesCache.find(uuid);
        if (it != mResourcesCache.end())
        {
            res = it->second;
        }
        return res;
    }
    
    //--------------------------------------------------------------------------

    bool ResourceManager::insertCache(const ResourcePtr &resource)
    {
        bool ret = true;

        do
        {
            const auto it = mResourcesCache.find(resource->getUUID());

            if (it != mResourcesCache.end())
            {
                // 已经加过了
                T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Insert resource [%s] to cache failed ! Already exists !", resource->getName().c_str());
                ret = false;
                break;
            }

            // 存到缓存池
            mResourcesCache.emplace(resource->getUUID(), resource);
            
            ret = insertLUT(resource);
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    void ResourceManager::removeCache(const ResourcePtr &resource)
    {
        // 从 lookup 移除
        if (!resource->getFilename().empty())
        {
            mResourcesLookup.erase(resource->getFilename());
        }
            
        // 从 cache 移除
        mResourcesCache.erase(resource->getUUID());
    }
    
    //--------------------------------------------------------------------------

    
    bool ResourceManager::insertLUT(const ResourcePtr &resource)
    {
        bool ret = false;

        String key;
        if (!resource->getFilename().empty())
        {
            key = resource->getFilename();
        }
        else
        {
            key = resource->getName();
        }

        const auto itr = mResourcesLookup.find(resource->getFilename());
        if (itr == mResourcesLookup.end())
        {
            // 沒緩存，則緩存
            mResourcesLookup.emplace(key, resource);
            ret = true;
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    void ResourceManager::removeLUT(const String &name)
    {
        auto it = mResourcesLookup.find(name);
        if (it != mResourcesLookup.end())
        {
            mResourcesLookup.erase(it);
        }
    }

    //--------------------------------------------------------------------------
    
    ResourcePtr ResourceManager::createResource(const String &name, int32_t argc, ...)
    {
        ResourcePtr res = nullptr;

        do
        {
            // 查找是否有缓存，如果有，则返回已经存在的
            res = lookup(name);
            if (res != nullptr)
            {
                // 已经存在了该资源
                break;
            }

            // 新建资源对象
            va_list args;
            va_start(args, argc);
            res = newResource(name, argc, args);
            va_end(args);
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

            // 回调通知已创建
            TResult ret = res->onCreate();
            if (T3D_FAILED(ret))
            {
                res->mState = Resource::State::kUnloaded;
                removeCache(res);
                res = nullptr;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Create resource [%s] failed !", name.c_str());
                break;
            }
        } while (false);
        
        return res;
    }

    //--------------------------------------------------------------------------

    ResourcePtr ResourceManager::load(Archive *archive, const String &filename)
    {
        ResourcePtr res = nullptr;
        
        do
        {
            // 查找是否有缓存
            res = lookup(filename);
            if (res != nullptr)
            {
                // 已经存在了该资源
                break;
            }

            // 加载资源
            res = loadResource(archive, filename);
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

            // 回调通知已加载
            TResult ret = res->onLoad(archive);
            if (T3D_FAILED(ret))
            {
                res->mState = Resource::State::kUnloaded;
                removeCache(res);
                res = nullptr;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Load resource [%s] failed !", filename.c_str());
                break;
            }
        } while (false);

        return res;
    }

    //--------------------------------------------------------------------------

    ResourcePtr ResourceManager::loadResource(Archive *archive, const String &filename)
    {
        ResourcePtr res = nullptr;

        do
        {
            TResult ret = archive->read(filename, [this, &res, &filename](DataStream &stream, const String &name)
                {
                    // 加载 resource 对象
                    TResult ret = T3D_OK;
                    res = loadResource(filename, stream);
                    if (res == nullptr)
                    {
                        ret = T3D_ERR_RES_LOAD_FAILED;
                    }
                    return ret;
                });
            
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Failed to load resource [%s] !", filename.c_str());
                res = nullptr;
                break;
            }

            res->setFilename(filename);
        } while (false);

        return res;
    }

    //--------------------------------------------------------------------------

    ResourcePtr ResourceManager::load(Archive *archive, const UUID &uuid)
    {
        ResourcePtr res = nullptr;
        
        do
        {
            // 查找是否有缓存
            res = lookup(uuid);
            if (res != nullptr)
            {
                // 已經存在，直接返回
                break;
            }

            // 加载资源
            res = loadResource(archive, uuid);
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

            // 回调通知已加载
            TResult ret = res->onLoad(archive);
            if (T3D_FAILED(ret))
            {
                res->mState = Resource::State::kUnloaded;
                removeCache(res);
                res = nullptr;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Load resource [%s] failed ! UUID : %s", uuid.toString().c_str());
                break;
            }
        } while (false);

        return res;
    }

    //--------------------------------------------------------------------------

    ResourcePtr ResourceManager::loadResource(Archive *archive, const UUID &uuid)
    {
        ResourcePtr res = nullptr;

        do
        {
            TResult ret = archive->read(uuid, [this, &res](DataStream &stream, const String &filename)
                {
                    // 加载 resource 对象
                    TResult ret = T3D_OK;
                    res = loadResource(stream);
                    if (res == nullptr)
                    {
                        ret = T3D_ERR_RES_LOAD_FAILED;
                    }
                    else if (!filename.empty())
                    {
                        res->setFilename(filename);
                    }
                    return ret;
                });
            
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Failed to load resource (uuid: %s) !", uuid.toString().c_str());
                res = nullptr;
            }
        } while (false);

        return res;
    }

    //--------------------------------------------------------------------------

    ResourcePtr ResourceManager::loadResource(DataStream &stream)
    {
        T3D_ASSERT(false);
        T3D_LOG_WARNING(LOG_TAG_RESOURCE, "ResourceManager::loadResource has not implement !");
        return nullptr;
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
                T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Unload resource failed ! Invalid resource object !");
                break;
            }

            // 回调通知卸载
            ret = res->onUnload();
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Unload resource [%s] failed !", res->getName().c_str());
                break;
            }

            // 从缓存中移除
            removeCache(res);
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ResourceManager::save(Archive *archive, const String &filename, Resource *res)
    {
        TResult ret = T3D_OK;

        do
        {
            if (archive == nullptr || res == nullptr || filename.empty())
            {
                // 参数不合法
                T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Save resource failed ! Invalid param ! archive = %p, res = %p, filename = %s", archive, res, filename.c_str());
                ret = T3D_ERR_INVALID_PARAM;
                break;
            }

            // 回调通知将要保存
            ret = res->onSave(archive);
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Save resource [%s (%s)] failed !", res->getName().c_str(), res->getUUID().toString().c_str());
                break;
            }

            // 保存资源对象
            ret = archive->write(filename, [this, res](DataStream &stream, const String &name)
                {
                    return saveResource(stream, res);
                });
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Save resource [%s] failed !", filename.c_str());
                break;
            }

            res->setFilename(filename);

            // 把以资源名称为 key 放在 LUT 里面的对象删掉
            removeLUT(res->getName());
            // 重新使用资源文件名作为 key 的资源对象放到 LUT 里
            insertLUT(res);
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ResourceManager::save(Archive *archive, Resource *res)
    {
        TResult ret = T3D_OK;

        do
        {
            if (archive == nullptr || res == nullptr)
            {
                // 参数不合法
                T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Save resource failed ! Invalid param ! archive = %p, res = %p", archive, res);
                ret = T3D_ERR_INVALID_PARAM;
                break;
            }

            // 回调通知将要保存
            ret = res->onSave(archive);
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Save resource [%s (%s)] failed ! ", res->getName().c_str(), res->getUUID().toString().c_str());
                break;
            }
            
            // 保存资源对象
            ret = archive->write(res->getUUID(), [this, res](DataStream &stream, const String &name)
                {
                    return saveResource(stream, res);
                });
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Save resource [%s] failed ! UUID : %s", res->getName().c_str(), res->getUUID().toString().c_str());
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
            {
                ret = unload(res);
                T3D_ASSERT(T3D_SUCCEEDED(ret));
            }
            else
            {
                T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Resource invalid state [%d] !", res->getState());
                mResourcesCache.erase(itr);
            }
        }

        mResourcesLookup.clear();

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
                T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Invalid source resource to clone !");
                break;
            }

            // 克隆对象
            ResourcePtr dst = src->clone();
            if (!insertCache(dst))
            {
                break;
            }

            res = dst;
        } while (false);

        return res;
    }
    
    //--------------------------------------------------------------------------

    Resource *ResourceManager::getResource(const String &filename) const
    {
        ResourcePtr res;

        do 
        {
            // 没有指明 UUID，只能到 lookup 去找
            auto itr = mResourcesLookup.find(filename);
            if (itr == mResourcesLookup.end())
            {
                // 没有找到
                break;
            }
                
            res = itr->second;
        } while (false);

        return res;
    }

    //--------------------------------------------------------------------------

    Resource *ResourceManager::getResource(const UUID &uuid) const
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

