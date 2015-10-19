
#include "Resource/T3DResourceManager.h"


namespace Tiny3D
{
    ResourceManager::ResourceManager()
        : m_unCloneID(0)
    {

    }

    ResourceManager::~ResourceManager()
    {

    }

    uint32_t ResourceManager::toID(const TString &strName)
    {
        return hash(strName.c_str());
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

    ResourcePtr ResourceManager::load(const TString &strName)
    {
        ResourcePtr res;

        // First, search cache
        auto itr = m_ResourceCache.find(strName);

        if (itr != m_ResourceCache.end())
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
            Resource *pResource = create(strName);

            if (pResource != nullptr)
            {
                bool bRet = pResource->load();

                if (bRet)
                {
                    ResourcePtr r(pResource);
                    res = r;
                    Resources resources;
                    resources.insert(ResPairValue(0, res));
                    m_ResourceCache.insert(ResMapPairValue(strName, resources));
                }
                else
                {
                    delete pResource;
                }
            }
        }

        return res;
    }

    void ResourceManager::unload(ResourcePtr &res)
    {
        if (res.unique())
        {
            // Only one instance is used. It should be deleted.

            auto itr = m_ResourceCache.find(res->getName());

            if (itr != m_ResourceCache.end())
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
            res.reset();
        }
    }

    ResourcePtr ResourceManager::clone(const ResourcePtr &rkSrcResource)
    {
        uint32_t unCloneID = (++m_unCloneID);

        ResourcePtr res;
        Resource *pResource = rkSrcResource->clone();

        if (pResource != nullptr)
        {
            pResource->m_unCloneID = unCloneID;

            auto i = m_ResourceCache.find(rkSrcResource->getName());

            if (i != m_ResourceCache.end())
            {
                ResourcePtr r(pResource);
                res = r;
                Resources &resources = i->second;
                resources.insert(ResPairValue(unCloneID, r));
            }
        }
        
        return res;
    }

    ResourcePtr ResourceManager::getResource(const TString &strName, 
        uint32_t unCloneID /* = 0 */) const
    {
        auto i = m_ResourceCache.find(strName);

        if (i != m_ResourceCache.end())
        {
            const Resources &resources = i->second;

            if (0 == unCloneID)
            {
                return resources.begin()->second;
            }
            else
            {
                auto itr = resources.find(unCloneID);
                if (itr != resources.end())
                {
                    return itr->second;
                }
            }
        }

        return ResourcePtr();
    }

    bool ResourceManager::getResources(const TString &strName,
        std::list<ResourcePtr> &rResList) const
    {
        bool bRet = false;
        auto i = m_ResourceCache.find(strName);

        if (i != m_ResourceCache.end())
        {
            const Resources &resources = i->second;

            auto itr = resources.begin();

            while (itr != resources.end())
            {
                rResList.push_back(itr->second);
                ++itr;
            }

            bRet = true;
        }

        return bRet;
    }
}

