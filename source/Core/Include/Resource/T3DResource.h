

#ifndef __T3D_RESOURCE_H__
#define __T3D_RESOURCE_H__


#include "T3DPrerequisites.h"


namespace Tiny3D
{
    class T3D_ENGINE_API Resource
    {
        friend class ResourceManager;

    public:
        enum EType
        {
            E_TYPE_UNKNOWN = 0,
            E_TYPE_DYLIB,
            E_TYPE_MESH,
            E_TYPE_MATERIAL,
            E_TYPE_TEXTURE,
        };

        Resource(const TString &strName);
        virtual ~Resource();

        virtual EType getType() const = 0;

        uint32_t getID() const
        {
            return m_unID;
        }

        uint32_t getCloneID() const
        {
            return m_unCloneID;
        }

        bool isCloned() const
        {
            return (m_unCloneID != 0);
        }

        size_t getSize() const
        {
            return m_unSize;
        }

        const TString &getName() const
        {
            return m_strName;
        }

        bool isLoaded() const
        {
            return m_bIsLoaded;
        }

    protected:
        virtual bool load() = 0;
        virtual void unload();
        virtual Resource *clone() const = 0;

    protected:
        size_t      m_unSize;       /** size of this resource */
        int32_t     m_unID;         /** ID of this resource */
        int32_t     m_unCloneID;    /** If this resource is cloned from another, this value is none zero */
        bool        m_bIsLoaded;    /** loaded flag */
        TString     m_strName;      /** name of the resource */
    };
}


#endif  /*__T3D_RESOURCE_H__*/
