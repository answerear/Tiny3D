
#include "Resource/T3DResourceManager.h"


namespace Tiny3D
{
    ResourceManager::ResourceManager()
        : mCloneID(0)
    {

    }

    ResourceManager::~ResourceManager()
    {

    }

    uint32_t ResourceManager::toID(const String &name)
    {
        return hash(name.c_str());
    }

    uint32_t ResourceManager::hash(const char *str)
    {
        uint32_t value = 5381;

        while (*str)
        {
            value += (value << 5) + (*str++);
        }

        return (value & 0x7FFFFFFF);
    }

    ResourcePtr ResourceManager::load(const String &name)
    {
        ResourcePtr res = nullptr;

        // First, search cache
        auto itr = mResourceCache.find(name);

        if (itr != mResourceCache.end())
        {
            // Found it in cache.
            Resources &resources = itr->second;

            if (resources.size() > 0)
            {
                auto i = resources.find(0);

                if (i != resources.end())
                {
                    // Found in original resource list
                    res = i->second;
                }
                else
                {
                    // Found not in original resource list
                    res = create(name);

                    if (res != nullptr)
                    {
                        bool bRet = res->load();

                        if (bRet)
                        {
                            resources.insert(ResPairValue(0, res));
                        }
                        else
                        {
                            res = nullptr;
                        }
                    }
                }
            }
        }
        else
        {
            // Found not, it should create a new instance.
            res = create(name);

            if (res != nullptr)
            {
                bool bRet = res->load();

                if (bRet)
                {
                    Resources resources;
                    resources.insert(ResPairValue(0, res));
                    mResourceCache.insert(ResMapPairValue(name, resources));
                }
                else
                {
                    res = nullptr;
                }
            }
        }

        return res;
    }

    void ResourceManager::unload(ResourcePtr &res)
    {
        if (res->referCount() > 1)
        {
            Resource *r = res;
            res = nullptr;

            if (r->referCount() == 1)
            {
                // Only one instance is used. It should be deleted.
                auto itr = mResourceCache.find(r->getName());

                if (itr != mResourceCache.end())
                {
                    // The resource is valid and in the cache.
                    Resources &resources = itr->second;

                    if (res->isCloned())
                    {
                        // erase cloning resource
                        resources.erase(r->getCloneID());
                    }
                    else
                    {
                        // erase original resource
                        resources.erase(0);
                    }
                }
            }
        }
        else
        {
            res = nullptr;
        }
    }

    void ResourceManager::unloadUnused()
    {
        auto itr = mResourceCache.begin();

        while (itr != mResourceCache.end())
        {
            Resources &resources = itr->second;

            auto i = resources.begin();

            while (i != resources.end())
            {
                ResourcePtr &res = i->second;

                if (res->referCount() == 1)
                {
                    resources.erase(i++);
                }
                else
                {
                    ++i;
                }
            }

            if (resources.empty())
            {
                mResourceCache.erase(itr++);
            }
            else
            {
                ++itr;
            }
        }
    }

    ResourcePtr ResourceManager::clone(const ResourcePtr &src)
    {
        uint32_t unCloneID = (++mCloneID);

        ResourcePtr res = src->clone();

        if (res != nullptr)
        {
            res->mCloneID = unCloneID;

            auto i = mResourceCache.find(src->getName());

            if (i != mResourceCache.end())
            {
                Resources &resources = i->second;
                resources.insert(ResPairValue(unCloneID, res));
            }
            else
            {
                T3D_ASSERT(0);
            }
        }
        
        return res;
    }

    ResourcePtr ResourceManager::getResource(const String &name, uint32_t cloneID /* = 0 */) const
    {
        ResourcePtr res = nullptr;

        auto i = mResourceCache.find(name);

        if (i != mResourceCache.end())
        {
            const Resources &resources = i->second;

           auto itr = resources.find(cloneID);

           if (itr != resources.end())
           {
               res = itr->second;
           }
        }

        return res;
    }

    bool ResourceManager::getResources(const String &name, std::list<ResourcePtr> &rList) const
    {
        bool bRet = false;
        auto i = mResourceCache.find(name);

        if (i != mResourceCache.end())
        {
            const Resources &resources = i->second;

            auto itr = resources.begin();

            while (itr != resources.end())
            {
                rList.push_back(itr->second);
                ++itr;
            }

            bRet = true;
        }

        return bRet;
    }
}

