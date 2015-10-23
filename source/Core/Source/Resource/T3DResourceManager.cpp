
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

    Resource *ResourceManager::load(const String &name)
    {
        Resource *res = nullptr;

        // First, search cache
        auto itr = mResourceCache.find(name);

        if (itr != mResourceCache.end())
        {
            // Found it in cache.
            Resources &resources = itr->second;

            if (resources.size() > 0)
            {
                res = resources.begin()->second;
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
                    delete res;
                }
            }
        }

        return res;
    }

    void ResourceManager::unload(Resource *res)
    {
        if (res->referCount() == 1)
        {
            // Only one instance is used. It should be deleted.
            auto itr = mResourceCache.find(res->getName());

            if (itr != mResourceCache.end())
            {
                // The resource is valid and in the cache.
                Resources &resources = itr->second;

                if (res->isCloned())
                {
                    auto i = resources.find(res->getCloneID());

                    if (i != resources.end())
                    {
                        res->unload();
                        resources.erase(i);
                    }
                }
                else
                {
                    resources.erase(resources.begin());
                }
            }
        }
        else
        {
            // Only decrease reference count of the resource.
            res->release();
        }
    }

    Resource *ResourceManager::clone(const Resource *src)
    {
        uint32_t unCloneID = (++mCloneID);

        Resource *res = src->clone();

        if (res != nullptr)
        {
            res->mCloneID = unCloneID;

            auto i = mResourceCache.find(src->getName());

            if (i != mResourceCache.end())
            {
                Resources &resources = i->second;
                resources.insert(ResPairValue(unCloneID, res));
            }
        }
        
        return res;
    }

    Resource *ResourceManager::getResource(const String &name, 
        uint32_t cloneID /* = 0 */) const
    {
        Resource *res = nullptr;

        auto i = mResourceCache.find(name);

        if (i != mResourceCache.end())
        {
            const Resources &resources = i->second;

            if (0 == cloneID)
            {
                res = resources.begin()->second;
            }
            else
            {
                auto itr = resources.find(cloneID);
                if (itr != resources.end())
                {
                    res = itr->second;
                }
            }
        }

        return res;
    }

    bool ResourceManager::getResources(const String &name,
        std::list<Resource*> &rList) const
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

