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

    TResult ResourceManager::addResource(ResourcePtr res)
    {
        TResult ret = T3D_OK;

        do
        {
            const auto it = mResourcesCache.find(res->getUUID());

            if (it != mResourcesCache.end())
            {
                // 已经加过了
                ret = T3D_ERR_DUPLICATED_ITEM;
                break;
            }

            // 存到缓存池
            mResourcesCache.insert(ResourcesCacheValue(res->getUUID(), res));
            
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
            auto rval = mResourcesLookup.emplace(name, Resources());
            if (!rval.second)
            {
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Insert resource [%s] to cache failed !", name.c_str());
                ret = false;
                break;
            }

            rval.first->second.push_back(resource.get());

            // 插入资源缓存池
            auto rt = mResourcesCache.emplace(resource->getUUID(), resource.get());
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
                    if (resource->getUUID() == (*it)->getUUID())
                    {
                        itr->second.erase(it);
                        break;
                    }
                }

                if (itr->second.empty())
                {
                    mResourcesLookup.erase(itr);
                }
            }

            // 从 cache 移除
            mResourcesCache.erase(resource->getUUID());
        } while (false);
    }

    //--------------------------------------------------------------------------

    ResourcePtr ResourceManager::createResource(const String &name, int32_t argc, ...)
    {
        ResourcePtr res = nullptr;

        do
        {
            // 查找是否有缓存，如果有，则报错
            res = lookup(name);
            if (res != nullptr)
            {
                // 已经存在了该资源
                break;
            }
            
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

            res->onCreate();
        } while (false);
        
        return res;
    }

    //--------------------------------------------------------------------------

    ResourcePtr ResourceManager::load(Archive *archive, const String &name)
    {
        ResourcePtr res = nullptr;
        
        do
        {
            // 查找是否有缓存
            res = lookup(name);
            if (res != nullptr)
            {
                // 已经存在了该资源
                break;
            }

            res = loadResource(archive, name);
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
            
            TResult ret = res->onLoad(archive);
            if (T3D_FAILED(ret))
            {
                removeCache(res);
                res = nullptr;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Load resource [%s] failed !", res->getName().c_str());
                break;
            }
        } while (false);

        return res;
    }

    //--------------------------------------------------------------------------

    ResourcePtr ResourceManager::loadResource(Archive *archive, const String &name)
    {
        ResourcePtr res = nullptr;

        do
        {
            TResult ret = archive->read(name, [this, &res, &name](DataStream &stream)
            {
                TResult ret = T3D_OK;
                // 加载 resource 对象
                res = loadResource(name, stream);
                if (res == nullptr)
                {
                    ret = T3D_ERR_RES_LOAD_FAILED;
                }
                return ret;
            });
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
                T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Invalid resource object !");
                break;
            }
            
            ret = res->onUnload();
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Unload resource [%s] failed !", res->getName().c_str());
                break;
            }
            
            removeCache(res);
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ResourceManager::save(Archive *archive, Resource *res)
    {
        TResult ret = T3D_OK;

        do
        {
            ret = res->onSave(archive);
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Save resource [%s] failed !", res->getName().c_str());
                break;
            }
            
            // 保存资源对象
            ret = archive->write(res->getName(), [this, res](DataStream &stream)
            {
                return saveResource(stream, res);
            });
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
                T3D_ASSERT(T3D_SUCCEEDED(ret), "Unload resource failed !");
            }
            else
            {
                T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Resource invalid state [%d] !", res->getState());
                mResourcesCache.erase(itr);
            }
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

    ResourcePtr ResourceManager::getResource(const String &name, UUID uuid /* = UUID::INVALID */) const
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
                
                res = getResource(itr->second.front()->getUUID());
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
        } while (false);

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

