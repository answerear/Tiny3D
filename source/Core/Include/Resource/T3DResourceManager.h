

#ifndef __T3D_RESOURCE_MANAGER_H__
#define __T3D_RESOURCE_MANAGER_H__


#include "T3DPrerequisites.h"
#include "T3DResource.h"


namespace Tiny3D
{
    class T3D_ENGINE_API ResourceManager
    {
    public:
        ResourceManager();
        virtual ~ResourceManager();

        /**
         * @brief Load resource from file.
         */
        virtual ResourcePtr load(const TString &strName);
        /**
         * @brief Unload resource in memory.
         */
        virtual void unload(ResourcePtr &pResource);

        /**
         * @brief Clone another new resource from source resource.
         * @param [in] pSrcRes
         *      Source resource used to clone.
         * @return retrieve a pointer to new instance of resource.
         * @remarks
         *      All Resource subclass should implement clone() method to
         *      new instance of resource. This method will call 
         *      Resource::clone() to implement this function.
         */
        ResourcePtr clone(ResourcePtr &pSrcRes);

        /**
         * @brief Get resource by name
         * @param [in] strName
         *      The name of resource.
         * @return retrieve a pointer to instance of resource.
         * @remarks
         *      If some resource is made by calling clone(), this instance of
         *      resource maybe not the instance that caller want. If caller
         *      want to the correct instance of resource, user should use
         *      getResource(const TString &) instead of 
         *      getResource(ulong_t, const TString &).
         */
        ResourcePtr getResource(const TString &strName) const;

        void addSearchPath(const TString &strPath);

    protected:
        virtual Resource *create(const TString &strName) = 0;

    protected:
        typedef std::vector<TString> SearchPathList;

        typedef std::map<TString, ResourcePtr>        ResourceMap;

        SearchPathList      m_SearchPathList;   /// search path list
        ResourceMap         m_Resources;        /// resources container
    };
}


#endif  /*__T3D_RESOURCE_MANAGER_H__*/
